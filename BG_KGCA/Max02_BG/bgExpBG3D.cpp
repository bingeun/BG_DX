#include "bgExporterMax.h"

bool bgExporterMax::ExportBG3D()
{
	m_strFileName = I_Exporter.SaveFileDialog(_T("BG3D"), _T("BG 3D Model Export"));

	m_pStream = _tfopen(m_strFileName, _T("wt"));
	if (!m_pStream)
		return 0;

	{
		GetMaterial();
		UpdateObject();

		ExpScene();
		ExpMaterial();
		ExpObject();
	}

	fclose(m_pStream);
	MessageBox(GetActiveWindow(), m_strFileName, _T("Succeed!!"), MB_OK);

	return true;
}

void bgExporterMax::ExpScene()
{
	struct tm *newtime;
	time_t aclock;

	time(&aclock);
	newtime = localtime(&aclock);

	TSTR today = _tasctime(newtime);
	today.remove(today.length() - 1);

	_ftprintf(m_pStream, _T("%s\t%s\n"),
		_T("#BG3D_MODEL"),
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

void bgExporterMax::ExpMaterial()
{
	_ftprintf(m_pStream, _T("%s\t%d\n"),
		_T("#MATERIAL_INFO"),
		m_MaterialList.size());		// ���͸��� ����

	for (int iRootMtl = 0; iRootMtl < m_MaterialList.size(); iRootMtl++)
	{
		_ftprintf(m_pStream, _T("%s\t%d\t%d\t%d\t%s\t%s\n"),
			_T("#MATERIAL_LIST"),
			m_MaterialList[iRootMtl].iIndex,				// ��ȣ
			m_MaterialList[iRootMtl].TextureMaps.size(),	// �ؽ��� ����
			m_MaterialList[iRootMtl].iNumSubMaterial,		// ������͸��� ����
			((m_MaterialList[iRootMtl].strName.isNull()) ? NULL_TEXT : m_MaterialList[iRootMtl].strName),				// �̸�
			((m_MaterialList[iRootMtl].strClassName.isNull()) ? NULL_TEXT : m_MaterialList[iRootMtl].strClassName));	// ����

		// ������͸����� ������
		if (m_MaterialList[iRootMtl].iNumSubMaterial > 0)
		{
			for (int iSubMtl = 0; iSubMtl < m_MaterialList[iRootMtl].iNumSubMaterial; iSubMtl++)
			{
				_ftprintf(m_pStream, _T("\t%s\t%d\t%d\t%d\t%s\t%s\n"),
					_T("#SUBMATERIAL"),
					m_MaterialList[iRootMtl].SubMaterials[iSubMtl].iIndex,				// ���� ��ȣ
					m_MaterialList[iRootMtl].SubMaterials[iSubMtl].TextureMaps.size(),	// �ؽ��� ����
					m_MaterialList[iRootMtl].SubMaterials[iSubMtl].iNumSubMaterial,		// ������͸��� ����
					((m_MaterialList[iRootMtl].SubMaterials[iSubMtl].strName.isNull()) ? NULL_TEXT : m_MaterialList[iRootMtl].SubMaterials[iSubMtl].strName),				// �̸�
					((m_MaterialList[iRootMtl].SubMaterials[iSubMtl].strClassName.isNull()) ? NULL_TEXT : m_MaterialList[iRootMtl].SubMaterials[iSubMtl].strClassName));	// ����

				ExpTexture(m_MaterialList[iRootMtl].SubMaterials[iSubMtl].TextureMaps, _T("\t\t"));
			}
		}
		// ������͸����� ������
		else
		{
			ExpTexture(m_MaterialList[iRootMtl].TextureMaps, _T("\t"));
		}
	}
}

void bgExporterMax::ExpTexture(vector<bgTextureMap>& TexMap, TCHAR* szLineHead)
{
	for (int iTexMap = 0; iTexMap < TexMap.size(); iTexMap++)
	{
		_ftprintf(m_pStream, _T("%s%s\t%d\t%d\t%s\n"),
			szLineHead,				// �鿩����(��)
			_T("#TEXTUREMAP"),
			iTexMap,				// ��ȣ
			TexMap[iTexMap].iType,	// ����
			((TexMap[iTexMap].strTextureName.isNull()) ? NULL_TEXT : TexMap[iTexMap].strTextureName));	// �̸�
	}
}

void bgExporterMax::ExpObject()
{
	_ftprintf(m_pStream, _T("%s\t%d\n"),
		_T("#OBJECT_INFO"),
		m_MatrixMap.Count());	// ����

	for (int iMesh = 0; iMesh < m_MatrixMap.Count(); iMesh++)
	{
		bgMatrixIndex* pPoint = (bgMatrixIndex*)m_MatrixMap.m_TMap[iMesh];
		bgMesh* pMesh = &pPoint->m_Mesh;

		_ftprintf(m_pStream, _T("%s\t%d\t%d\t%s\t%s\n"),
			_T("#OBJECT_LIST"),
			iMesh,					// ��ȣ
			m_MatrixMap.GetID(pMesh->strParentName),	// �θ��ȣ
			((pMesh->strNodeName.isNull()) ? NULL_TEXT : pMesh->strNodeName),		// �̸�
			((pMesh->strParentName.isNull()) ? NULL_TEXT : pMesh->strParentName));	// �θ��̸�

		_ftprintf(m_pStream, _T("\t%s\t%d\t%d\t%d\t%d\t%d\t%d\n"),
			_T("#OBJECT_DETAIL"),
			pMesh->ClassType,			// ����
			pMesh->iMaterialRef,		// ��ƮID
			pMesh->PosTrack.size(),		// �̵�Ʈ�� ����
			pMesh->RotTrack.size(),		// ȸ��Ʈ�� ����
			pMesh->SclTrack.size(),		// ����Ʈ�� ����
			pMesh->AlpTrack.size());	// ����Ʈ�� ����

		_ftprintf(m_pStream, _T("\t%s\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			_T("#BOUNDING_BOX"),
			pMesh->BoundBox.pmax.x, pMesh->BoundBox.pmax.y, pMesh->BoundBox.pmax.z,	// �ٿ���ڽ� �ִ�
			pMesh->BoundBox.pmin.x, pMesh->BoundBox.pmin.y, pMesh->BoundBox.pmin.z);	// �ٿ���ڽ� �ּ�

		_ftprintf(m_pStream, _T("\t%s\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			_T("#WORLD_MATRIX"),
			pMesh->matWorld._11, pMesh->matWorld._12, pMesh->matWorld._13, pMesh->matWorld._14,	// ���� ���
			pMesh->matWorld._21, pMesh->matWorld._22, pMesh->matWorld._23, pMesh->matWorld._24,
			pMesh->matWorld._31, pMesh->matWorld._32, pMesh->matWorld._33, pMesh->matWorld._34,
			pMesh->matWorld._41, pMesh->matWorld._42, pMesh->matWorld._43, pMesh->matWorld._44);

		_ftprintf(m_pStream, _T("\t%s\t%d\n"),
			_T("#TRIANGLE_INFO"),
			pMesh->pSubMeshes.size());		// ����ID ����

		std::sort(pMesh->Triangles.begin(), pMesh->Triangles.end(), DescendingDateSort());

		int iAddCount = 0;
		if (pMesh->iMaterialRef >= 0 && m_MaterialList[pMesh->iMaterialRef].iNumSubMaterial > 0)
		{
			for (int iMtrl = 0; iMtrl < m_MaterialList[pMesh->iMaterialRef].iNumSubMaterial; iMtrl++)
			{
				iAddCount += ExpMesh(pMesh, iMtrl, iAddCount);
			}
		}
		else
		{
			ExpMesh(pMesh, -1, 0);
		}

		ExpAnimation(pMesh);
	}
}

int bgExporterMax::ExpMesh(bgMesh* pMesh, int iMtrl, int iAddCount)
{
	vector<PNCT_Vertex>	VertexArray;
	vector<int>			IndexArray;

	int iNumFaces = SetUniqueBuffer(pMesh, iMtrl, iAddCount, VertexArray, IndexArray);

	_ftprintf(m_pStream, _T("\t%s\t%d\t%d\t%d\n"),
		_T("#TRIANGLE_LIST"),
		iMtrl,						// ����ID
		VertexArray.size(),			// ���� ����
		iNumFaces);					// ���̽� ����

	for (int iCnt = 0; iCnt < VertexArray.size(); iCnt++)
	{
		_ftprintf(m_pStream, _T("\t\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			VertexArray[iCnt].pos.x, VertexArray[iCnt].pos.y, VertexArray[iCnt].pos.z,								// ������
			VertexArray[iCnt].nor.x, VertexArray[iCnt].nor.y, VertexArray[iCnt].nor.z,								// ���
			VertexArray[iCnt].col.x, VertexArray[iCnt].col.y, VertexArray[iCnt].col.z, VertexArray[iCnt].col.w,		// �÷�
			VertexArray[iCnt].tex.x, VertexArray[iCnt].tex.y);														// �ؽ���
	}

	for (int iIndex = 0; iIndex < IndexArray.size(); iIndex += 3)
	{
		_ftprintf(m_pStream, _T("\t\t%d\t%d\t%d\n"),
			IndexArray[iIndex + 0],		// �ﰢ�� �ε���
			IndexArray[iIndex + 1],
			IndexArray[iIndex + 2]);
	}

	return iNumFaces;
}

void bgExporterMax::ExpAnimation(bgMesh* pMesh)
{
	_ftprintf(m_pStream, _T("\t%s\t%d\n"),
		_T("#POS_TRACK_LIST"),
		pMesh->PosTrack.size());	// ��ġ Ʈ�� ����

	for (DWORD dwPosTrack = 0; dwPosTrack < pMesh->PosTrack.size(); dwPosTrack++)
	{
		_ftprintf(m_pStream, _T("\t\t%d\t%f\t%f\t%f\n"),
			pMesh->PosTrack[dwPosTrack].iTick,			// ƽ
			pMesh->PosTrack[dwPosTrack].vValue.x,		// ������
			pMesh->PosTrack[dwPosTrack].vValue.y,
			pMesh->PosTrack[dwPosTrack].vValue.z);
	}

	_ftprintf(m_pStream, _T("\t%s\t%d\n"),
		_T("#ROT_TRACK_LIST"),
		pMesh->RotTrack.size());	// ȸ�� Ʈ�� ����

	for (DWORD dwRotTrack = 0; dwRotTrack < pMesh->RotTrack.size(); dwRotTrack++)
	{
		_ftprintf(m_pStream, _T("\t\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			pMesh->RotTrack[dwRotTrack].iTick,			// ƽ
			pMesh->RotTrack[dwRotTrack].vValue.x,		// ����
			pMesh->RotTrack[dwRotTrack].vValue.y,
			pMesh->RotTrack[dwRotTrack].vValue.z,
			pMesh->RotTrack[dwRotTrack].qValue.x,		// �����
			pMesh->RotTrack[dwRotTrack].qValue.y,
			pMesh->RotTrack[dwRotTrack].qValue.z,
			pMesh->RotTrack[dwRotTrack].qValue.w);
	}

	_ftprintf(m_pStream, _T("\t%s\t%d\n"),
		_T("#SCL_TRACK_LIST"),
		pMesh->SclTrack.size());	// ���� Ʈ�� ����

	for (DWORD dwSclTrack = 0; dwSclTrack < pMesh->SclTrack.size(); dwSclTrack++)
	{
		_ftprintf(m_pStream, _T("\t\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			pMesh->SclTrack[dwSclTrack].iTick,			// ƽ
			pMesh->SclTrack[dwSclTrack].vValue.x,		// ����
			pMesh->SclTrack[dwSclTrack].vValue.y,
			pMesh->SclTrack[dwSclTrack].vValue.z,
			pMesh->SclTrack[dwSclTrack].qValue.x,		// �����
			pMesh->SclTrack[dwSclTrack].qValue.y,
			pMesh->SclTrack[dwSclTrack].qValue.z,
			pMesh->SclTrack[dwSclTrack].qValue.w);
	}

	_ftprintf(m_pStream, _T("\t%s\t%d\n"),
		_T("#ALP_TRACK_LIST"),
		pMesh->AlpTrack.size());	// ���� Ʈ�� ����

	for (DWORD dwAlpTrack = 0; dwAlpTrack < pMesh->AlpTrack.size(); dwAlpTrack++)
	{
		_ftprintf(m_pStream, _T("\t\t%d\t%f\n"),
			pMesh->AlpTrack[dwAlpTrack].iTick,			// ƽ
			pMesh->AlpTrack[dwAlpTrack].vValue.x);		// ����
	}
}
