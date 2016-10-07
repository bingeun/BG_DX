#include "bgExporterSkin.h"

bgExporterSkin::bgExporterSkin()
{
	Init();
}

bgExporterSkin::~bgExporterSkin()
{
	Release();
}

bool bgExporterSkin::Init()
{
	return true;
}

bool bgExporterSkin::Release()
{
	m_BipedList.clear();
	for (vector<bgSkinMesh*>::iterator iter = m_SkinMeshList.begin(); iter != m_SkinMeshList.end(); iter++)
	{
		bgSkinMesh* pMesh = (*iter);
		if (pMesh->iMaterialRef > 0)
		{
			pMesh->m_pSkinSubMesh.clear();
			for (int imtl = 0; imtl < pMesh->iMaterialRef; imtl++)
			{
				//SAFE_DEL( pMesh->m_pSkinSubMesh[imtl] );
			}
		}
		SAFE_DEL(pMesh);
	}
	m_SkinMeshList.clear();

	return true;
}

TCHAR* bgExporterSkin::SaveFileDialog(TCHAR* szExt, TCHAR* szTitle)
{
	static TCHAR* szFilter;

	TCHAR szFile[256], szFileTitle[256];
	_tcscpy_s(szFile, _T("*."));
	_tcscat_s(szFile, szExt);
	_tcscat_s(szFile, _T("\0"));

	OPENFILENAME OpenFileName;
	ZeroMemory(&OpenFileName, sizeof(OPENFILENAME));
	OpenFileName.lStructSize = sizeof(OPENFILENAME);
	OpenFileName.hwndOwner = GetActiveWindow();
	OpenFileName.lpstrFilter = szFilter;
	OpenFileName.lpstrCustomFilter = NULL;
	OpenFileName.nMaxCustFilter = 0L;
	OpenFileName.nFilterIndex = 1;
	OpenFileName.lpstrFile = szFile;
	OpenFileName.nMaxFile = sizeof(szFile);
	OpenFileName.lpstrFileTitle = szFileTitle;
	OpenFileName.nMaxFileTitle = sizeof(szFileTitle);
	OpenFileName.lpstrInitialDir = NULL;
	OpenFileName.lpstrTitle = szTitle;
	OpenFileName.Flags = 0L;
	OpenFileName.nFileOffset = 0;
	OpenFileName.nFileExtension = 0;
	OpenFileName.lpstrDefExt = szExt;

	if (!GetSaveFileName(&OpenFileName))
	{
		return NULL;
	}

	return szFile;
}

TCHAR* bgExporterSkin::FixupName(MSTR name)
{
	TCHAR* szPtr = m_szTemp;

	memset(m_szTemp, 0, sizeof(TCHAR) * MAX_PATH);
	_tcscpy(m_szTemp, name);

	while (*szPtr)
	{
		if (*szPtr == '"')
			*szPtr = SINGLE_QUOTE;
		else if (*szPtr == ' ' || *szPtr <= CTL_CHARS)
			*szPtr = _T('_');

		szPtr++;
	}

	return m_szTemp;
}

int bgExporterSkin::IsEqualVertexList(vector<PNCT5_Vertex>& VertexArray, PNCT5_Vertex& Vertex)
{
	for (int iVertex = 0; iVertex < VertexArray.size(); iVertex++)
	{
		if (I_Exporter.EqualPoint3(VertexArray[iVertex].pos, Vertex.pos) &&
			I_Exporter.EqualPoint3(VertexArray[iVertex].nor, Vertex.nor) &&
			I_Exporter.EqualPoint4(VertexArray[iVertex].col, Vertex.col) &&
			I_Exporter.EqualPoint2(VertexArray[iVertex].tex, Vertex.tex))
		{
			return iVertex;
		}
	}
	return -1;
}

bool bgExporterSkin::GetNodeInfo(INode* node, TimeValue t)
{
	BOOL needDel;
	TriObject *tri = I_Exporter.GetTriObject(node, t, needDel);
	if (tri == NULL)
		return false;

	Mesh* mesh = &tri->GetMesh();
	mesh->buildNormals();
	if (!mesh->faces || !tri || !mesh || !mesh->getNumFaces() || !mesh->getNumVerts() || !mesh->getNumTVerts())
	{
		return false;
	}

	SetBipedInfo(node);

	bgSkinMesh*	SkinMesh = NULL;
	SAFE_NEW(SkinMesh, bgSkinMesh);

	GenerateGroup(node, mesh, SkinMesh);

	m_SkinMeshList.push_back(SkinMesh);

	if (needDel)
	{
		delete tri;
	}
	return true;
}

void bgExporterSkin::SetBipedInfo(INode* node)
{
	Modifier* phyMod = FindModifier(node, Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));
	Modifier* skinMod = FindModifier(node, SKIN_CLASSID);

	if (phyMod)
	{
		ExportPhysiqueData(node, phyMod);
	}
	else if (skinMod)
	{
		ExportSkinData(node, skinMod);
	}
}

void bgExporterSkin::GenerateGroup(INode* node, Mesh* mesh, bgSkinMesh* pSkinMesh)
{
	Mtl* mtl = node->GetMtl();
	pSkinMesh->strNodeName = node->GetName();

	if (mtl)
	{
		pSkinMesh->iMaterialRef = mtl->NumSubMtls();
	}
	if (pSkinMesh->iMaterialRef > 0)
	{
		for (int imtl = 0; imtl < pSkinMesh->iMaterialRef; imtl++)
		{
			bgSkinMesh* pSubSkinMesh = NULL;
			SAFE_NEW(pSubSkinMesh, bgSkinMesh);
			Mtl* subMtl = mtl->GetSubMtl(imtl);
			LoadMaterial(pSubSkinMesh, subMtl);
			SetTriangle(node, mesh, pSkinMesh, pSubSkinMesh, imtl);
			pSkinMesh->pSubMeshes.push_back(pSubSkinMesh);
		}
	}
	else
	{
		LoadMaterial(pSkinMesh, mtl);
		SetTriangle(node, mesh, pSkinMesh);
	}
}

Modifier* bgExporterSkin::FindModifier(INode *nodePtr, Class_ID classID)
{
	Object *ObjectPtr = nodePtr->GetObjectRef();
	if (!ObjectPtr)
	{
		return NULL;
	}
	while (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID && ObjectPtr)
	{
		IDerivedObject *DerivedObjectPtr = (IDerivedObject *)(ObjectPtr);

		int ModStackIndex = 0;
		while (ModStackIndex < DerivedObjectPtr->NumModifiers())
		{
			Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);

			if (ModifierPtr->ClassID() == classID)
			{
				return ModifierPtr;
			}

			ModStackIndex++;
		}
		ObjectPtr = DerivedObjectPtr->GetObjRef();
	}
	return NULL;
}

void bgExporterSkin::ExportPhysiqueData(INode* node, Modifier* phyMod)
{
	IPhysiqueExport *phyExport = (IPhysiqueExport *)phyMod->GetInterface(I_PHYEXPORT);
	IPhyContextExport *mcExport = (IPhyContextExport *)phyExport->GetContextInterface(node);

	mcExport->ConvertToRigid(true);
	mcExport->AllowBlending(true);

	IPhyBlendedRigidVertex	*rb_vtx = NULL;
	IPhyRigidVertex			*r_vtx = NULL;

	int numverts = mcExport->GetNumberVertices();

	ObjectState os = node->EvalWorldState(I_Exporter.m_p3DMax->GetTime());

	for (int i = 0; i < numverts; i++)
	{
		float totalWeight = 0.0f, weight = 0.0f;
		TSTR nodeName;

		IPhyVertexExport *vi = mcExport->GetVertexInterface(i);
		if (vi)
		{
			int type = vi->GetVertexType();
			switch (type)
			{
			case RIGID_BLENDED_TYPE:
			{
				rb_vtx = (IPhyBlendedRigidVertex*)vi;
				int iNode = rb_vtx->GetNumberNodes();
				bgBipedVertex BipVertex;
				BipVertex.m_dwNumWeight = iNode;

				for (int x = 0; x < iNode; x++)
				{
					INode* pNode = rb_vtx->GetNode(x);
					nodeName = pNode->GetName();
					BYTE Index = (BYTE)I_Exporter.GetIndex(nodeName);
					BipVertex.m_BipIDList.push_back(Index);

					float fWeight = rb_vtx->GetWeight(x);
					BipVertex.m_fWeightList.push_back(fWeight);

					if (Index < 0)
					{
						MessageBox(GetActiveWindow(), _T("No Biped ID."), _T("ERROR!!"), MB_OK | MB_ICONSTOP);
						break;
					}
				}
				if (m_Scene.iMaxWeight < iNode)
				{
					m_Scene.iMaxWeight = iNode;
					if (m_Scene.iMaxWeight > 8)
					{
						MessageBox(GetActiveWindow(), _T("바이패드+본+더미가 정점 당 8개 이상 적용되었습니다."), _T("최대 적용 에러!"), MB_OK | MB_ICONSTOP);
					}
				}
				m_BipedList.push_back(BipVertex);
			}
			break;
			case RIGID_NON_BLENDED_TYPE:
			{
				r_vtx = (IPhyRigidVertex*)vi;
				INode* pNode = r_vtx->GetNode();
				nodeName = pNode->GetName();

				bgBipedVertex BipVertex;
				BipVertex.m_dwNumWeight = 1;
				BYTE Index = (BYTE)I_Exporter.GetIndex(nodeName);
				BipVertex.m_BipIDList.push_back(Index);
				BipVertex.m_fWeightList.push_back(1.0f);

				// 최대 가중치 갯수를 저장
				if (m_Scene.iMaxWeight < 1)
				{
					m_Scene.iMaxWeight = 1;
				}
				m_BipedList.push_back(BipVertex);

				if (Index < 0)
				{
					MessageBox(GetActiveWindow(), _T("No Biped ID."), _T("ERROR!!"), MB_OK | MB_ICONSTOP);
					break;
				}
			}
			break;
			default:
				MessageBox(GetActiveWindow(), _T("Nothing!! Rigid Vertex Type"), _T("ERROR!!"), MB_OK | MB_ICONSTOP);
				break;
			}
			mcExport->ReleaseVertexInterface(vi);
		}
	}
	phyExport->ReleaseContextInterface(mcExport);
	phyMod->ReleaseInterface(I_PHYINTERFACE, phyExport);
}

void bgExporterSkin::ExportSkinData(INode* node, Modifier* skinMod)
{
	if (node == NULL || skinMod == NULL) return;
	ISkin *skin = (ISkin *)skinMod->GetInterface(I_SKIN);
	ISkinContextData *skinData = skin->GetContextInterface(node);

	if (skin && skinData)
	{
		int numberOfPoints;
		numberOfPoints = skinData->GetNumPoints();
		for (int i = 0; i < numberOfPoints; i++)
		{
			int numOfWeights = skinData->GetNumAssignedBones(i);

			bgBipedVertex BipVertex;
			BipVertex.m_dwNumWeight = numOfWeights;

			for (int j = 0; j < numOfWeights; j++)
			{
				int boneIndex = skinData->GetAssignedBone(i, j);
				INode * pBone = skin->GetBone(boneIndex);
				if (pBone == NULL)  break;
				const TCHAR* nodeName = nodeName = pBone->GetName();
				float boneWeight = skinData->GetBoneWeight(i, j);

				BYTE Index = (BYTE)I_Exporter.GetIndex(nodeName);
				BipVertex.m_BipIDList.push_back(Index);
				BipVertex.m_fWeightList.push_back(boneWeight);

				if (Index < 0)
				{
					MessageBox(GetActiveWindow(), _T("No Biped ID."), _T("ERROR!!"), MB_OK | MB_ICONSTOP);
					break;
				}
			}
			m_BipedList.push_back(BipVertex);

			if (m_Scene.iMaxWeight < numOfWeights)
			{
				m_Scene.iMaxWeight = numOfWeights;
				if (m_Scene.iMaxWeight > 8)
				{
					MessageBox(GetActiveWindow(), _T("바이패드+본+더미가 정점 당 8개 이상 적용되었습니다."), _T("최대 적용 에러!"), MB_OK | MB_ICONSTOP);
				}
			}
		}
	}
}

void bgExporterSkin::LoadMaterial(bgSkinMesh* pMesh, Mtl* mtl)
{
	if (mtl)
	{
		pMesh->m_iNumTex = 0;
		memset(pMesh->m_szTexName, 0, sizeof(TCHAR) * 13 * 30);
		for (int i = 0; i < mtl->NumSubTexmaps(); i++)
		{
			Texmap* subTex = mtl->GetSubTexmap(i);
			float amt = 1.0f;
			if (subTex)
			{
				if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
				{
					if (!((StdMat*)mtl)->MapEnabled(i))
						continue;
				}
				if (subTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00))
				{
					pMesh->m_iTexType[pMesh->m_iNumTex] = GetMapID(mtl->ClassID(), i);
					TSTR mapName = ((BitmapTex *)subTex)->GetMapName();
					TCHAR Drive[MAX_PATH];
					TCHAR Dir[MAX_PATH];
					TCHAR FName[MAX_PATH];
					TCHAR Ext[MAX_PATH];
					_tsplitpath_s(I_Exporter.FixupName(mapName), Drive, Dir, FName, Ext);
					Ext[4] = 0;
					_stprintf_s(pMesh->m_szTexName[pMesh->m_iNumTex], _T("%s%s"), FName, Ext);
					pMesh->m_iNumTex++;
				}
			}
		}
	}
}

int bgExporterSkin::GetMapID(Class_ID cid, int subNo)
{
	int iResultIndex = 1;
	if (cid == Class_ID(0, 0))
	{
		iResultIndex = 30;
	}
	else if (cid == Class_ID(DMTL_CLASS_ID, 0))
	{
		switch (subNo)
		{
		case ID_AM: iResultIndex = 0; break;
		case ID_DI: iResultIndex = 1; break;
		case ID_SP: iResultIndex = 2; break;
		case ID_SH: iResultIndex = 3; break;
		case ID_SS: iResultIndex = 4; break;
		case ID_SI: iResultIndex = 5; break;
		case ID_OP: iResultIndex = 6; break;
		case ID_FI: iResultIndex = 7; break;
		case ID_BU: iResultIndex = 8; break;
		case ID_RL: iResultIndex = 9; break;
		case ID_RR: iResultIndex = 10; break;
		}
	}
	else
	{
		iResultIndex = 31;
	}
	return iResultIndex;
}

void bgExporterSkin::SetTriangle(INode* node, Mesh* mesh, bgSkinMesh* pSkinMesh, bgSkinMesh* pSubMesh, int iMtrl)
{
	bgSkinMesh* pMesh = NULL;
	if (pSubMesh == NULL)
	{
		pMesh = pSkinMesh;
	}
	else
	{
		pMesh = pSubMesh;
	}

	Matrix3 tm = node->GetObjTMAfterWSM(I_Exporter.m_Interval.Start());
	Matrix3 nodetm = node->GetNodeTM(I_Exporter.m_Interval.Start());
	I_Exporter.DumpMatrix3(&nodetm, pSkinMesh->matWorld);

	int v0, v1, v2;
	v0 = 0, v1 = 1, v2 = 2;
	if (I_Exporter.CheckNegativeTM(tm))
	{
		v0 = 2, v1 = 1, v2 = 0;
	}
	Matrix3 invTM = Inverse(nodetm);
	for (int i = 0; i < mesh->getNumFaces(); i++)
	{
		if (pMesh->iMaterialRef > 0 && mesh->faces[i].getMatID() != iMtrl)
		{
			continue;
		}

		bgSkinTri Triangle;
		memset(&Triangle, 0, sizeof(bgTriangle));
		Triangle.iSubIndex = mesh->faces[i].getMatID();

		if (mesh->getNumVerts() > 0)
		{
			SetVertexBiped(node, &mesh->faces[i], v0, v1, v2, &Triangle);
			for (int iVertex = 0; iVertex < 3; iVertex++)
			{
				Point3 v;
				if (iVertex == 0)		v = tm * mesh->verts[mesh->faces[i].v[v0]];
				else if (iVertex == 1)	v = tm * mesh->verts[mesh->faces[i].v[v2]];
				else					v = tm * mesh->verts[mesh->faces[i].v[v1]];
				Triangle.m_vVertex[iVertex].pos.x = v.x;
				Triangle.m_vVertex[iVertex].pos.y = v.z;
				Triangle.m_vVertex[iVertex].pos.z = v.y;
			}
		}

		if (mesh->getNumTVerts() > 0)
		{
			Triangle.m_vVertex[0].tex.x = mesh->tVerts[mesh->tvFace[i].t[v0]].x;
			Triangle.m_vVertex[0].tex.y = 1.0f - mesh->tVerts[mesh->tvFace[i].t[v0]].y;
			Triangle.m_vVertex[1].tex.x = mesh->tVerts[mesh->tvFace[i].t[v2]].x;
			Triangle.m_vVertex[1].tex.y = 1.0f - mesh->tVerts[mesh->tvFace[i].t[v2]].y;
			Triangle.m_vVertex[2].tex.x = mesh->tVerts[mesh->tvFace[i].t[v1]].x;
			Triangle.m_vVertex[2].tex.y = 1.0f - mesh->tVerts[mesh->tvFace[i].t[v1]].y;
		}

		Triangle.m_vVertex[0].col = Point4(1.0f, 1.0f, 1.0f, 1.0f);
		Triangle.m_vVertex[1].col = Point4(1.0f, 1.0f, 1.0f, 1.0f);
		Triangle.m_vVertex[2].col = Point4(1.0f, 1.0f, 1.0f, 1.0f);
		if (mesh->getNumVertCol() > 0)
		{
			Triangle.m_vVertex[0].col.w = 0xff;
			Triangle.m_vVertex[0].col.x = mesh->vertCol[mesh->vcFace[i].t[v0]].x;
			Triangle.m_vVertex[0].col.y = mesh->vertCol[mesh->vcFace[i].t[v0]].y;
			Triangle.m_vVertex[0].col.z = mesh->vertCol[mesh->vcFace[i].t[v0]].z;

			Triangle.m_vVertex[1].col.w = 0xff;
			Triangle.m_vVertex[1].col.x = mesh->vertCol[mesh->vcFace[i].t[v2]].x;
			Triangle.m_vVertex[1].col.y = mesh->vertCol[mesh->vcFace[i].t[v2]].y;
			Triangle.m_vVertex[1].col.z = mesh->vertCol[mesh->vcFace[i].t[v2]].z;

			Triangle.m_vVertex[2].col.w = 0xff;
			Triangle.m_vVertex[2].col.x = mesh->vertCol[mesh->vcFace[i].t[v1]].x;
			Triangle.m_vVertex[2].col.y = mesh->vertCol[mesh->vcFace[i].t[v1]].y;
			Triangle.m_vVertex[2].col.z = mesh->vertCol[mesh->vcFace[i].t[v1]].z;
		}

		Point3 vn, fn;
		int  vert;
		Face* f;

		f = &mesh->faces[i];
		vert = f->getVert(v0);
		vn = I_Exporter.GetVertexNormal(mesh, i, mesh->getRVertPtr(vert));

		Triangle.m_vVertex[0].nor.x = vn.x;
		Triangle.m_vVertex[0].nor.y = vn.z;
		Triangle.m_vVertex[0].nor.z = vn.y;
		vert = f->getVert(v2);
		vn = I_Exporter.GetVertexNormal(mesh, i, mesh->getRVertPtr(vert));

		Triangle.m_vVertex[1].nor.x = vn.x;
		Triangle.m_vVertex[1].nor.y = vn.z;
		Triangle.m_vVertex[1].nor.z = vn.y;
		vert = f->getVert(v1);
		vn = I_Exporter.GetVertexNormal(mesh, i, mesh->getRVertPtr(vert));

		Triangle.m_vVertex[2].nor.x = vn.x;
		Triangle.m_vVertex[2].nor.y = vn.z;
		Triangle.m_vVertex[2].nor.z = vn.y;

		pMesh->m_SkinTriList.push_back(Triangle);
	}
	pMesh->iNumFace = pMesh->m_SkinTriList.size();
}

void bgExporterSkin::SetVertexBiped(INode* node, Face*	face, int v0, int v1, int v2, bgSkinTri* pTri)
{
	int iNumWeight = 1;
	if (m_BipedList.size())
	{
		iNumWeight = m_BipedList[face->v[v0]].m_BipIDList.size();
		for (int iBip = 0; iBip < iNumWeight; iBip++)
		{
			if (iBip < 4)
			{
				pTri->m_vVertex[0].i1[iBip] = m_BipedList[face->v[v0]].m_BipIDList[iBip];
				pTri->m_vVertex[0].w1[iBip] = m_BipedList[face->v[v0]].m_fWeightList[iBip];
			}
			else
			{
				pTri->m_vVertex[0].i2[iBip - 4] = m_BipedList[face->v[v0]].m_BipIDList[iBip];
				pTri->m_vVertex[0].w2[iBip - 4] = m_BipedList[face->v[v0]].m_fWeightList[iBip];
			}
		}
		iNumWeight = m_BipedList[face->v[v2]].m_BipIDList.size();
		for (int iBip = 0; iBip < iNumWeight; iBip++)
		{
			if (iBip < 4)
			{
				pTri->m_vVertex[1].i1[iBip] = m_BipedList[face->v[v2]].m_BipIDList[iBip];
				pTri->m_vVertex[1].w1[iBip] = m_BipedList[face->v[v2]].m_fWeightList[iBip];
			}
			else
			{
				pTri->m_vVertex[1].i2[iBip - 4] = m_BipedList[face->v[v2]].m_BipIDList[iBip];
				pTri->m_vVertex[1].w2[iBip - 4] = m_BipedList[face->v[v2]].m_fWeightList[iBip];
			}
		}
		iNumWeight = m_BipedList[face->v[v1]].m_BipIDList.size();
		for (int iBip = 0; iBip < iNumWeight; iBip++)
		{
			if (iBip < 4)
			{
				pTri->m_vVertex[2].i1[iBip] = m_BipedList[face->v[v1]].m_BipIDList[iBip];
				pTri->m_vVertex[2].w1[iBip] = m_BipedList[face->v[v1]].m_fWeightList[iBip];
			}
			else
			{
				pTri->m_vVertex[2].i2[iBip - 4] = m_BipedList[face->v[v1]].m_BipIDList[iBip];
				pTri->m_vVertex[2].w2[iBip - 4] = m_BipedList[face->v[v1]].m_fWeightList[iBip];
			}
		}
		pTri->m_vVertex[0].w2[3] = m_BipedList[face->v[v0]].m_dwNumWeight;
		pTri->m_vVertex[1].w2[3] = m_BipedList[face->v[v2]].m_dwNumWeight;
		pTri->m_vVertex[2].w2[3] = m_BipedList[face->v[v1]].m_dwNumWeight;
	}
	else
	{
		int iBipIndex = I_Exporter.GetIndex(node->GetName());
		pTri->m_vVertex[0].i1[0] = iBipIndex;
		pTri->m_vVertex[0].w1[0] = 1.0f;
		pTri->m_vVertex[1].i1[0] = iBipIndex;
		pTri->m_vVertex[1].w1[0] = 1.0f;
		pTri->m_vVertex[2].i1[0] = iBipIndex;
		pTri->m_vVertex[2].w1[0] = 1.0f;
		pTri->m_vVertex[0].w2[3] = 1.0f;
		pTri->m_vVertex[1].w2[3] = 1.0f;
		pTri->m_vVertex[2].w2[3] = 1.0f;
	}
}
