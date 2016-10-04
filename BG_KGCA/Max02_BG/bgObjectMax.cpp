#include "bgObjectMax.h"

bgObjectMax::bgObjectMax()
{
}

bgObjectMax::~bgObjectMax()
{
}

void bgObjectMax::GetMaterial()
{
	for (int iMtl = 0; iMtl < m_pMtlList.size(); iMtl++)
	{
		Mtl* pSrcMtl = m_pMtlList[iMtl];
		bgMaterial AddMtl;
		TSTR className;
		pSrcMtl->GetClassName(className);
		AddMtl.strName = FixupName(pSrcMtl->GetName());
		AddMtl.strClassName = FixupName(className);
		AddMtl.iNumSubMaterial = pSrcMtl->NumSubMtls();
		AddMtl.iIndex = iMtl;

		if (pSrcMtl->NumSubMtls() > 0)
		{
			for (int iSubMtrl = 0; iSubMtrl< pSrcMtl->NumSubMtls(); iSubMtrl++)
			{
				Mtl* subMtl = pSrcMtl->GetSubMtl(iSubMtrl);
				bgMaterial AddSubMtl;
				AddSubMtl.iIndex = iSubMtrl;
				AddSubMtl.iNumSubMaterial = subMtl->NumSubMtls();
				AddSubMtl.strName = FixupName(subMtl->GetName());
				TSTR className;
				subMtl->GetClassName(className);
				AddSubMtl.strClassName = FixupName(className);

				if (subMtl->NumSubMtls() > 0)
				{
					MessageBox(0, _T("다중 서브 매터리얼"), _T("Fatal error"), MB_OK);
				}
				if (subMtl)
				{
					GetTexture(&AddSubMtl, subMtl);
				}
				AddMtl.SubMaterials.push_back(AddSubMtl);
			}
		}
		else
		{
			GetTexture(&AddMtl, pSrcMtl);
		}
		m_MaterialList.push_back(AddMtl);
	}
}

void bgObjectMax::GetTexture(bgMaterial* pMaterial, Mtl* pMtl)
{
	for (int iSubMap = 0; iSubMap<pMtl->NumSubTexmaps(); iSubMap++)
	{
		Texmap* tex = pMtl->GetSubTexmap(iSubMap);
		if (tex)
		{
			if (pMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
			{
				StdMat* std = (StdMat*)pMtl;
				if (!std->MapEnabled(iSubMap))
					continue;
			}

			bgTextureMap TexMap;
			TexMap.iType = iSubMap;

			if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00))
			{
				TSTR mapName = ((BitmapTex *)tex)->GetMapName();
				TSTR strFullName;
				SplitPathFile(mapName, &strFullName, &mapName);
				TexMap.strTextureName = FixupName(mapName);
			}
			pMaterial->TextureMaps.push_back(TexMap);
			pMaterial->iNumTextureMap = (int)pMaterial->TextureMaps.size();
		}
	}
}

void bgObjectMax::UpdateObject()
{
	for (int iObj = 0; iObj < m_MatrixMap.Count(); iObj++)
	{
		bgMatrixIndex *pPoint = (bgMatrixIndex *)m_MatrixMap.m_TMap[iObj];
		GetObject(pPoint);
	}
}

void bgObjectMax::GetObject(bgMatrixIndex* pPoint)
{
	bgMesh* pMesh = &pPoint->m_Mesh;
	pMesh->ClassType = pPoint->m_ClassType;
	GetNodeHeader(pPoint->m_pINode, pMesh);
	GetNodeTM(pPoint->m_pINode, pMesh);
	GetMesh(pPoint->m_pINode, pMesh);
	GetAnimKeys(pPoint->m_pINode, pMesh);
}

void bgObjectMax::GetNodeHeader(INode* node, bgMesh* pMesh)
{
	pMesh->strNodeName = FixupName(node->GetName());
	INode* parent = node->GetParentNode();
	if (parent && (!parent->IsRootNode()))
	{
		pMesh->strParentName = FixupName(parent->GetName());
	}
}

void bgObjectMax::GetNodeTM(INode* node, bgMesh* pMesh)
{
	Matrix3 matWorld = node->GetNodeTM(m_Interval.Start());
	DumpMatrix3(&matWorld, pMesh->matWorld);
}

void bgObjectMax::GetMesh(INode* node, bgMesh* pMesh)
{
	Matrix3 tm = node->GetObjTMAfterWSM(m_Interval.Start());
	ObjectState os = node->EvalWorldState(m_Interval.Start());
	if (!os.obj || os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID)
		return;

	BOOL needDel;
	TriObject* tri = GetTriObject(node, m_Interval.Start(), needDel);
	if (!tri)
		return;

	Mesh* mesh = &tri->GetMesh();
	mesh->buildBoundingBox();
	Box3 box = mesh->getBoundingBox(&tm);
	DumpPoint3(pMesh->Box.pmax, box.pmax);
	DumpPoint3(pMesh->Box.pmin, box.pmin);

	pMesh->iNumFace = mesh->getNumFaces();

	BOOL negScale = CheckNegativeTM(tm);
	int v0, v1, v2;
	if (negScale) { v0 = 2;		v1 = 1;		v2 = 0; }
	else { v0 = 0;		v1 = 1;		v2 = 2; }

	pMesh->Triangles.resize(pMesh->iNumFace);
	for (int iFace = 0; iFace < pMesh->iNumFace; iFace++)
	{
		Matrix3 pInverse = Inverse(node->GetNodeTM(m_Interval.Start()));
		Point3 v;
		if (mesh->getNumVerts() > 0)
		{
			v = mesh->verts[mesh->faces[iFace].v[v0]] * tm * pInverse;
			DumpPoint3(pMesh->Triangles[iFace].vertex[0].pos, v);

			v = mesh->verts[mesh->faces[iFace].v[v2]] * tm * pInverse;
			DumpPoint3(pMesh->Triangles[iFace].vertex[1].pos, v);

			v = mesh->verts[mesh->faces[iFace].v[v1]] * tm * pInverse;
			DumpPoint3(pMesh->Triangles[iFace].vertex[2].pos, v);
		}
		if (mesh->getNumTVerts() > 0)
		{
			pMesh->Triangles[iFace].vertex[0].tex.x = mesh->tVerts[mesh->tvFace[iFace].t[v0]].x;
			pMesh->Triangles[iFace].vertex[0].tex.y = 1.0f - mesh->tVerts[mesh->tvFace[iFace].t[v0]].y;
			pMesh->Triangles[iFace].vertex[1].tex.x = mesh->tVerts[mesh->tvFace[iFace].t[v2]].x;
			pMesh->Triangles[iFace].vertex[1].tex.y = 1.0f - mesh->tVerts[mesh->tvFace[iFace].t[v2]].y;
			pMesh->Triangles[iFace].vertex[2].tex.x = mesh->tVerts[mesh->tvFace[iFace].t[v1]].x;
			pMesh->Triangles[iFace].vertex[2].tex.y = 1.0f - mesh->tVerts[mesh->tvFace[iFace].t[v1]].y;
		}

		pMesh->Triangles[iFace].vertex[0].col = Point4(1, 1, 1, 1);
		pMesh->Triangles[iFace].vertex[1].col = Point4(1, 1, 1, 1);
		pMesh->Triangles[iFace].vertex[2].col = Point4(1, 1, 1, 1);

		if (mesh->getNumVertCol() > 0)
		{
			pMesh->Triangles[iFace].vertex[0].col.x = mesh->vertCol[mesh->vcFace[iFace].t[v0]].x;
			pMesh->Triangles[iFace].vertex[0].col.y = mesh->vertCol[mesh->vcFace[iFace].t[v0]].y;
			pMesh->Triangles[iFace].vertex[0].col.z = mesh->vertCol[mesh->vcFace[iFace].t[v0]].z;
			pMesh->Triangles[iFace].vertex[0].col.w = 1.0f;
			pMesh->Triangles[iFace].vertex[1].col.x = mesh->vertCol[mesh->vcFace[iFace].t[v2]].x;
			pMesh->Triangles[iFace].vertex[1].col.y = mesh->vertCol[mesh->vcFace[iFace].t[v2]].y;
			pMesh->Triangles[iFace].vertex[1].col.z = mesh->vertCol[mesh->vcFace[iFace].t[v2]].z;
			pMesh->Triangles[iFace].vertex[1].col.w = 1.0f;
			pMesh->Triangles[iFace].vertex[2].col.x = mesh->vertCol[mesh->vcFace[iFace].t[v1]].x;
			pMesh->Triangles[iFace].vertex[2].col.y = mesh->vertCol[mesh->vcFace[iFace].t[v1]].y;
			pMesh->Triangles[iFace].vertex[2].col.z = mesh->vertCol[mesh->vcFace[iFace].t[v1]].z;
			pMesh->Triangles[iFace].vertex[2].col.w = 1.0f;
		}

		mesh->buildNormals();

		int  vert = mesh->faces[iFace].getVert(v0);
		Point3 vn = GetVertexNormal(mesh, iFace, mesh->getRVertPtr(vert));
		DumpPoint3(pMesh->Triangles[iFace].vertex[0].nor, vn);

		vert = mesh->faces[iFace].getVert(v2);
		vn = GetVertexNormal(mesh, iFace, mesh->getRVertPtr(vert));
		DumpPoint3(pMesh->Triangles[iFace].vertex[1].nor, vn);

		vert = mesh->faces[iFace].getVert(v1);
		vn = GetVertexNormal(mesh, iFace, mesh->getRVertPtr(vert));
		DumpPoint3(pMesh->Triangles[iFace].vertex[2].nor, vn);

		pMesh->Triangles[iFace].iSubIndex = mesh->faces[iFace].getMatID();
	}
	if (needDel)
	{
		delete tri;
	}

	pMesh->iMaterialRef = GetMtlRef(node->GetMtl());
}

Point3 bgObjectMax::GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
{
	Face* f = &mesh->faces[faceNo];
	DWORD smGroup = f->smGroup;
	int numNormals = rv->rFlags & NORCT_MASK;
	Point3 vertexNormal;

	if (rv->rFlags & SPECIFIED_NORMAL)
	{
		vertexNormal = rv->rn.getNormal();
	}
	else if (numNormals && smGroup)
	{
		if (numNormals == 1)
		{
			vertexNormal = rv->rn.getNormal();
		}
		else
		{
			for (int i = 0; i < numNormals; i++)
			{
				if (rv->ern[i].getSmGroup() & smGroup)
				{
					vertexNormal = rv->ern[i].getNormal();
				}
			}
		}
	}
	else
	{
		vertexNormal = mesh->getFaceNormal(faceNo);
	}
	return vertexNormal;
}

TriObject* bgObjectMax::GetTriObject(INode* node, TimeValue time, int& deleteIt)
{
	deleteIt = FALSE;
	Object *obj = node->EvalWorldState(time).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
	{
		TriObject* tri = (TriObject*)obj->ConvertToType(time, Class_ID(TRIOBJ_CLASS_ID, 0));
		if (obj != tri)
			deleteIt = TRUE;
		return tri;
	}
	else
	{
		return NULL;
	}
}
