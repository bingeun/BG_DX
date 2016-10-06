#include "bgExporterMax.h"

bool bgExporterMax::ExportBGMTX()
{
	m_strFileName = SaveFileDialog(_T("BGMTX"), _T("BG Matrix Export"));

	m_pStream = _tfopen(m_strFileName, _T("wb"));
	if (!m_pStream)
		return 0;

	struct tm *newtime;
	time_t aclock;
	time(&aclock);
	newtime = localtime(&aclock);
	fwrite(newtime, sizeof(tm), 1, m_pStream);

	bgScene scene = m_Scene;
	scene.iNumMesh = m_MatrixMap.Count();
	fwrite(&scene, sizeof(bgScene), 1, m_pStream);

	for (DWORD iBiped = 0; iBiped < m_MatrixMap.Count(); iBiped++)
	{
		bgMatrixIndex *pPoint = (bgMatrixIndex *)m_MatrixMap.m_TMap[iBiped];
		INode* pNode = pPoint->m_pINode;
		Matrix3		m_pAnimTm;
		D3D_MATRIX	m_pAnimation;

		for (int iFrame = m_Scene.iFirstFrame; iFrame < m_Scene.iLastFrame; iFrame++)
		{
			TimeValue iTime = iFrame * m_Scene.iTickPerFrame;
			Matrix3 m_pAnimTm = pNode->GetNodeTM(iTime);
			if (DotProd(CrossProd(m_pAnimTm.GetRow(0), m_pAnimTm.GetRow(1)), m_pAnimTm.GetRow(2)) < 0.0)
			{
				Matrix3 itm;
				itm.IdentityMatrix();
				itm.SetScale(Point3(-1.0f, -1.0f, -1.0f));
				m_pAnimTm = itm * m_pAnimTm;
			}
			DumpMatrix3(&m_pAnimTm, m_pAnimation);
			fwrite(&m_pAnimation, sizeof(D3D_MATRIX), 1, m_pStream);
		}
	}
	// 바이패드 랜더링에 사용.
	for (DWORD iBiped = 0; iBiped < m_MatrixMap.Count(); iBiped++)
	{
		bgMatrixIndex *pPoint = (bgMatrixIndex *)m_MatrixMap.GetPtr(iBiped);
		INode* pNode = pPoint->m_pINode;

		TimeValue	t = this->m_Interval.Start();

		if (pPoint->m_ClassType == CLASS_BIPED || pPoint->m_ClassType == CLASS_GEOM)
		{
			BOOL needDel;
			TriObject	*tri = GetTriObject(pPoint->m_pINode, t, needDel);
			if (tri != NULL)
			{
				Matrix3 tm = pPoint->m_pINode->GetObjTMAfterWSM(m_p3DMax->GetTime());
				Mesh* mesh = &tri->GetMesh();
				mesh->buildNormals();
				pPoint->m_VertexList.resize(mesh->getNumFaces() * 3);
				for (int iFace = 0; iFace < mesh->getNumFaces(); iFace++)
				{
					for (int iVertex = 0; iVertex < 3; iVertex++)
					{
						P3NC_Vertex vVertex;
						Point3 v;
						if (iVertex == 0)		v = tm * mesh->verts[mesh->faces[iFace].v[0]];
						else if (iVertex == 1)	v = tm * mesh->verts[mesh->faces[iFace].v[2]];
						else					v = tm * mesh->verts[mesh->faces[iFace].v[1]];
						vVertex.posX = v.x;
						vVertex.posY = v.z;
						vVertex.posZ = v.y;
						pPoint->m_VertexList[iFace * 3 + iVertex] = vVertex;
					}
				}
			}
		}
		else if (pPoint->m_ClassType == CLASS_BONE || pPoint->m_ClassType == CLASS_DUMMY)
		{
			Object* helperObj = pPoint->m_pObject;
			Matrix3 oTM = pPoint->m_pINode->GetObjectTM(t);
			Box3 bbox;
			helperObj->GetDeformBBox(t, bbox, &oTM);

			P3NC_Vertex vVertex[2];
			memset(vVertex, 0, sizeof(P3NC_Vertex) * 2);
			vVertex[0].posX = bbox.pmax.x;
			vVertex[0].posY = bbox.pmax.z;
			vVertex[0].posZ = bbox.pmax.y;
			vVertex[1].posX = bbox.pmin.x;
			vVertex[1].posY = bbox.pmin.z;
			vVertex[1].posZ = bbox.pmin.y;
			vVertex[0].col = Point4(1.0f, 1.0f, 0.0f, 0.0f);
			vVertex[1].col = Point4(1.0f, 1.0f, 0.0f, 0.0f);
			pPoint->m_VertexList.push_back(vVertex[0]);
			pPoint->m_VertexList.push_back(vVertex[1]);
		}

		int iLen = pPoint->m_szName.size();
		fwrite(&pPoint->m_ClassType, sizeof(int), 1, m_pStream);
		fwrite(&iLen, sizeof(int), 1, m_pStream);
		fwrite(pPoint->m_szName.c_str(), sizeof(TCHAR) * iLen, 1, m_pStream);

		D3D_MATRIX matWorld;
		DumpMatrix3(&pPoint->m_pINode->GetNodeTM(t), matWorld);
		fwrite(&matWorld, sizeof(D3D_MATRIX), 1, m_pStream);

		int iCount = pPoint->m_VertexList.size();
		fwrite(&iCount, sizeof(int), 1, m_pStream);

		for (int iVertex = 0; iVertex < pPoint->m_VertexList.size(); iVertex++)
		{
			fwrite(&pPoint->m_VertexList[iVertex], sizeof(P3NC_Vertex), 1, m_pStream);
		}
	}

	fclose(m_pStream);
	MessageBox(GetActiveWindow(), m_strFileName, _T("Succeed!!"), MB_OK);

	return true;
}

// 바이너리 출력 원본코드
//bool bgExporterMax::ExportBGMTX()
//{
//	m_strFileName = SaveFileDialog(_T("BGMTX"), _T("BG Matrix Export"));
//
//	m_pStream = _tfopen(m_strFileName, _T("wb"));
//	if (!m_pStream)
//		return 0;
//
//	struct tm *newtime;
//	time_t aclock;
//	time(&aclock);
//	newtime = localtime(&aclock);
//	fwrite(newtime, sizeof(tm), 1, m_pStream);
//
//	bgScene scene = m_Scene;
//	scene.iNumMesh = m_MatrixMap.Count();
//	fwrite(&scene, sizeof(bgScene), 1, m_pStream);
//
//	for (DWORD iBiped = 0; iBiped < m_MatrixMap.Count(); iBiped++)
//	{
//		bgMatrixIndex *pPoint = (bgMatrixIndex *)m_MatrixMap.m_TMap[iBiped];
//		INode* pNode = pPoint->m_pINode;
//		Matrix3		m_pAnimTm;
//		D3D_MATRIX	m_pAnimation;
//
//		for (int iFrame = m_Scene.iFirstFrame; iFrame < m_Scene.iLastFrame; iFrame++)
//		{
//			TimeValue iTime = iFrame * m_Scene.iTickPerFrame;
//			Matrix3 m_pAnimTm = pNode->GetNodeTM(iTime);
//			if (DotProd(CrossProd(m_pAnimTm.GetRow(0), m_pAnimTm.GetRow(1)), m_pAnimTm.GetRow(2)) < 0.0)
//			{
//				Matrix3 itm;
//				itm.IdentityMatrix();
//				itm.SetScale(Point3(-1.0f, -1.0f, -1.0f));
//				m_pAnimTm = itm * m_pAnimTm;
//			}
//			DumpMatrix3(&m_pAnimTm, m_pAnimation);
//			fwrite(&m_pAnimation, sizeof(D3D_MATRIX), 1, m_pStream);
//		}
//	}
//	// 바이패드 랜더링에 사용.
//	for (DWORD iBiped = 0; iBiped < m_MatrixMap.Count(); iBiped++)
//	{
//		bgMatrixIndex *pPoint = (bgMatrixIndex *)m_MatrixMap.GetPtr(iBiped);
//		INode* pNode = pPoint->m_pINode;
//
//		TimeValue	t = this->m_Interval.Start();
//
//		if (pPoint->m_ClassType == CLASS_BIPED || pPoint->m_ClassType == CLASS_GEOM)
//		{
//			BOOL needDel;
//			TriObject	*tri = GetTriObject(pPoint->m_pINode, t, needDel);
//			if (tri != NULL)
//			{
//				Matrix3 tm = pPoint->m_pINode->GetObjTMAfterWSM(m_p3DMax->GetTime());
//				Mesh* mesh = &tri->GetMesh();
//				mesh->buildNormals();
//				pPoint->m_VertexList.resize(mesh->getNumFaces() * 3);
//				for (int iFace = 0; iFace < mesh->getNumFaces(); iFace++)
//				{
//					for (int iVertex = 0; iVertex < 3; iVertex++)
//					{
//						P3NC_Vertex vVertex;
//						Point3 v;
//						if (iVertex == 0)		v = tm * mesh->verts[mesh->faces[iFace].v[0]];
//						else if (iVertex == 1)	v = tm * mesh->verts[mesh->faces[iFace].v[2]];
//						else					v = tm * mesh->verts[mesh->faces[iFace].v[1]];
//						vVertex.posX = v.x;
//						vVertex.posY = v.z;
//						vVertex.posZ = v.y;
//						pPoint->m_VertexList[iFace * 3 + iVertex] = vVertex;
//					}
//				}
//			}
//		}
//		else if (pPoint->m_ClassType == CLASS_BONE || pPoint->m_ClassType == CLASS_DUMMY)
//		{
//			Object* helperObj = pPoint->m_pObject;
//			Matrix3 oTM = pPoint->m_pINode->GetObjectTM(t);
//			Box3 bbox;
//			helperObj->GetDeformBBox(t, bbox, &oTM);
//
//			P3NC_Vertex vVertex[2];
//			memset(vVertex, 0, sizeof(P3NC_Vertex) * 2);
//			vVertex[0].posX = bbox.pmax.x;
//			vVertex[0].posY = bbox.pmax.z;
//			vVertex[0].posZ = bbox.pmax.y;
//			vVertex[1].posX = bbox.pmin.x;
//			vVertex[1].posY = bbox.pmin.z;
//			vVertex[1].posZ = bbox.pmin.y;
//			vVertex[0].col = Point4(1.0f, 1.0f, 0.0f, 0.0f);
//			vVertex[1].col = Point4(1.0f, 1.0f, 0.0f, 0.0f);
//			pPoint->m_VertexList.push_back(vVertex[0]);
//			pPoint->m_VertexList.push_back(vVertex[1]);
//		}
//
//		int iLen = pPoint->m_szName.size();
//		fwrite(&pPoint->m_ClassType, sizeof(int), 1, m_pStream);
//		fwrite(&iLen, sizeof(int), 1, m_pStream);
//		fwrite(pPoint->m_szName.c_str(), sizeof(TCHAR) * iLen, 1, m_pStream);
//
//		D3D_MATRIX matWorld;
//		DumpMatrix3(&pPoint->m_pINode->GetNodeTM(t), matWorld);
//		fwrite(&matWorld, sizeof(D3D_MATRIX), 1, m_pStream);
//
//		int iCount = pPoint->m_VertexList.size();
//		fwrite(&iCount, sizeof(int), 1, m_pStream);
//
//		for (int iVertex = 0; iVertex < pPoint->m_VertexList.size(); iVertex++)
//		{
//			fwrite(&pPoint->m_VertexList[iVertex], sizeof(P3NC_Vertex), 1, m_pStream);
//		}
//	}
//
//	fclose(m_pStream);
//	MessageBox(GetActiveWindow(), m_strFileName, _T("Succeed!!"), MB_OK);
//
//	return true;
//}
