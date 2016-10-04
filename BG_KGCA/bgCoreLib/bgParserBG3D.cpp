#include "bgParserBG3D.h"

bgParserBG3D::bgParserBG3D()
{
}

bgParserBG3D::~bgParserBG3D()
{
}

bool bgParserBG3D::Init(bgModelBG3D* pModel)
{
	m_pModel = pModel;
	return true;
}

bool bgParserBG3D::Read()
{
	bool hr = true;

	TCHAR szWordArray[2][MAX_PATH];

	// 데이터 초기화
	ZeroMemory(&m_pModel->m_Scene, sizeof(SceneInfo));
	m_pModel->m_MaterialList.clear();
	m_pModel->m_ObjectList.clear();

	// 섹션 읽기
	IF_FALSE_RETURN(FindWord(_T("#BG3D_MODEL")));
	IF_FALSE_RETURN(ReadScene());
	IF_FALSE_RETURN(ReadMaterial());
	IF_FALSE_RETURN(ReadObject());

	ConvertToModel();	// 읽은 데이터를 모델용 데이터로 컨버팅
	LinkNode();			// 노드 관계 연결
	OperationTM();		// 노드 관계에 따른 행렬 연산

	return hr;
}

bool bgParserBG3D::ReadScene()
{
	bool hr = true;

	int iData;

	IF_FALSE_RETURN(FindWord(_T("#SCENE_INFO")));
	_stscanf(m_szLine, _T("%s%d%d%d%d%d%d%d%d"),
		m_szWord,
		&iData, //&m_pModel->m_Scene.iVersion,		// 버전
		&m_pModel->m_Scene.iFirstFrame,		// 시작 프레임
		&m_pModel->m_Scene.iLastFrame,		// 마지막 프레임
		&m_pModel->m_Scene.iFrameSpeed,		// 프레임 스피드
		&m_pModel->m_Scene.iTicksPerFrame,	// 프레임당 틱
		&iData, //&m_pModel->m_Scene.iNumMesh,		// 메시 갯수
		&iData, //&m_pModel->m_Scene.iMaxWeight,		// 가중치
		&iData); //&m_pModel->m_Scene.iBindPose);		// 바인드포즈

	return hr;
}

bool bgParserBG3D::ReadMaterial()
{
	bool hr = true;

	TCHAR szData[MAX_PATH] = { 0, };
	int iData, iData1, iData2, iData3, iData4;
	int iLoop, iLoopMax;
	int iLoopSub, iLoopSubMax;

	// 최상위 메터리얼 갯수
	IF_FALSE_RETURN(FindWord(_T("#MATERIAL_INFO")));
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_pModel->m_MaterialList.resize(iData);

	// 최상위 메터리얼 갯수만큼 반복하며 읽기
	for (int iRootMtl = 0; iRootMtl < m_pModel->m_MaterialList.size(); iRootMtl++)
	{
		IF_FALSE_RETURN(FindWord(_T("#MATERIAL_LIST")));
		_stscanf(m_szLine, _T("%s%d%d%d%s%s"),
			m_szWord,
			&iData,		// 번호
			&iData1,	// 텍스쳐 갯수
			&iData2,	// 서브메터리얼 갯수
			m_pModel->m_MaterialList[iRootMtl].szMaterialName,	// 이름
			szData);	// 종류

		// 서브메터리얼이 있으면
		if (iData2 > 0)
		{
			m_pModel->m_MaterialList[iRootMtl].SubMaterialList.resize(iData2);
			for (int iSubMtl = 0; iSubMtl < m_pModel->m_MaterialList[iRootMtl].SubMaterialList.size(); iSubMtl++)
			{
				IF_FALSE_RETURN(FindWord(_T("#SUBMATERIAL")));
				_stscanf(m_szLine, _T("%s%d%d%d%s%s"),
					m_szWord,
					&iData,		// 서브 번호
					&iData1,	// 텍스쳐 갯수
					&iData2,	// 서브메터리얼 갯수
					m_pModel->m_MaterialList[iRootMtl].SubMaterialList[iSubMtl].szMaterialName,	// 이름
					szData);	// 종류

				for (int iSubTex = 0; iSubTex < iData1; iSubTex++)
				{
					ReadTexture(iSubTex, &m_pModel->m_MaterialList[iRootMtl], &m_pModel->m_MaterialList[iRootMtl].SubMaterialList[iSubMtl]);
				}
			}
		}
		// 서브메터리얼이 없으면
		else
		{
			for (int iTex = 0; iTex < iData1; iTex++)
			{
				ReadTexture(iTex, &m_pModel->m_MaterialList[iRootMtl]);
			}
		}
	}

	return hr;
}

bool bgParserBG3D::ReadTexture(int iIndex, MaterialInfo* pMat, SubMaterialInfo* pSubMat)
{
	bool hr = true;

	int iData, iDataType;

	IF_FALSE_RETURN(FindWord(_T("#TEXTUREMAP")));
	_stscanf(m_szLine, _T("%s%d%d"),
		m_szWord,
		&iData,				// 번호
		&iDataType);		// 종류

	if (iDataType == 1)
	{
		if (pSubMat == NULL)
		{
			_stscanf(m_szLine, _T("%s%d%d%s"),
				m_szWord,
				&iData,				// 번호
				&iDataType,			// 종류
				pMat->szBitmap);	// 이름
		}
		else
		{
			_stscanf(m_szLine, _T("%s%d%d%s"),
				m_szWord,
				&iData,				// 번호
				&iDataType,			// 종류
				pSubMat->szBitmap);	// 이름
		}
	}

	return hr;
}

bool bgParserBG3D::ReadObject()
{
	bool hr = true;

	int iData, iData1, iData2, iData3, iData4;
	D3DXVECTOR3 vData1, vData2;

	IF_FALSE_RETURN(FindWord(_T("#OBJECT_INFO")));
	_stscanf(m_szLine, _T("%s%d"),
		m_szWord,
		&iData1);	// 갯수
	m_pModel->m_ObjectList.resize(iData1);

	for (int iMesh = 0; iMesh < m_pModel->m_ObjectList.size(); iMesh++)
	{
		ObjectNode* pPoint = (ObjectNode*)&m_pModel->m_ObjectList[iMesh];
		m_pModel->m_ObjectList[iMesh].vpObj = new GeomObject;
		m_pModel->m_ObjectList[iMesh].eNodeType = OBJECT_NODE_TYPE_GEOMOBJECT;
		GeomObject* pMesh = static_cast<GeomObject*>(m_pModel->m_ObjectList[iMesh].vpObj);

		IF_FALSE_RETURN(FindWord(_T("#OBJECT_LIST")));
		_stscanf(m_szLine, _T("%s%d%d%s%s"),
			m_szWord,
			&iData,					// 번호
			&iData1,				// 부모번호
			pPoint->szNodeName,		// 이름
			pPoint->szNodeParent);	// 부모이름

		IF_FALSE_RETURN(FindWord(_T("#OBJECT_DETAIL")));
		_stscanf(m_szLine, _T("%s %d%d%d%d %d%d%d%d"),
			m_szWord,
			&iData,					// 종류
			&pMesh->iMaterialRef,	// 루트ID
			&iData1,				// 이동트랙 갯수
			&iData2,				// 회전트랙 갯수
			&iData3,				// 신축트랙 갯수
			&iData4);				// 알파트랙 갯수
		pPoint->Anim.PosTrack.resize(iData1);
		pPoint->Anim.RotTrack.resize(iData2);
		pPoint->Anim.SclTrack.resize(iData3);
		pPoint->Anim.AlpTrack.resize(iData4);
		if(iData1 || iData2 || iData3 || iData4)
			pPoint->bAnim = true;
		else
			pPoint->bAnim = false;

		IF_FALSE_RETURN(FindWord(_T("#BOUNDING_BOX")));
		_stscanf(m_szLine, _T("%s %f%f%f %f%f%f"),
			m_szWord,
			&vData1.x, &vData1.y, &vData1.z,	// 바운딩박스 최대
			&vData2.x, &vData2.y, &vData2.z);	// 바운딩박스 최소

		IF_FALSE_RETURN(FindWord(_T("#WORLD_MATRIX")));
		_stscanf(m_szLine, _T("%s %f%f%f%f %f%f%f%f %f%f%f%f %f%f%f%f"),
			m_szWord,
			&pPoint->nodeTM.matWorld._11, &pPoint->nodeTM.matWorld._12, &pPoint->nodeTM.matWorld._13, &pPoint->nodeTM.matWorld._14,	// 월드 행렬
			&pPoint->nodeTM.matWorld._21, &pPoint->nodeTM.matWorld._22, &pPoint->nodeTM.matWorld._23, &pPoint->nodeTM.matWorld._24,
			&pPoint->nodeTM.matWorld._31, &pPoint->nodeTM.matWorld._32, &pPoint->nodeTM.matWorld._33, &pPoint->nodeTM.matWorld._34,
			&pPoint->nodeTM.matWorld._41, &pPoint->nodeTM.matWorld._42, &pPoint->nodeTM.matWorld._43, &pPoint->nodeTM.matWorld._44);

		IF_FALSE_RETURN(FindWord(_T("#TRIANGLE_INFO")));
		_stscanf(m_szLine, _T("%s%d"),
			m_szWord,
			&iData1);		// 서브ID 갯수

		if (pMesh->iMaterialRef >= 0 && m_pModel->m_MaterialList[pMesh->iMaterialRef].SubMaterialList.size() > 0)
		{
			for (int iMtrl = 0; iMtrl < m_pModel->m_MaterialList[pMesh->iMaterialRef].SubMaterialList.size(); iMtrl++)
			{
				IF_FALSE_RETURN(FindWord(_T("#TRIANGLE_LIST")));
				_stscanf(m_szLine, _T("%s%d%d%d"),
					m_szWord,
					&iData,			// 번호
					&iData1,		// 정점 갯수
					&iData2);		// 페이스 갯수

				if (iData1 > 0)
				{
					pMesh->VertexList.resize(iData1);
					pMesh->NorVertexList.resize(iData1);
					pMesh->ColVertexList.resize(iData1);
					pMesh->TexVertexList.resize(iData1);

					ReadVertex(pMesh, iMtrl, pMesh->VertexList.size());
				}

				if (iData2 > 0)
				{
					pMesh->FaceList.resize(iData2);
					pMesh->NorFaceList.resize(iData2);
					pMesh->ColFaceList.resize(iData2);
					pMesh->TexFaceList.resize(iData2);

					ReadIndex(pMesh, iMtrl, pMesh->FaceList.size());
				}
			}
		}
		else
		{
			IF_FALSE_RETURN(FindWord(_T("#TRIANGLE_LIST")));
			_stscanf(m_szLine, _T("%s%d%d%d"),
				m_szWord,
				&iData,			// 번호
				&iData1,		// 정점 갯수
				&iData2);		// 페이스 갯수

			if (iData1 > 0)
			{
				pMesh->VertexList.resize(iData1);
				pMesh->NorVertexList.resize(iData1);
				pMesh->ColVertexList.resize(iData1);
				pMesh->TexVertexList.resize(iData1);

				ReadVertex(pMesh, 0, pMesh->VertexList.size());
			}

			if (iData2 > 0)
			{
				pMesh->FaceList.resize(iData2);
				pMesh->NorFaceList.resize(iData2);
				pMesh->ColFaceList.resize(iData2);
				pMesh->TexFaceList.resize(iData2);

				ReadIndex(pMesh, 0, pMesh->FaceList.size());
			}
		}

		ReadAnimation(&pPoint->Anim);
	}

	return hr;
}

bool bgParserBG3D::ReadVertex(GeomObject* pMesh, int iMtrl, int iNumVertex)
{
	bool hr = true;

	float fData;

	for (int iCnt = 0; iCnt < iNumVertex; iCnt++)
	{
		_fgetts(m_szLine, MAX_PATH, m_pFile);
		_stscanf(m_szLine, _T("%f%f%f %f%f%f %f%f%f%f %f%f"),
			&pMesh->VertexList[iCnt].x, &pMesh->VertexList[iCnt].y, &pMesh->VertexList[iCnt].z,						// 포지션
			&pMesh->NorVertexList[iCnt].x, &pMesh->NorVertexList[iCnt].y, &pMesh->NorVertexList[iCnt].z,			// 노멀
			&pMesh->ColVertexList[iCnt].x, &pMesh->ColVertexList[iCnt].y, &pMesh->ColVertexList[iCnt].z, &fData,	// 컬러
			&pMesh->TexVertexList[iCnt].x, &pMesh->TexVertexList[iCnt].y);											// 텍스쳐
	}

	return hr;
}

bool bgParserBG3D::ReadIndex(GeomObject* pMesh, int iMtrl, int iNumIndex)
{
	bool hr = true;

	for (int iCnt = 0; iCnt < iNumIndex; iCnt++)
	{
		_fgetts(m_szLine, MAX_PATH, m_pFile);
		_stscanf(m_szLine, _T("%d%d%d"),
			&pMesh->FaceList[iCnt].iA, &pMesh->FaceList[iCnt].iB, &pMesh->FaceList[iCnt].iC);

		pMesh->FaceList[iCnt].iID = iMtrl;

		pMesh->TexFaceList[iCnt] = pMesh->ColFaceList[iCnt] = pMesh->FaceList[iCnt];
		pMesh->NorFaceList[iCnt] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	}

	return hr;
}

bool bgParserBG3D::ReadAnimation(TMAnimation* pMesh)
{
	bool hr = true;

	TCHAR szData[MAX_PATH] = { 0, };
	int iData;
	
	IF_FALSE_RETURN(FindWord(_T("#POS_TRACK_LIST")));
	_stscanf(m_szLine, _T("%s%d"),
		szData,
		&iData);	// 위치 트랙 길이

	for (DWORD dwPosTrack = 0; dwPosTrack < pMesh->PosTrack.size(); dwPosTrack++)
	{
		_fgetts(m_szLine, MAX_PATH, m_pFile);
		_stscanf(m_szLine, _T("%d %f%f%f"),
			&pMesh->PosTrack[dwPosTrack].iTick,			// 틱
			&pMesh->PosTrack[dwPosTrack].vVector.x,		// 포지션
			&pMesh->PosTrack[dwPosTrack].vVector.y,
			&pMesh->PosTrack[dwPosTrack].vVector.z);
	}

	IF_FALSE_RETURN(FindWord(_T("#ROT_TRACK_LIST")));
	_stscanf(m_szLine, _T("%s%d"),
		szData,
		&iData);	// 회전 트랙 길이

	for (DWORD dwRotTrack = 0; dwRotTrack < pMesh->RotTrack.size(); dwRotTrack++)
	{
		_fgetts(m_szLine, MAX_PATH, m_pFile);
		_stscanf(m_szLine, _T("%d %f%f%f %f%f%f%f"),
			&pMesh->RotTrack[dwRotTrack].iTick,			// 틱
			&pMesh->RotTrack[dwRotTrack].vVector.x,		// 벡터
			&pMesh->RotTrack[dwRotTrack].vVector.y,
			&pMesh->RotTrack[dwRotTrack].vVector.z,
			&pMesh->RotTrack[dwRotTrack].qRotate.x,		// 사원수
			&pMesh->RotTrack[dwRotTrack].qRotate.y,
			&pMesh->RotTrack[dwRotTrack].qRotate.z,
			&pMesh->RotTrack[dwRotTrack].qRotate.w);
	}

	IF_FALSE_RETURN(FindWord(_T("#SCL_TRACK_LIST")));
	_stscanf(m_szLine, _T("%s%d"),
		szData,
		&iData);	// 신축 트랙 길이

	for (DWORD dwSclTrack = 0; dwSclTrack < pMesh->SclTrack.size(); dwSclTrack++)
	{
		_fgetts(m_szLine, MAX_PATH, m_pFile);
		_stscanf(m_szLine, _T("%d %f%f%f %f%f%f%f"),
			&pMesh->SclTrack[dwSclTrack].iTick,			// 틱
			&pMesh->SclTrack[dwSclTrack].vVector.x,		// 벡터
			&pMesh->SclTrack[dwSclTrack].vVector.y,
			&pMesh->SclTrack[dwSclTrack].vVector.z,
			&pMesh->SclTrack[dwSclTrack].qRotate.x,		// 사원수
			&pMesh->SclTrack[dwSclTrack].qRotate.y,
			&pMesh->SclTrack[dwSclTrack].qRotate.z,
			&pMesh->SclTrack[dwSclTrack].qRotate.w);
	}

	IF_FALSE_RETURN(FindWord(_T("#ALP_TRACK_LIST")));
	_stscanf(m_szLine, _T("%s%d"),
		szData,
		&iData);	// 위치 트랙 길이

	for (DWORD dwAlpTrack = 0; dwAlpTrack < pMesh->AlpTrack.size(); dwAlpTrack++)
	{
		_fgetts(m_szLine, MAX_PATH, m_pFile);
		_stscanf(m_szLine, _T("%d %f"),
			&pMesh->AlpTrack[dwAlpTrack].iTick,			// 틱
			&pMesh->AlpTrack[dwAlpTrack].vVector.x);	// 알파
	}

	return hr;
}

// 파일에서 읽은 데이터를 모델(출력용) 데이터로 변환
void bgParserBG3D::ConvertToModel()
{
	TCHAR szFileName[MAX_PATH] = { 0, };
	int iLoop, iLoopSub;
	int iNumMaterial;
	int iNumSubMaterial;

	// 메터리얼 텍스쳐 로드
	m_pModel->m_TexIDList.resize(m_pModel->m_MaterialList.size());
	iNumMaterial = 0;
	for (iLoop = 0; iLoop < m_pModel->m_MaterialList.size(); iLoop++)
	{
		// 서브 메터리얼이 있으면
		if (m_pModel->m_MaterialList[iLoop].SubMaterialList.size() > 0)
		{
			iNumSubMaterial = m_pModel->m_MaterialList[iLoop].SubMaterialList.size();
			m_pModel->m_TexIDList[iLoop].SubIDList.resize(iNumSubMaterial);
			for (iLoopSub = 0; iLoopSub < iNumSubMaterial; iLoopSub++)
			{
				iNumMaterial++;
				_stprintf(szFileName, _T("%s%s"), _T("../../data/model/"), m_pModel->m_MaterialList[iLoop].SubMaterialList[iLoopSub].szBitmap);

				m_pModel->m_TexIDList[iLoop].SubIDList[iLoopSub].iID = I_TextureMgr.Add(szFileName);
			}
		}
		// 서브 메터리얼이 없으면
		else
		{
			iNumMaterial++;
			_stscanf(szFileName, _T("%s%s\0"), _T("../../data/model/"), m_pModel->m_MaterialList[iLoop].szBitmap);
			m_pModel->m_TexIDList[iLoop].SubIDList[iLoopSub].iID = 
			m_pModel->m_TexIDList[iLoop].iID = I_TextureMgr.Add(szFileName);
		}
	}
	// 종류별로 분류하여 정점배열에 데이터 추가
	// 텍스쳐 종류별(iLoop => MTLID)로 해당종류의 갯수(iLoopSub)만큼 반복하며 인덱스번호 지정
}

void bgParserBG3D::LinkNode()
{
	int iNode, iFindParent;

	// 전체 노드의 관계 연결
	for (iNode = 0; iNode < m_pModel->m_ObjectList.size(); iNode++)
	{
		// 부모노드의 이름이 있을때만 탐색
		if (_tcsicmp(m_pModel->m_ObjectList[iNode].szNodeParent, _T("(NULL)")))
		{
			for (iFindParent = 0; iFindParent < m_pModel->m_ObjectList.size(); iFindParent++)
			{
				// 부모노드를 찾으면 부모노드와 자식노드 서로간의 포인터 연결
				if (!_tcsicmp(m_pModel->m_ObjectList[iNode].szNodeParent, m_pModel->m_ObjectList[iFindParent].szNodeName))
				{
					m_pModel->m_ObjectList[iNode].pNodeParent = &m_pModel->m_ObjectList[iFindParent];
					m_pModel->m_ObjectList[iFindParent].pNodeChildList.push_back(&m_pModel->m_ObjectList[iNode]);
					break;
				}
			}

			// 찾는 부모노드가 없으면 (정상적인 ASE 데이터이면 항상 거짓이어야 함)
			if (iFindParent >= m_pModel->m_ObjectList.size())
			{
				m_pModel->m_ObjectList[iNode].pNodeParent = NULL;
			}
		}
	}

	// 헬퍼오브젝트이고 자식노드가 없으면 제거 (사용되지 않는 오브젝트)
	for (iNode = 0; iNode < m_pModel->m_ObjectList.size(); iNode++)
	{
		if (m_pModel->m_ObjectList[iNode].eNodeType == OBJECT_NODE_TYPE_HELPEROBJECT &&
			m_pModel->m_ObjectList[iNode].pNodeChildList.size() == 0)
		{
			// 제거 처리
		}
	}

	iNode = 0;
}

void bgParserBG3D::OperationTM()
{
	D3DXMATRIX* matParentWorld;
	D3DXMATRIX matChildWorld;
	D3DXMATRIX matInvParentWorld;
	D3DXMATRIX matPos, matRot, matScl;
	D3DXVECTOR3 vPos, vScl;
	D3DXQUATERNION qRot;
	
	for (int iNode = 0; iNode < m_pModel->m_ObjectList.size(); iNode++)
	{
		if (m_pModel->m_ObjectList[iNode].pNodeParent)
		{
			matParentWorld = &m_pModel->m_ObjectList[iNode].pNodeParent->nodeTM.matWorld;

			D3DXMatrixInverse(&matInvParentWorld, NULL, matParentWorld);
			matChildWorld = m_pModel->m_ObjectList[iNode].nodeTM.matWorld * matInvParentWorld;
			D3DXMatrixDecompose(&vScl, &qRot, &vPos, &matChildWorld);
			D3DXMatrixScaling(&m_pModel->m_ObjectList[iNode].matWorldScl, vScl.x, vScl.y, vScl.z);
			D3DXMatrixTranslation(&m_pModel->m_ObjectList[iNode].matWorldPos, vPos.x, vPos.y, vPos.z);
			D3DXMatrixRotationQuaternion(&m_pModel->m_ObjectList[iNode].matWorldRot, &qRot);
		}
	}
}
