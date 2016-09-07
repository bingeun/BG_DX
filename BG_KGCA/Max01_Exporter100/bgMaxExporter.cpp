#include "bgMaxExporter.h"

#define SINGLE_QUOTE	39	// (')
#define CTL_CHARS		31	//시스템 제어 문자

bgMaxExporter::bgMaxExporter()
{
}

bgMaxExporter::~bgMaxExporter()
{
}

void bgMaxExporter::Init(TSTR name, Interface* ip)
{
	m_szFileName = name;
	m_p3DSMax = ip;

	m_pRootNode = m_p3DSMax->GetRootNode();
	NodeProcess(m_pRootNode);

	m_Scene.iVersion = 100;
	m_Scene.iNumMtl = m_pMtlList.size();
	m_Scene.iNumObject = m_pObjectList.size();
	
	// 파일 열기
	m_pFile = _tfopen(m_szFileName, _T("wt"));
	_ftprintf(m_pFile, _T("%s %d\n"), _T("#KGCA_BG_3D_MODEL"), m_Scene.iVersion);
}

void bgMaxExporter::Release()
{
	// 파일 닫기
	fclose(m_pFile);
}

bool bgMaxExporter::Exporter()
{
	ExportScene();
	ExportMaterial();
	ExportObject();
	return true;
}

bool bgMaxExporter::ExportScene()
{
	m_Interval = m_p3DSMax->GetAnimRange();

	m_Scene.iTickPerFrame = GetTicksPerFrame();
	m_Scene.iFirstFrame = m_Interval.Start() / GetTicksPerFrame();
	m_Scene.iLastFrame = m_Interval.End() / GetTicksPerFrame();
	m_Scene.iFrameSpeed = GetFrameRate();

	//                      #  1  2  3  4  5  6
	_ftprintf(m_pFile, _T("%s %d %d %d %d %d %d\n"),
		_T("#SCENE"),				// #
		m_Scene.iFirstFrame,		// 1
		m_Scene.iLastFrame,			// 2
		m_Scene.iFrameSpeed,		// 3
		m_Scene.iTickPerFrame,		// 4
		m_Scene.iNumMtl,			// 5
		m_Scene.iNumObject			// 6
	);

	return true;
}

bool bgMaxExporter::ExportMaterial()
{
	Mtl* pMtl;

	for (int iMtl = 0; iMtl < m_pMtlList.size(); iMtl++)
	{
		bgMaxMtl MtlData;
		pMtl = m_pMtlList[iMtl];
		MtlData.szName = FixupName(pMtl->GetName());
		if (pMtl->NumSubMtls() > 0)
		{
			for (int iSubMtl = 0; iSubMtl < pMtl->NumSubMtls(); iSubMtl++)
			{
				Mtl* pSubMtl = pMtl->GetSubMtl(iSubMtl);
				bgMaxMtl tSubMtl;
				GetTexture(pSubMtl, tSubMtl);
				MtlData.SubMtlsList.push_back(tSubMtl);
			}
		}
		else
		{
			GetTexture(pMtl, MtlData);
		}
		m_MtlList.push_back(MtlData);
	}

	for (int iMtl = 0; iMtl < m_MtlList.size(); iMtl++)
	{
		//                      #  1  2  3
		_ftprintf(m_pFile, _T("%s %d %d %d\n"),
			_T("#MATERIAL"),				// #
			iMtl,							// 1
			m_MtlList[iMtl].SubMtlsList.size(),	// 2
			m_MtlList[iMtl].TexList.size()	// 3
		);

		if (m_MtlList[iMtl].SubMtlsList.size() > 0)
		{
			for (int iSubMtl = 0; iSubMtl < m_MtlList[iMtl].SubMtlsList.size(); iSubMtl++)
			{
				//                        #  1  2  3
				_ftprintf(m_pFile, _T("\t%s %d %d %d\n"),
					_T("#SUBMATERIAL"),											// #
					iSubMtl,													// 1
					m_MtlList[iMtl].SubMtlsList[iSubMtl].SubMtlsList.size(),	// 2
					m_MtlList[iMtl].SubMtlsList[iSubMtl].TexList.size()			// 3
				);

				for (int iTex = 0; iTex < m_MtlList[iMtl].SubMtlsList[iSubMtl].TexList.size(); iTex++)
				{
					//                          #  1  2
					_ftprintf(m_pFile, _T("\t\t%s %d %s\n"),
						_T("#TEXTURE"),													// #
						m_MtlList[iMtl].SubMtlsList[iSubMtl].TexList[iTex].iMapType,	// 1
						m_MtlList[iMtl].SubMtlsList[iSubMtl].TexList[iTex].szTexName	// 2
					);
				}
			}
		}
		else
		{
			for (int iTex = 0; iTex < m_MtlList[iMtl].TexList.size(); iTex++)
			{
				//                        #  1  2
				_ftprintf(m_pFile, _T("\t%s %d %s\n"),
					_T("#TEXTURE"),							// #
					m_MtlList[iMtl].TexList[iTex].iMapType,	// 1
					m_MtlList[iMtl].TexList[iTex].szTexName	// 2
				);
			}
		}
	}

	return true;
}

bool bgMaxExporter::ExportObject()
{
	for (int iObj = 0; iObj < m_ObjectList.size(); iObj++)
	{
		bgMaxMesh tMesh;
		INode* pINode = m_pObjectList[iObj];
		ObjectState os = pINode->EvalWorldState(m_Interval.Start());
		Object *obj = os.obj;
		if (obj)
		{
			switch (obj->SuperClassID())
			{
			case GEOMOBJECT_CLASS_ID:
			{
				tMesh.iType = 1;
			}
			break;
			case HELPER_CLASS_ID:
			{
				tMesh.iType = 0;
			}
			break;
			}
		}

		tMesh.NodeName = FixupName(pINode->GetName());
		INode* pParent = pINode->GetParentNode();
		tMesh.ParentNodeName = FixupName(pParent->GetName());
		Matrix3  matWorld = pINode->GetNodeTM(m_Interval.Start());
		DumpMatrix3(matWorld, tMesh.matWorld);

		GetMesh(pINode, tMesh);
		GetAnimation(pINode, tMesh);

		Mtl* pMtl = pINode->GetMtl();
		for (int iMtl = 0; iMtl < m_pMtlList.size(); iMtl++)
		{
			if (pMtl == m_pMtlList[iMtl])
			{
				tMesh.iRef = iMtl;
				break;
			}
		}
		m_ObjectList.push_back(tMesh);
	}
	return true;
}

void bgMaxExporter::NodeProcess(INode* pNode)
{
	if (pNode == NULL)
		return;

	AddMtl(pNode);
	AddObject(pNode);

	int iNumChild;
	INode* pChild;

	iNumChild = pNode->NumberOfChildren();
	for (int iChild = 0; iChild < iNumChild; iChild++)
	{
		pChild = pNode->GetChildNode(iChild);
		NodeProcess(pChild);
	}
}

void bgMaxExporter::AddMtl(INode* pNode)
{
	Mtl* pMtl = pNode->GetMtl();
	if (pMtl != NULL)
	{
		bool bAdd = true;
		for (int iFind = 0; iFind < m_pMtlList.size(); iFind++)
		{
			if (m_pMtlList[iFind] == pMtl)
			{
				bAdd = false;
				break;
			}
		}
		if (bAdd)
		{
			m_pMtlList.push_back(pMtl);
		}
	}
}

void bgMaxExporter::AddObject(INode* pNode)
{
	ObjectState os = pNode->EvalWorldState(m_Interval.Start());

	if (os.obj)
	{
		switch (os.obj->SuperClassID())
		{
		case GEOMOBJECT_CLASS_ID:
		case HELPER_CLASS_ID:
		{
			m_pObjectList.push_back(pNode);
		}
		break;
		}
	}
}

void bgMaxExporter::GetTexture(Mtl* pMtl, bgMaxMtl& MtlData)
{
	int iNumMap = pMtl->NumSubTexmaps();
	for (int iMap = 0; iMap < iNumMap; iMap++)
	{
		Texmap* pTex = pMtl->GetSubTexmap(iMap);
		if (pTex == NULL) continue;
		//if (bitmap == NULL) continue;

		if (pTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
		{
			bgMaxTex tex;
			tex.iMapType = iMap;
			BitmapTex* bitmap = (BitmapTex*)pTex;
			TSTR mapName = bitmap->GetMapName();
			SplitPathFile(mapName, NULL, &tex.szTexName);
			MtlData.TexList.push_back(tex);
		}
	}
}

void bgMaxExporter::GetMesh(INode* pNode, bgMaxMesh& tMesh)
{
	// INode -> TriObject -> Mesh
	BOOL needDel;
	TriObject* pTri = GetTriObjectFromNode(pNode, m_Interval.Start(), needDel);

	Matrix3 tm = pNode->GetObjTMAfterWSM(m_Interval.Start());

	if (pTri != NULL)
	{
		Mesh* pMesh = &(pTri->GetMesh());
		for (int iFace = 0; iFace < pMesh->numFaces; iFace++)
		{
			bgMaxTri tTri;

			if (pMesh->numVerts > 0)
			{
				Point3 v;
				v = pMesh->verts[pMesh->faces[iFace].v[0]] * tm;
				DumpPoint3(v, tTri.v[0].pos);
				v = pMesh->verts[pMesh->faces[iFace].v[2]] * tm;
				DumpPoint3(v, tTri.v[1].pos);
				v = pMesh->verts[pMesh->faces[iFace].v[1]] * tm;
				DumpPoint3(v, tTri.v[2].pos);
			}

			if (pMesh->numTVerts > 0)
			{
				Point2 t;
				t.x = pMesh->tVerts[pMesh->tvFace[iFace].t[0]].x;
				t.y = 1.0f - pMesh->tVerts[pMesh->tvFace[iFace].t[0]].y;
				DumpPoint2(t, tTri.v[0].tex);
				t.x = pMesh->tVerts[pMesh->tvFace[iFace].t[2]].x;
				t.y = 1.0f - pMesh->tVerts[pMesh->tvFace[iFace].t[2]].y;
				DumpPoint2(t, tTri.v[1].tex);
				t.x = pMesh->tVerts[pMesh->tvFace[iFace].t[1]].x;
				t.y = 1.0f - pMesh->tVerts[pMesh->tvFace[iFace].t[1]].y;
				DumpPoint2(t, tTri.v[2].tex);
			}
			tTri.v[0].col = Point4(1, 1, 1, 1);
			tTri.v[1].col = Point4(1, 1, 1, 1);
			tTri.v[2].col = Point4(1, 1, 1, 1);

			if (pMesh->numCVerts > 0)
			{
				Point3 c;
				c = pMesh->vertCol[pMesh->vcFace[iFace].t[0]];
				tTri.v[0].col.x = c.x;
				tTri.v[0].col.y = c.y;
				tTri.v[0].col.z = c.z;
				tTri.v[0].col.w = 1.0f;
				c = pMesh->vertCol[pMesh->vcFace[iFace].t[2]];
				tTri.v[1].col.x = c.x;
				tTri.v[1].col.y = c.y;
				tTri.v[1].col.z = c.z;
				tTri.v[1].col.w = 1.0f;
				c = pMesh->vertCol[pMesh->vcFace[iFace].t[1]];
				tTri.v[2].col.x = c.x;
				tTri.v[2].col.y = c.y;
				tTri.v[2].col.z = c.z;
				tTri.v[2].col.w = 1.0f;
			}
		}
	}
}

void bgMaxExporter::GetAnimation(INode* pNode, bgMaxMesh& tMesh)
{
}

TriObject* bgMaxExporter::GetTriObjectFromNode(INode* pNode, int iTick, BOOL& needDel)
{
	needDel = FALSE;

	// Object -> TriObject
	Object* pObj = pNode->EvalWorldState(iTick).obj;
	if (pObj)
	{
		if (pObj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
		{
			TriObject* pTri = (TriObject*)pObj->ConvertToType(iTick, Class_ID(TRIOBJ_CLASS_ID, 0));
			if (pObj != (Object*)pTri)
			{
				needDel = TRUE;
			}
			return pTri;
		}
	}
	return NULL;
}

void bgMaxExporter::DumpPoint2(Point2& v, Point2& Vertex)
{
	Vertex.x = v.x;
	Vertex.y = 1.0f - v.y;
}

void bgMaxExporter::DumpPoint3(Point3& v, Point3& Vertex)
{
	Vertex.x = v.x;
	Vertex.y = v.z;
	Vertex.z = v.y;
}

void bgMaxExporter::DumpPoint4(Point4& v, Point4& Vertex)
{
	Vertex.x = v.x;
	Vertex.y = v.z;
	Vertex.z = v.y;
	Vertex.w = v.w;
}

void bgMaxExporter::DumpQuat(Quat& v, Quat& q)
{
	q.x = v.x;
	q.y = v.z;
	q.z = v.y;
	q.w = v.w;
}

void bgMaxExporter::DumpMatrix3(Matrix3& matWorld, D3D_MATRIX& d3dWorld)
{
	Point3 row;

	row = matWorld.GetRow(0);
	d3dWorld._11 = row.x;
	d3dWorld._12 = row.z;
	d3dWorld._13 = row.y;
	d3dWorld._14 = 0.0f;

	row = matWorld.GetRow(2);
	d3dWorld._21 = row.x;
	d3dWorld._22 = row.z;
	d3dWorld._23 = row.y;
	d3dWorld._24 = 0.0f;

	row = matWorld.GetRow(1);
	d3dWorld._31 = row.x;
	d3dWorld._32 = row.z;
	d3dWorld._33 = row.y;
	d3dWorld._34 = 0.0f;

	row = matWorld.GetRow(3);
	d3dWorld._41 = row.x;
	d3dWorld._42 = row.z;
	d3dWorld._43 = row.y;
	d3dWorld._44 = 1.0f;
}

TSTR bgMaxExporter::FixupName(TSTR name)
{
	TCHAR m_tmpBuffer[MAX_PATH] = { 0, };
	memset(m_tmpBuffer, 0, sizeof(TCHAR) * MAX_PATH);

	TCHAR* cPtr;
	_tcscpy(m_tmpBuffer, name);
	cPtr = m_tmpBuffer;

	// "MAP #1" -> MAP#1
	while (*cPtr)
	{
		if (*cPtr == '"')
			*cPtr = SINGLE_QUOTE;
		else if (*cPtr == ' ' || *cPtr <= CTL_CHARS)
			*cPtr = _T('_');

		cPtr++;
	}
	return m_tmpBuffer;
}
