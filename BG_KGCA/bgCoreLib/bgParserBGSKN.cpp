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
		&m_pObject->m_Scene.iVersion,		// ����
		&m_pObject->m_Scene.iFirstFrame,	// ���� ������
		&m_pObject->m_Scene.iLastFrame,		// ������ ������
		&m_pObject->m_Scene.iFrameSpeed,	// ������ ���ǵ�
		&m_pObject->m_Scene.iTickPerFrame,	// �����Ӵ� ƽ
		&m_pObject->m_Scene.iNumMesh,		// �޽� ����
		&m_pObject->m_Scene.iMaxWeight,		// ����ġ
		&m_pObject->m_Scene.iBindPose);		// ���ε�����

	return hr;
}

bool bgParserBGSKN::ReadObject()
{
	bool hr = true;

	int iData, iData1, iData2;

	IF_FALSE_RETURN(FindWord(_T("#OBJECT_INFO")));
	_stscanf(m_szLine, _T("%s %d"),
		m_szWord,
		&iData);		// ������Ʈ ����
	m_pObject->m_MeshList.resize(iData);

	for (int iMesh = 0; iMesh < m_pObject->m_MeshList.size(); iMesh++)
	{
		bgMesh* pMesh = &m_pObject->m_MeshList[iMesh];

		IF_FALSE_RETURN(FindWord(_T("#WORLD_MATRIX")));
		_stscanf(m_szLine, _T("%s %f%f%f%f %f%f%f%f %f%f%f%f %f%f%f%f"),
			m_szWord,
			&pMesh->matWorld._11, &pMesh->matWorld._12, &pMesh->matWorld._13, &pMesh->matWorld._14,		// ���� ���
			&pMesh->matWorld._21, &pMesh->matWorld._22, &pMesh->matWorld._23, &pMesh->matWorld._24,
			&pMesh->matWorld._31, &pMesh->matWorld._32, &pMesh->matWorld._33, &pMesh->matWorld._34,
			&pMesh->matWorld._41, &pMesh->matWorld._42, &pMesh->matWorld._43, &pMesh->matWorld._44);

		IF_FALSE_RETURN(FindWord(_T("#TRIANGLE_LIST")));
		_stscanf(m_szLine, _T("%s %d%d%d"),
			m_szWord,
			&iData,
			&iData1,		// ���ؽ� ����
			&iData2);		// �ε��� ����
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
			&pVertex->p.x, &pVertex->p.y, &pVertex->p.z,					// ������
			&pVertex->n.x, &pVertex->n.y, &pVertex->n.z,					// ���
			&pVertex->c.x, &pVertex->c.y, &pVertex->c.z, &pVertex->c.w,		// �÷�
			&pVertex->t.x, &pVertex->t.y									// �ؽ���
			);	// ���� ����ġ �� �߰�����...
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
			&pMesh->IndexList[iIndex + 0],		// ���̽� �ϳ��� �ش��ϴ� �ε���(3��)
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
		&pMesh->Texture.iType,			// �ؽ��� Ÿ��
		pMesh->Texture.szBitmap);		// �ؽ��� �̸�

	return hr;
}

bool bgParserBGSKN::ReadMatrix()
{
	bool hr = true;

	int iData;

	IF_FALSE_RETURN(FindWord(_T("#NODE_INFO")));
	_stscanf(m_szLine, _T("%s %d"),
		m_szWord,
		&iData);		// ��� ����
	m_pObject->m_matNodeList.resize(iData);

	for (int iNode = 0; iNode < m_pObject->m_matNodeList.size(); iNode++)
	{
		D3DXMATRIX* pNodeMatrix = &m_pObject->m_matNodeList[iNode];

		IF_FALSE_RETURN(FindWord(_T("#MATRIX_LIST")));
		_stscanf(m_szLine, _T("%s %d %f%f%f%f %f%f%f%f %f%f%f%f %f%f%f%f"),
			m_szWord,
			&iData,																			// ��� ��ȣ
			&pNodeMatrix->_11, &pNodeMatrix->_12, &pNodeMatrix->_13, &pNodeMatrix->_14,		// ������ ���
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
