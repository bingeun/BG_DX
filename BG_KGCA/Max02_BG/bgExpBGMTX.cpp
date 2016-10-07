#include "bgExporterMax.h"

bool bgExporterMax::ExportBGMTX()
{
	m_strFileName = I_Exporter.SaveFileDialog(_T("BGMTX"), _T("BG Matrix Export"));

	m_pStream = _tfopen(m_strFileName, _T("wt"));
	if (!m_pStream)
		return 0;

	{
		ExpSceneMTX();
		ExpObjectMTX();
	}

	fclose(m_pStream);
	MessageBox(GetActiveWindow(), m_strFileName, _T("Succeed!!"), MB_OK);

	return true;
}

void bgExporterMax::ExpSceneMTX()
{
	struct tm *newtime;
	time_t aclock;

	time(&aclock);
	newtime = localtime(&aclock);

	TSTR today = _tasctime(newtime);
	today.remove(today.length() - 1);

	_ftprintf(m_pStream, _T("%s\t%s\n"),
		_T("#BG3D_MATRIX"),
		FixupName(today));		// 날짜

	_ftprintf(m_pStream, _T("%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n"),
		_T("#SCENE_INFO"),
		m_Scene.iVersion,		// 버전
		m_Scene.iFirstFrame,	// 시작 프레임
		m_Scene.iLastFrame,		// 마지막 프레임
		m_Scene.iFrameSpeed,	// 프레임 스피드
		m_Scene.iTickPerFrame,	// 프레임당 틱
		m_Scene.iNumMesh,		// 메시 갯수
		m_Scene.iMaxWeight,		// 가중치
		m_Scene.iBindPose);		// 바인드포즈
}

void bgExporterMax::ExpObjectMTX()
{
	_ftprintf(m_pStream, _T("%s\t%d\n"),
		_T("#OBJECT_INFO"),
		m_MatrixMap.Count());	// 갯수

	for (DWORD iBiped = 0; iBiped < m_MatrixMap.Count(); iBiped++)
	{
		bgMatrixIndex* pPoint = (bgMatrixIndex*)m_MatrixMap.m_TMap[iBiped];
		INode* pNode = pPoint->m_pINode;

		_ftprintf(m_pStream, _T("%s\t%d\t%d\t%s\t%s\n"),
			_T("#OBJECT_LIST"),
			iBiped,				// 번호
			-1,					// 부모번호
			pPoint->m_szName,	// 이름
			_T("(NULL)"));		// 부모이름

		TimeValue t = this->m_Interval.Start();
		D3D_MATRIX matWorld;
		DumpMatrix3(&pPoint->m_pINode->GetNodeTM(t), matWorld);

		_ftprintf(m_pStream, _T("\t%s\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			_T("#WORLD_MATRIX"),
			matWorld._11, matWorld._12, matWorld._13, matWorld._14,	// 월드 행렬
			matWorld._21, matWorld._22, matWorld._23, matWorld._24,
			matWorld._31, matWorld._32, matWorld._33, matWorld._34,
			matWorld._41, matWorld._42, matWorld._43, matWorld._44);

		if (pPoint->m_ClassType == CLASS_BIPED || pPoint->m_ClassType == CLASS_GEOM)
		{
			BOOL needDel;
			TriObject *tri = GetTriObject(pPoint->m_pINode, t, needDel);
			if (tri != NULL)
			{
				Matrix3 tm = pPoint->m_pINode->GetObjTMAfterWSM(t);
				Mesh* mesh = &tri->GetMesh();
				mesh->buildNormals();
				pPoint->m_VertexList.resize(mesh->getNumFaces() * 3);
				for (int iFace = 0; iFace < mesh->getNumFaces(); iFace++)
				{
					for (int iVertex = 0; iVertex < 3; iVertex++)
					{
						P3NC_Vertex vVertex;
						Point3 v;
						if (iVertex == 0)
							v = tm * mesh->verts[mesh->faces[iFace].v[0]];
						else if (iVertex == 1)
							v = tm * mesh->verts[mesh->faces[iFace].v[2]];
						else
							v = tm * mesh->verts[mesh->faces[iFace].v[1]];
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

		_ftprintf(m_pStream, _T("\t%s\t%d\n"),
			_T("#VERTEX_LIST"),
			pPoint->m_VertexList.size());		// 바운드박스 정점 갯수

		for (int iVertex = 0; iVertex < pPoint->m_VertexList.size(); iVertex++)
		{
			P3NC_Vertex* pVertex = &pPoint->m_VertexList[iVertex];
			_ftprintf(m_pStream, _T("\t\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
				pVertex->posX, pVertex->posY, pVertex->posZ,						// 포지션
				pVertex->norX, pVertex->norY, pVertex->norZ,						// 노멀
				pVertex->col.x, pVertex->col.y, pVertex->col.z, pVertex->col.w);	// 컬러
		}

		_ftprintf(m_pStream, _T("\t%s\t%d\n"),
			_T("#MATRIX_LIST"),
			m_Scene.iLastFrame - m_Scene.iFirstFrame);		// 프레임 갯수

		for (int iFrame = m_Scene.iFirstFrame; iFrame < m_Scene.iLastFrame; iFrame++)
		{
			D3D_MATRIX pAnimation;
			TimeValue iTime = iFrame * m_Scene.iTickPerFrame;
			Matrix3 m_pAnimTm = pNode->GetNodeTM(iTime);
			DumpMatrix3(&m_pAnimTm, pAnimation);
			
			_ftprintf(m_pStream, _T("\t\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
				pAnimation._11, pAnimation._12, pAnimation._13, pAnimation._14,	// 프레임단위 애니메이션 행렬
				pAnimation._21, pAnimation._22, pAnimation._23, pAnimation._24,
				pAnimation._31, pAnimation._32, pAnimation._33, pAnimation._34,
				pAnimation._41, pAnimation._42, pAnimation._43, pAnimation._44);
		}
	}
}
