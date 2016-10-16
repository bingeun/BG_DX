#include "bgParserBGMTX.h"

bgParserBGMTX::bgParserBGMTX()
{
}

bgParserBGMTX::~bgParserBGMTX()
{
}

bool bgParserBGMTX::Read()
{
	bool hr = true;

	IF_FALSE_RETURN(ReadScene());
	IF_FALSE_RETURN(ReadNode());

	IF_FALSE_RETURN(PostRead());

	return hr;
}

bool bgParserBGMTX::ReadScene()
{
	bool hr = true;

	int iData;

	IF_FALSE_RETURN(FindWord(_T("#SCENE_INFO")));
	_stscanf(m_szLine, _T("%s %d%d%d%d%d%d%d%d"),
		m_szWord,
		&m_pAnimation->m_Scene.iVersion,		// ����
		&m_pAnimation->m_Scene.iFirstFrame,		// ���� ������
		&m_pAnimation->m_Scene.iLastFrame,		// ������ ������
		&m_pAnimation->m_Scene.iFrameSpeed,		// ������ ���ǵ�
		&m_pAnimation->m_Scene.iTickPerFrame,	// �����Ӵ� ƽ
		&m_pAnimation->m_Scene.iNumMesh,		// �޽� ����
		&m_pAnimation->m_Scene.iMaxWeight,		// ����ġ
		&m_pAnimation->m_Scene.iBindPose);		// ���ε�����

	return hr;
}

bool bgParserBGMTX::ReadNode()
{
	bool hr = true;

	int iData;

	IF_FALSE_RETURN(FindWord(_T("#OBJECT_INFO")));
	_stscanf(m_szLine, _T("%s %d"),
		m_szWord,
		&iData);		// ��� ����
	m_pAnimation->m_NodeList.resize(iData);

	for (int iNode = 0; iNode < m_pAnimation->m_NodeList.size(); iNode++)
	{
		bgAnimNode* pNode = &m_pAnimation->m_NodeList[iNode];

		IF_FALSE_RETURN(FindWord(_T("#WORLD_MATRIX")));
		_stscanf(m_szLine, _T("%s %f%f%f%f %f%f%f%f %f%f%f%f %f%f%f%f"),
			m_szWord,
			&pNode->matWorld._11, &pNode->matWorld._12, &pNode->matWorld._13, &pNode->matWorld._14,	// ���� ���
			&pNode->matWorld._21, &pNode->matWorld._22, &pNode->matWorld._23, &pNode->matWorld._24,
			&pNode->matWorld._31, &pNode->matWorld._32, &pNode->matWorld._33, &pNode->matWorld._34,
			&pNode->matWorld._41, &pNode->matWorld._42, &pNode->matWorld._43, &pNode->matWorld._44);
		
		D3DXMatrixInverse(&pNode->matWorldInv, 0, &pNode->matWorld);

		ReadVertex(pNode);
		ReadMatrix(pNode);
	}

	return hr;
}

bool bgParserBGMTX::ReadVertex(bgAnimNode* pNode)
{
	bool hr = true;

	int iData;

	IF_FALSE_RETURN(FindWord(_T("#VERTEX_LIST")));
	_stscanf(m_szLine, _T("%s %d"),
		m_szWord,
		&iData);		// �����е�� ���� ����
	pNode->vBipedList.resize(iData);

	for (int iVertex = 0; iVertex < pNode->vBipedList.size(); iVertex++)
	{
		PNC_VERTEX* pVertex = &pNode->vBipedList[iVertex];
		_fgetts(m_szLine, MAX_PATH * 4, m_pFile);
		_stscanf(m_szLine, _T("%f%f%f %f%f%f %f%f%f%f"),
			&pVertex->p.x, &pVertex->p.y, &pVertex->p.z,					// ������
			&pVertex->n.x, &pVertex->n.y, &pVertex->n.z,					// ���
			&pVertex->c.x, &pVertex->c.y, &pVertex->c.z, &pVertex->c.w);	// �÷�

		if (m_pAnimation->m_Scene.iBindPose)
			D3DXVec3TransformCoord(&pVertex->p, &pVertex->p, &pNode->matWorldInv);
	}

	return hr;
}

bool bgParserBGMTX::ReadMatrix(bgAnimNode* pNode)
{
	bool hr = true;

	int iData;

	IF_FALSE_RETURN(FindWord(_T("#MATRIX_LIST")));
	_stscanf(m_szLine, _T("%s %d"),
		m_szWord,
		&iData);		// �ִϸ��̼� ������ ����
	pNode->matFrameList.resize(iData);

	for (int iMatrix = 0; iMatrix < pNode->matFrameList.size(); iMatrix++)
	{
		D3DXMATRIX* pMatrix = &pNode->matFrameList[iMatrix];
		_fgetts(m_szLine, MAX_PATH * 4, m_pFile);
		_stscanf(m_szLine, _T("%s %f%f%f%f %f%f%f%f %f%f%f%f %f%f%f%f"),
			&pMatrix->_11, &pMatrix->_12, &pMatrix->_13, &pMatrix->_14,		// �����Ӵ��� �ִϸ��̼� ���
			&pMatrix->_21, &pMatrix->_22, &pMatrix->_23, &pMatrix->_24,
			&pMatrix->_31, &pMatrix->_32, &pMatrix->_33, &pMatrix->_34,
			&pMatrix->_41, &pMatrix->_42, &pMatrix->_43, &pMatrix->_44);
	}

	return hr;
}

bool bgParserBGMTX::PostRead()
{
	bool hr = true;

	D3DXQUATERNION qRot;
	D3DXVECTOR3 vPos, vScl;

	for (int iNode = 0; iNode < m_pAnimation->m_NodeList.size(); iNode++)
	{
		bgAnimNode* pNode = &m_pAnimation->m_NodeList[iNode];
		if (pNode->matFrameList.size() > 0)
		{
			pNode->qRotList.resize(pNode->matFrameList.size());
			pNode->vSclList.resize(pNode->matFrameList.size());
			pNode->vPosList.resize(pNode->matFrameList.size());
		}
		for (int iFrame = 0; iFrame < pNode->matFrameList.size(); iFrame++)
		{
			if (SUCCEEDED(D3DXMatrixDecompose(&vScl, &qRot, &vPos, &pNode->matFrameList[iFrame])))
			{
				pNode->qRotList[iFrame] = qRot;
				pNode->vSclList[iFrame] = vScl;
				pNode->vPosList[iFrame] = vPos;
			}
			else
			{
				D3DXQuaternionRotationMatrix(&pNode->qRotList[iFrame], &pNode->matFrameList[iFrame]);
			}
		}
	}

	return hr;
}
