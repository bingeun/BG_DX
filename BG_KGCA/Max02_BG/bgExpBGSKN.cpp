#include "bgExporterSkin.h"

bool bgExporterSkin::ExportBGSKN()
{
	int iSkinMeshCount = m_SkinMeshList.size();
	if (iSkinMeshCount <= 0)
	{
		MessageBox(NULL, _T("����� ������ �����ϴ�."), _T("���ϻ�������"), MB_ICONERROR);
		return FALSE;
	}

	m_strFileName = I_ExporterSkin.SaveFileDialog(_T("BGSKN"), _T("BG Skin Export"));

	m_pStream = _tfopen(m_strFileName, _T("wt"));
	if (!m_pStream)
		return 0;

	{
		m_Scene.iNumMesh = m_SkinMeshList.size();
		ExpScene();

		ExpSkinMesh();

		ExpNodeMatrix();
	}
	
	fclose(m_pStream);
	Release();
	MessageBox(GetActiveWindow(), m_strFileName, _T("Succeed!!"), MB_OK);

	return true;
}

void bgExporterSkin::ExpScene()
{
	struct tm *newtime;
	time_t aclock;

	time(&aclock);
	newtime = localtime(&aclock);

	TSTR today = _tasctime(newtime);
	today.remove(today.length() - 1);

	_ftprintf(m_pStream, _T("%s\t%s\n"),
		_T("#BG3D_SKIN"),
		FixupName(today));		// ��¥

	_ftprintf(m_pStream, _T("%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n"),
		_T("#SCENE_INFO"),
		m_Scene.iVersion,		// ����
		m_Scene.iFirstFrame,	// ���� ������
		m_Scene.iLastFrame,		// ������ ������
		m_Scene.iFrameSpeed,	// ������ ���ǵ�
		m_Scene.iTickPerFrame,	// �����Ӵ� ƽ
		m_Scene.iNumMesh,		// �޽� ����
		m_Scene.iMaxWeight,		// ����ġ
		m_Scene.iBindPose);		// ���ε�����
}

void bgExporterSkin::ExpSkinMesh()
{
	_ftprintf(m_pStream, _T("%s\t%d\n"),
		_T("#OBJECT_INFO"),
		m_SkinMeshList.size());		// ����

	for (int iter = 0; iter < m_SkinMeshList.size(); iter++)
	{
		bgSkinMesh* pMesh = m_SkinMeshList[iter];
		TCHAR szName[256] = { 0, };
		TCHAR szParentName[256] = { 0, };

		_ftprintf(m_pStream, _T("%s\t%d\t%d\t%s\t%s\n"),
			_T("#OBJECT_LIST"),
			iter,					// ��ȣ
			-1,						// �θ��ȣ
			((pMesh->strNodeName.isNull()) ? NULL_TEXT : pMesh->strNodeName),		// �̸�
			((pMesh->strParentName.isNull()) ? NULL_TEXT : pMesh->strParentName));	// �θ��̸�

		_ftprintf(m_pStream, _T("\t%s\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			_T("#WORLD_MATRIX"),
			pMesh->matWorld._11, pMesh->matWorld._12, pMesh->matWorld._13, pMesh->matWorld._14,	// ���� ���
			pMesh->matWorld._21, pMesh->matWorld._22, pMesh->matWorld._23, pMesh->matWorld._24,
			pMesh->matWorld._31, pMesh->matWorld._32, pMesh->matWorld._33, pMesh->matWorld._34,
			pMesh->matWorld._41, pMesh->matWorld._42, pMesh->matWorld._43, pMesh->matWorld._44);
		
		_ftprintf(m_pStream, _T("\t%s\t%d\n"),
			_T("#TRIANGLE_INFO"),
			pMesh->pSubMeshes.size());		// ����ID ����

		if (pMesh->pSubMeshes.size() <= 0)
		{
			ExpMesh(pMesh, 0);
		}
		else
		{
			for (int iMesh = 0; iMesh < pMesh->pSubMeshes.size(); iMesh++)
			{
				ExpMesh(pMesh->m_pSkinSubMesh[iMesh], iMesh);
			}
		}
	}
}

bool bgExporterSkin::ExpMesh(bgSkinMesh* pMesh, int iNum)
{
	vector<PNCT5_Vertex> VertexArray;
	vector<unsigned int> IndexArray;
	VertexArray.reserve(pMesh->iNumFace * 3);
	IndexArray.reserve(pMesh->iNumFace * 3);

	int iPosPushCnt = 0;
	for (int iFace = 0; iFace < pMesh->iNumFace; iFace++)
	{
		for (int iCnt = 0; iCnt < 3; iCnt++)
		{
			int iPosReturn = IsEqualVertexList(VertexArray, pMesh->m_SkinTriList[iFace].m_vVertex[iCnt]);
			if (iPosReturn < 0)
			{
				VertexArray.push_back(pMesh->m_SkinTriList[iFace].m_vVertex[iCnt]);
				iPosReturn = iPosPushCnt++;
			}
			IndexArray.push_back(iPosReturn);
		}
	}
	pMesh->iNumVertex = VertexArray.size();

	_ftprintf(m_pStream, _T("\t%s\t%d\t%d\t%d\n"),
		_T("#TRIANGLE_LIST"),
		iNum,						// ����ID
		pMesh->iNumVertex,			// ���� ����
		pMesh->iNumFace,			// ���̽� ����
		pMesh->m_iNumTex);			// �ؽ��� ����

	for (int iCnt = 0; iCnt < VertexArray.size(); iCnt++)
	{
		_ftprintf(m_pStream, _T("\t\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			VertexArray[iCnt].pos.x, VertexArray[iCnt].pos.y, VertexArray[iCnt].pos.z,								// ������
			VertexArray[iCnt].nor.x, VertexArray[iCnt].nor.y, VertexArray[iCnt].nor.z,								// ���
			VertexArray[iCnt].col.x, VertexArray[iCnt].col.y, VertexArray[iCnt].col.z, VertexArray[iCnt].col.w,		// �÷�
			VertexArray[iCnt].tex.x, VertexArray[iCnt].tex.y,														// �ؽ���
			VertexArray[iCnt].w1[0], VertexArray[iCnt].w1[1], VertexArray[iCnt].w1[2], VertexArray[iCnt].w1[3],		// W0, fW1, fW2, fW3
			VertexArray[iCnt].i1[0], VertexArray[iCnt].i1[1], VertexArray[iCnt].i1[2], VertexArray[iCnt].i1[3],		// I0, I1, I2, I3
			VertexArray[iCnt].w2[0], VertexArray[iCnt].w2[1], VertexArray[iCnt].w2[2], VertexArray[iCnt].w2[3],		// W4, fW5, fW6, fNumWeight
			VertexArray[iCnt].i2[0], VertexArray[iCnt].i2[1], VertexArray[iCnt].i2[2], VertexArray[iCnt].i2[3]);	// I4, I5, I6, I7
	}

	for (int iIndex = 0; iIndex < IndexArray.size(); iIndex += 3)
	{
		_ftprintf(m_pStream, _T("\t\t%d\t%d\t%d\n"),
			IndexArray[iIndex + 0],		// �ﰢ�� �ε���
			IndexArray[iIndex + 1],
			IndexArray[iIndex + 2]);
	}

	for (int itex = 0; itex < pMesh->m_iNumTex; itex++)
	{
		_ftprintf(m_pStream, _T("\t%s\t%d\t%s\n"),
			_T("#TEXTURE_INFO"),
			pMesh->m_iTexType[itex],	// Ÿ��
			pMesh->m_szTexName[itex]);	// �̸�
	}

	return true;
}

void bgExporterSkin::ExpNodeMatrix()
{
	bgMatrixIndex* matIndex;
	D3D_MATRIX d3dMat;
	int iNumMatrix = I_Exporter.m_MatrixMap.m_TMap.size();

	_ftprintf(m_pStream, _T("%s\t%d\n"),
		_T("#NODE_INFO"),
		iNumMatrix);	// ����

	for (int iMat = 0; iMat < iNumMatrix; iMat++)
	{
		matIndex = I_Exporter.m_MatrixMap.GetPtr(iMat);
		I_Exporter.DumpMatrix3(&matIndex->m_InvNodeTM, d3dMat);

		_ftprintf(m_pStream, _T("%s\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			_T("#MATRIX_LIST"),
			iMat,											// ��ȣ
			d3dMat._11, d3dMat._12, d3dMat._13, d3dMat._14,	// �����е� ���
			d3dMat._21, d3dMat._22, d3dMat._23, d3dMat._24,
			d3dMat._31, d3dMat._32, d3dMat._33, d3dMat._34,
			d3dMat._41, d3dMat._42, d3dMat._43, d3dMat._44);
	}
}
