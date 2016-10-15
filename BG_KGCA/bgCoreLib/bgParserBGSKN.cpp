#include "bgParserBGSKN.h"

bgParserBGSKN::bgParserBGSKN()
{
}

bgParserBGSKN::~bgParserBGSKN()
{
}

bool bgParserBGSKN::Read()
{
	bool hr = true;

	IF_FALSE_RETURN(ReadScene());
	IF_FALSE_RETURN(ReadObject());

	IF_FALSE_RETURN(PostRead());

	return hr;
}

bool bgParserBGSKN::ReadScene()
{
	bool hr = true;

	int iData;

	IF_FALSE_RETURN(FindWord(_T("#SCENE_INFO")));
	_stscanf(m_szLine, _T("%s %d%d%d%d%d%d%d%d"),
		m_szWord,
		&m_pObject->m_Scene.iVersion,		// 버전
		&m_pObject->m_Scene.iFirstFrame,	// 시작 프레임
		&m_pObject->m_Scene.iLastFrame,		// 마지막 프레임
		&m_pObject->m_Scene.iFrameSpeed,	// 프레임 스피드
		&m_pObject->m_Scene.iTickPerFrame,	// 프레임당 틱
		&m_pObject->m_Scene.iNumMesh,		// 메시 갯수
		&m_pObject->m_Scene.iMaxWeight,		// 가중치
		&m_pObject->m_Scene.iBindPose);		// 바인드포즈

	return hr;
}

bool bgParserBGSKN::ReadObject()
{
	bool hr = true;

	int iData, iData1, iData2;

	IF_FALSE_RETURN(FindWord(_T("#OBJECT_INFO")));
	_stscanf(m_szLine, _T("%s %d"),
		m_szWord,
		&iData);		// 오브젝트 갯수
	m_pObject->m_MeshList.resize(iData);

	for (int iMesh = 0; iMesh < m_pObject->m_MeshList.size(); iMesh++)
	{
		bgMesh* pMesh = &m_pObject->m_MeshList[iMesh];

		IF_FALSE_RETURN(FindWord(_T("#WORLD_MATRIX")));
		_stscanf(m_szLine, _T("%s %f%f%f%f %f%f%f%f %f%f%f%f %f%f%f%f"),
			m_szWord,
			&pMesh->matWorld._11, &pMesh->matWorld._12, &pMesh->matWorld._13, &pMesh->matWorld._14,		// 월드 행렬
			&pMesh->matWorld._21, &pMesh->matWorld._22, &pMesh->matWorld._23, &pMesh->matWorld._24,
			&pMesh->matWorld._31, &pMesh->matWorld._32, &pMesh->matWorld._33, &pMesh->matWorld._34,
			&pMesh->matWorld._41, &pMesh->matWorld._42, &pMesh->matWorld._43, &pMesh->matWorld._44);

		IF_FALSE_RETURN(FindWord(_T("#TRIANGLE_LIST")));
		_stscanf(m_szLine, _T("%s %d%d%d"),
			m_szWord,
			&iData,
			&iData1,		// 버텍스 갯수
			&iData2);		// 인덱스 갯수
		pMesh->VertexList.resize(iData1);
		pMesh->IndexList.resize(iData2 * 3);

		ReadVertex(pMesh);
		ReadIndex(pMesh);
		ReadTexture(pMesh);
	}

	ReadMatrix();

	return hr;
}

bool bgParserBGSKN::ReadVertex(bgMesh* pMesh)
{
	bool hr = true;

	int iData;

	for (int iVertex = 0; iVertex < pMesh->VertexList.size(); iVertex++)
	{
		PNCT_VERTEX* pVertex = &pMesh->VertexList[iVertex];
		_fgetts(m_szLine, MAX_PATH * 4, m_pFile);
		_stscanf(m_szLine, _T("%f%f%f %f%f%f %f%f%f%f %f%f"),
			&pVertex->p.x, &pVertex->p.y, &pVertex->p.z,					// 포지션
			&pVertex->n.x, &pVertex->n.y, &pVertex->n.z,					// 노멀
			&pVertex->c.x, &pVertex->c.y, &pVertex->c.z, &pVertex->c.w,		// 컬러
			&pVertex->t.x, &pVertex->t.y									// 텍스쳐
			);	// 정점 가중치 등 추가예정...
	}

	return hr;
}

bool bgParserBGSKN::ReadIndex(bgMesh* pMesh)
{
	bool hr = true;

	for (int iIndex = 0; iIndex < pMesh->IndexList.size(); iIndex += 3)
	{
		_fgetts(m_szLine, MAX_PATH * 4, m_pFile);
		_stscanf(m_szLine, _T("%d%d%d"),
			&pMesh->IndexList[iIndex + 0],		// 페이스 하나에 해당하는 인덱스(3개)
			&pMesh->IndexList[iIndex + 1],
			&pMesh->IndexList[iIndex + 2]);
	}

	return hr;
}

bool bgParserBGSKN::ReadTexture(bgMesh* pMesh)
{
	bool hr = true;

	int iData;

	IF_FALSE_RETURN(FindWord(_T("#TEXTURE_INFO")));
	_stscanf(m_szLine, _T("%s %d %s"),
		m_szWord,
		&pMesh->Texture.iType,			// 텍스쳐 타입
		pMesh->Texture.szBitmap);		// 텍스쳐 이름

	return hr;
}

bool bgParserBGSKN::ReadMatrix()
{
	bool hr = true;

	int iData;

	IF_FALSE_RETURN(FindWord(_T("#NODE_INFO")));
	_stscanf(m_szLine, _T("%s %d"),
		m_szWord,
		&iData);		// 노드 갯수
	m_pObject->m_matNodeList.resize(iData);

	for (int iNode = 0; iNode < m_pObject->m_matNodeList.size(); iNode++)
	{
		D3DXMATRIX* pNodeMatrix = &m_pObject->m_matNodeList[iNode];

		IF_FALSE_RETURN(FindWord(_T("#MATRIX_LIST")));
		_stscanf(m_szLine, _T("%s %d %f%f%f%f %f%f%f%f %f%f%f%f %f%f%f%f"),
			m_szWord,
			&iData,																			// 노드 번호
			&pNodeMatrix->_11, &pNodeMatrix->_12, &pNodeMatrix->_13, &pNodeMatrix->_14,		// 노드단위 행렬
			&pNodeMatrix->_21, &pNodeMatrix->_22, &pNodeMatrix->_23, &pNodeMatrix->_24,
			&pNodeMatrix->_31, &pNodeMatrix->_32, &pNodeMatrix->_33, &pNodeMatrix->_34,
			&pNodeMatrix->_41, &pNodeMatrix->_42, &pNodeMatrix->_43, &pNodeMatrix->_44);
	}

	return hr;
}

bool bgParserBGSKN::PostRead()
{
	bool hr = true;
	return hr;
}
