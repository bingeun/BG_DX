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

	// ������ �ʱ�ȭ
	ZeroMemory(&m_pModel->m_Scene, sizeof(SceneInfo));
	m_pModel->m_MaterialList.clear();
	m_pModel->m_ObjectList.clear();

	// ���� �б�
	IF_FALSE_RETURN(FindWord(_T("#BG3D_MODEL")));
	IF_FALSE_RETURN(ReadScene());
	IF_FALSE_RETURN(ReadMaterial());
	IF_FALSE_RETURN(ReadObject());

	ConvertToModel();	// ���� �����͸� �𵨿� �����ͷ� ������
	LinkNode();			// ��� ���� ����
	OperationTM();		// ��� ���迡 ���� ��� ����

	return hr;
}

bool bgParserBG3D::ReadScene()
{
	bool hr = true;

	int iData;

	IF_FALSE_RETURN(FindWord(_T("#SCENE_INFO")));
	_stscanf(m_szLine, _T("%s%d%d%d%d%d%d%d%d"),
		m_szWord,
		&iData, //&m_pModel->m_Scene.iVersion,		// ����
		&m_pModel->m_Scene.iFirstFrame,		// ���� ������
		&m_pModel->m_Scene.iLastFrame,		// ������ ������
		&m_pModel->m_Scene.iFrameSpeed,		// ������ ���ǵ�
		&m_pModel->m_Scene.iTicksPerFrame,	// �����Ӵ� ƽ
		&iData, //&m_pModel->m_Scene.iNumMesh,		// �޽� ����
		&iData, //&m_pModel->m_Scene.iMaxWeight,		// ����ġ
		&iData); //&m_pModel->m_Scene.iBindPose);		// ���ε�����

	return hr;
}

bool bgParserBG3D::ReadMaterial()
{
	bool hr = true;

	TCHAR szData[MAX_PATH] = { 0, };
	int iData, iData1, iData2, iData3, iData4;
	int iLoop, iLoopMax;
	int iLoopSub, iLoopSubMax;

	// �ֻ��� ���͸��� ����
	IF_FALSE_RETURN(FindWord(_T("#MATERIAL_INFO")));
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_pModel->m_MaterialList.resize(iData);

	// �ֻ��� ���͸��� ������ŭ �ݺ��ϸ� �б�
	for (int iRootMtl = 0; iRootMtl < m_pModel->m_MaterialList.size(); iRootMtl++)
	{
		IF_FALSE_RETURN(FindWord(_T("#MATERIAL_LIST")));
		_stscanf(m_szLine, _T("%s%d%d%d%s%s"),
			m_szWord,
			&iData,		// ��ȣ
			&iData1,	// �ؽ��� ����
			&iData2,	// ������͸��� ����
			m_pModel->m_MaterialList[iRootMtl].szMaterialName,	// �̸�
			szData);	// ����

		// ������͸����� ������
		if (iData2 > 0)
		{
			m_pModel->m_MaterialList[iRootMtl].SubMaterialList.resize(iData2);
			for (int iSubMtl = 0; iSubMtl < m_pModel->m_MaterialList[iRootMtl].SubMaterialList.size(); iSubMtl++)
			{
				IF_FALSE_RETURN(FindWord(_T("#SUBMATERIAL")));
				_stscanf(m_szLine, _T("%s%d%d%d%s%s"),
					m_szWord,
					&iData,		// ���� ��ȣ
					&iData1,	// �ؽ��� ����
					&iData2,	// ������͸��� ����
					m_pModel->m_MaterialList[iRootMtl].SubMaterialList[iSubMtl].szMaterialName,	// �̸�
					szData);	// ����

				for (int iSubTex = 0; iSubTex < iData1; iSubTex++)
				{
					ReadTexture(iSubTex, &m_pModel->m_MaterialList[iRootMtl], &m_pModel->m_MaterialList[iRootMtl].SubMaterialList[iSubMtl]);
				}
			}
		}
		// ������͸����� ������
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
		&iData,				// ��ȣ
		&iDataType);		// ����

	if (iDataType == 1)
	{
		if (pSubMat == NULL)
		{
			_stscanf(m_szLine, _T("%s%d%d%s"),
				m_szWord,
				&iData,				// ��ȣ
				&iDataType,			// ����
				pMat->szBitmap);	// �̸�
		}
		else
		{
			_stscanf(m_szLine, _T("%s%d%d%s"),
				m_szWord,
				&iData,				// ��ȣ
				&iDataType,			// ����
				pSubMat->szBitmap);	// �̸�
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
		&iData1);	// ����
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
			&iData,					// ��ȣ
			&iData1,				// �θ��ȣ
			pPoint->szNodeName,		// �̸�
			pPoint->szNodeParent);	// �θ��̸�

		IF_FALSE_RETURN(FindWord(_T("#OBJECT_DETAIL")));
		_stscanf(m_szLine, _T("%s %d%d%d%d %d%d%d%d"),
			m_szWord,
			&iData,					// ����
			&pMesh->iMaterialRef,	// ��ƮID
			&iData1,				// �̵�Ʈ�� ����
			&iData2,				// ȸ��Ʈ�� ����
			&iData3,				// ����Ʈ�� ����
			&iData4);				// ����Ʈ�� ����
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
			&vData1.x, &vData1.y, &vData1.z,	// �ٿ���ڽ� �ִ�
			&vData2.x, &vData2.y, &vData2.z);	// �ٿ���ڽ� �ּ�

		IF_FALSE_RETURN(FindWord(_T("#WORLD_MATRIX")));
		_stscanf(m_szLine, _T("%s %f%f%f%f %f%f%f%f %f%f%f%f %f%f%f%f"),
			m_szWord,
			&pPoint->nodeTM.matWorld._11, &pPoint->nodeTM.matWorld._12, &pPoint->nodeTM.matWorld._13, &pPoint->nodeTM.matWorld._14,	// ���� ���
			&pPoint->nodeTM.matWorld._21, &pPoint->nodeTM.matWorld._22, &pPoint->nodeTM.matWorld._23, &pPoint->nodeTM.matWorld._24,
			&pPoint->nodeTM.matWorld._31, &pPoint->nodeTM.matWorld._32, &pPoint->nodeTM.matWorld._33, &pPoint->nodeTM.matWorld._34,
			&pPoint->nodeTM.matWorld._41, &pPoint->nodeTM.matWorld._42, &pPoint->nodeTM.matWorld._43, &pPoint->nodeTM.matWorld._44);

		IF_FALSE_RETURN(FindWord(_T("#TRIANGLE_INFO")));
		_stscanf(m_szLine, _T("%s%d"),
			m_szWord,
			&iData1);		// ����ID ����

		if (pMesh->iMaterialRef >= 0 && m_pModel->m_MaterialList[pMesh->iMaterialRef].SubMaterialList.size() > 0)
		{
			for (int iMtrl = 0; iMtrl < m_pModel->m_MaterialList[pMesh->iMaterialRef].SubMaterialList.size(); iMtrl++)
			{
				IF_FALSE_RETURN(FindWord(_T("#TRIANGLE_LIST")));
				_stscanf(m_szLine, _T("%s%d%d%d"),
					m_szWord,
					&iData,			// ��ȣ
					&iData1,		// ���� ����
					&iData2);		// ���̽� ����

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
				&iData,			// ��ȣ
				&iData1,		// ���� ����
				&iData2);		// ���̽� ����

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
			&pMesh->VertexList[iCnt].x, &pMesh->VertexList[iCnt].y, &pMesh->VertexList[iCnt].z,						// ������
			&pMesh->NorVertexList[iCnt].x, &pMesh->NorVertexList[iCnt].y, &pMesh->NorVertexList[iCnt].z,			// ���
			&pMesh->ColVertexList[iCnt].x, &pMesh->ColVertexList[iCnt].y, &pMesh->ColVertexList[iCnt].z, &fData,	// �÷�
			&pMesh->TexVertexList[iCnt].x, &pMesh->TexVertexList[iCnt].y);											// �ؽ���
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
		&iData);	// ��ġ Ʈ�� ����

	for (DWORD dwPosTrack = 0; dwPosTrack < pMesh->PosTrack.size(); dwPosTrack++)
	{
		_fgetts(m_szLine, MAX_PATH, m_pFile);
		_stscanf(m_szLine, _T("%d %f%f%f"),
			&pMesh->PosTrack[dwPosTrack].iTick,			// ƽ
			&pMesh->PosTrack[dwPosTrack].vVector.x,		// ������
			&pMesh->PosTrack[dwPosTrack].vVector.y,
			&pMesh->PosTrack[dwPosTrack].vVector.z);
	}

	IF_FALSE_RETURN(FindWord(_T("#ROT_TRACK_LIST")));
	_stscanf(m_szLine, _T("%s%d"),
		szData,
		&iData);	// ȸ�� Ʈ�� ����

	for (DWORD dwRotTrack = 0; dwRotTrack < pMesh->RotTrack.size(); dwRotTrack++)
	{
		_fgetts(m_szLine, MAX_PATH, m_pFile);
		_stscanf(m_szLine, _T("%d %f%f%f %f%f%f%f"),
			&pMesh->RotTrack[dwRotTrack].iTick,			// ƽ
			&pMesh->RotTrack[dwRotTrack].vVector.x,		// ����
			&pMesh->RotTrack[dwRotTrack].vVector.y,
			&pMesh->RotTrack[dwRotTrack].vVector.z,
			&pMesh->RotTrack[dwRotTrack].qRotate.x,		// �����
			&pMesh->RotTrack[dwRotTrack].qRotate.y,
			&pMesh->RotTrack[dwRotTrack].qRotate.z,
			&pMesh->RotTrack[dwRotTrack].qRotate.w);
	}

	IF_FALSE_RETURN(FindWord(_T("#SCL_TRACK_LIST")));
	_stscanf(m_szLine, _T("%s%d"),
		szData,
		&iData);	// ���� Ʈ�� ����

	for (DWORD dwSclTrack = 0; dwSclTrack < pMesh->SclTrack.size(); dwSclTrack++)
	{
		_fgetts(m_szLine, MAX_PATH, m_pFile);
		_stscanf(m_szLine, _T("%d %f%f%f %f%f%f%f"),
			&pMesh->SclTrack[dwSclTrack].iTick,			// ƽ
			&pMesh->SclTrack[dwSclTrack].vVector.x,		// ����
			&pMesh->SclTrack[dwSclTrack].vVector.y,
			&pMesh->SclTrack[dwSclTrack].vVector.z,
			&pMesh->SclTrack[dwSclTrack].qRotate.x,		// �����
			&pMesh->SclTrack[dwSclTrack].qRotate.y,
			&pMesh->SclTrack[dwSclTrack].qRotate.z,
			&pMesh->SclTrack[dwSclTrack].qRotate.w);
	}

	IF_FALSE_RETURN(FindWord(_T("#ALP_TRACK_LIST")));
	_stscanf(m_szLine, _T("%s%d"),
		szData,
		&iData);	// ��ġ Ʈ�� ����

	for (DWORD dwAlpTrack = 0; dwAlpTrack < pMesh->AlpTrack.size(); dwAlpTrack++)
	{
		_fgetts(m_szLine, MAX_PATH, m_pFile);
		_stscanf(m_szLine, _T("%d %f"),
			&pMesh->AlpTrack[dwAlpTrack].iTick,			// ƽ
			&pMesh->AlpTrack[dwAlpTrack].vVector.x);	// ����
	}

	return hr;
}

// ���Ͽ��� ���� �����͸� ��(��¿�) �����ͷ� ��ȯ
void bgParserBG3D::ConvertToModel()
{
	TCHAR szFileName[MAX_PATH] = { 0, };
	int iLoop, iLoopSub;
	int iNumMaterial;
	int iNumSubMaterial;

	// ���͸��� �ؽ��� �ε�
	m_pModel->m_TexIDList.resize(m_pModel->m_MaterialList.size());
	iNumMaterial = 0;
	for (iLoop = 0; iLoop < m_pModel->m_MaterialList.size(); iLoop++)
	{
		// ���� ���͸����� ������
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
		// ���� ���͸����� ������
		else
		{
			iNumMaterial++;
			_stscanf(szFileName, _T("%s%s\0"), _T("../../data/model/"), m_pModel->m_MaterialList[iLoop].szBitmap);
			m_pModel->m_TexIDList[iLoop].SubIDList[iLoopSub].iID = 
			m_pModel->m_TexIDList[iLoop].iID = I_TextureMgr.Add(szFileName);
		}
	}
	// �������� �з��Ͽ� �����迭�� ������ �߰�
	// �ؽ��� ������(iLoop => MTLID)�� �ش������� ����(iLoopSub)��ŭ �ݺ��ϸ� �ε�����ȣ ����
}

void bgParserBG3D::LinkNode()
{
	int iNode, iFindParent;

	// ��ü ����� ���� ����
	for (iNode = 0; iNode < m_pModel->m_ObjectList.size(); iNode++)
	{
		// �θ����� �̸��� �������� Ž��
		if (_tcsicmp(m_pModel->m_ObjectList[iNode].szNodeParent, _T("(NULL)")))
		{
			for (iFindParent = 0; iFindParent < m_pModel->m_ObjectList.size(); iFindParent++)
			{
				// �θ��带 ã���� �θ���� �ڽĳ�� ���ΰ��� ������ ����
				if (!_tcsicmp(m_pModel->m_ObjectList[iNode].szNodeParent, m_pModel->m_ObjectList[iFindParent].szNodeName))
				{
					m_pModel->m_ObjectList[iNode].pNodeParent = &m_pModel->m_ObjectList[iFindParent];
					m_pModel->m_ObjectList[iFindParent].pNodeChildList.push_back(&m_pModel->m_ObjectList[iNode]);
					break;
				}
			}

			// ã�� �θ��尡 ������ (�������� ASE �������̸� �׻� �����̾�� ��)
			if (iFindParent >= m_pModel->m_ObjectList.size())
			{
				m_pModel->m_ObjectList[iNode].pNodeParent = NULL;
			}
		}
	}

	// ���ۿ�����Ʈ�̰� �ڽĳ�尡 ������ ���� (������ �ʴ� ������Ʈ)
	for (iNode = 0; iNode < m_pModel->m_ObjectList.size(); iNode++)
	{
		if (m_pModel->m_ObjectList[iNode].eNodeType == OBJECT_NODE_TYPE_HELPEROBJECT &&
			m_pModel->m_ObjectList[iNode].pNodeChildList.size() == 0)
		{
			// ���� ó��
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
