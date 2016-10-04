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

void bgExporterMax::ExpMaterial()
{
	_ftprintf(m_pStream, _T("%s\t%d\n"),
		_T("#MATERIAL_INFO"),
		m_MaterialList.size());		// 메터리얼 갯수

	for (int iRootMtl = 0; iRootMtl < m_MaterialList.size(); iRootMtl++)
	{
		_ftprintf(m_pStream, _T("%s\t%d\t%d\t%d\t%s\t%s\n"),
			_T("#MATERIAL_LIST"),
			m_MaterialList[iRootMtl].iIndex,				// 번호
			m_MaterialList[iRootMtl].TextureMaps.size(),	// 텍스쳐 갯수
			m_MaterialList[iRootMtl].iNumSubMaterial,		// 서브메터리얼 갯수
			((m_MaterialList[iRootMtl].strName.isNull()) ? NULL_TEXT : m_MaterialList[iRootMtl].strName),				// 이름
			((m_MaterialList[iRootMtl].strClassName.isNull()) ? NULL_TEXT : m_MaterialList[iRootMtl].strClassName));	// 종류

		// 서브메터리얼이 있으면
		if (m_MaterialList[iRootMtl].iNumSubMaterial > 0)
		{
			for (int iSubMtl = 0; iSubMtl < m_MaterialList[iRootMtl].iNumSubMaterial; iSubMtl++)
			{
				_ftprintf(m_pStream, _T("\t%s\t%d\t%d\t%d\t%s\t%s\n"),
					_T("#SUBMATERIAL"),
					m_MaterialList[iRootMtl].SubMaterials[iSubMtl].iIndex,				// 서브 번호
					m_MaterialList[iRootMtl].SubMaterials[iSubMtl].TextureMaps.size(),	// 텍스쳐 갯수
					m_MaterialList[iRootMtl].SubMaterials[iSubMtl].iNumSubMaterial,		// 서브메터리얼 갯수
					((m_MaterialList[iRootMtl].SubMaterials[iSubMtl].strName.isNull()) ? NULL_TEXT : m_MaterialList[iRootMtl].SubMaterials[iSubMtl].strName),				// 이름
					((m_MaterialList[iRootMtl].SubMaterials[iSubMtl].strClassName.isNull()) ? NULL_TEXT : m_MaterialList[iRootMtl].SubMaterials[iSubMtl].strClassName));	// 종류

				ExpTexture(m_MaterialList[iRootMtl].SubMaterials[iSubMtl].TextureMaps, _T("\t\t"));
			}
		}
		// 서브메터리얼이 없으면
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
			szLineHead,				// 들여쓰기(탭)
			_T("#TEXTUREMAP"),
			iTexMap,				// 번호
			TexMap[iTexMap].iType,	// 종류
			((TexMap[iTexMap].strTextureName.isNull()) ? NULL_TEXT : TexMap[iTexMap].strTextureName));	// 이름
	}
}

void bgExporterMax::ExpObject()
{
	_ftprintf(m_pStream, _T("%s\t%d\n"),
		_T("#OBJECT_INFO"),
		m_MatrixMap.Count());	// 갯수

	for (int iMesh = 0; iMesh < m_MatrixMap.Count(); iMesh++)
	{
		bgMatrixIndex* pPoint = (bgMatrixIndex*)m_MatrixMap.m_TMap[iMesh];
		bgMesh* pMesh = &pPoint->m_Mesh;

		_ftprintf(m_pStream, _T("%s\t%d\t%d\t%s\t%s\n"),
			_T("#OBJECT_LIST"),
			iMesh,					// 번호
			m_MatrixMap.GetID(pMesh->strParentName),	// 부모번호
			((pMesh->strNodeName.isNull()) ? NULL_TEXT : pMesh->strNodeName),		// 이름
			((pMesh->strParentName.isNull()) ? NULL_TEXT : pMesh->strParentName));	// 부모이름

		_ftprintf(m_pStream, _T("\t%s\t%d\t%d\t%d\t%d\t%d\t%d\n"),
			_T("#OBJECT_DETAIL"),
			pMesh->ClassType,			// 종류
			pMesh->iMaterialRef,		// 루트ID
			pMesh->PosTrack.size(),		// 이동트랙 갯수
			pMesh->RotTrack.size(),		// 회전트랙 갯수
			pMesh->SclTrack.size(),		// 신축트랙 갯수
			pMesh->AlpTrack.size());	// 알파트랙 갯수

		_ftprintf(m_pStream, _T("\t%s\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			_T("#BOUNDING_BOX"),
			pMesh->BoundBox.pmax.x, pMesh->BoundBox.pmax.y, pMesh->BoundBox.pmax.z,	// 바운딩박스 최대
			pMesh->BoundBox.pmin.x, pMesh->BoundBox.pmin.y, pMesh->BoundBox.pmin.z);	// 바운딩박스 최소

		_ftprintf(m_pStream, _T("\t%s\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			_T("#WORLD_MATRIX"),
			pMesh->matWorld._11, pMesh->matWorld._12, pMesh->matWorld._13, pMesh->matWorld._14,	// 월드 행렬
			pMesh->matWorld._21, pMesh->matWorld._22, pMesh->matWorld._23, pMesh->matWorld._24,
			pMesh->matWorld._31, pMesh->matWorld._32, pMesh->matWorld._33, pMesh->matWorld._34,
			pMesh->matWorld._41, pMesh->matWorld._42, pMesh->matWorld._43, pMesh->matWorld._44);

		_ftprintf(m_pStream, _T("\t%s\t%d\n"),
			_T("#TRIANGLE_INFO"),
			pMesh->pSubMeshes.size());		// 서브ID 갯수

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
		iMtrl,						// 서브ID
		VertexArray.size(),			// 정점 갯수
		iNumFaces);					// 페이스 갯수

	for (int iCnt = 0; iCnt < VertexArray.size(); iCnt++)
	{
		_ftprintf(m_pStream, _T("\t\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			VertexArray[iCnt].pos.x, VertexArray[iCnt].pos.y, VertexArray[iCnt].pos.z,								// 포지션
			VertexArray[iCnt].nor.x, VertexArray[iCnt].nor.y, VertexArray[iCnt].nor.z,								// 노멀
			VertexArray[iCnt].col.x, VertexArray[iCnt].col.y, VertexArray[iCnt].col.z, VertexArray[iCnt].col.w,		// 컬러
			VertexArray[iCnt].tex.x, VertexArray[iCnt].tex.y);														// 텍스쳐
	}

	for (int iIndex = 0; iIndex < IndexArray.size(); iIndex += 3)
	{
		_ftprintf(m_pStream, _T("\t\t%d\t%d\t%d\n"),
			IndexArray[iIndex + 0],		// 삼각형 인덱스
			IndexArray[iIndex + 1],
			IndexArray[iIndex + 2]);
	}

	return iNumFaces;
}

void bgExporterMax::ExpAnimation(bgMesh* pMesh)
{
	_ftprintf(m_pStream, _T("\t%s\t%d\n"),
		_T("#POS_TRACK_LIST"),
		pMesh->PosTrack.size());	// 위치 트랙 길이

	for (DWORD dwPosTrack = 0; dwPosTrack < pMesh->PosTrack.size(); dwPosTrack++)
	{
		_ftprintf(m_pStream, _T("\t\t%d\t%f\t%f\t%f\n"),
			pMesh->PosTrack[dwPosTrack].iTick,			// 틱
			pMesh->PosTrack[dwPosTrack].vValue.x,		// 포지션
			pMesh->PosTrack[dwPosTrack].vValue.y,
			pMesh->PosTrack[dwPosTrack].vValue.z);
	}

	_ftprintf(m_pStream, _T("\t%s\t%d\n"),
		_T("#ROT_TRACK_LIST"),
		pMesh->RotTrack.size());	// 회전 트랙 길이

	for (DWORD dwRotTrack = 0; dwRotTrack < pMesh->RotTrack.size(); dwRotTrack++)
	{
		_ftprintf(m_pStream, _T("\t\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			pMesh->RotTrack[dwRotTrack].iTick,			// 틱
			pMesh->RotTrack[dwRotTrack].vValue.x,		// 벡터
			pMesh->RotTrack[dwRotTrack].vValue.y,
			pMesh->RotTrack[dwRotTrack].vValue.z,
			pMesh->RotTrack[dwRotTrack].qValue.x,		// 사원수
			pMesh->RotTrack[dwRotTrack].qValue.y,
			pMesh->RotTrack[dwRotTrack].qValue.z,
			pMesh->RotTrack[dwRotTrack].qValue.w);
	}

	_ftprintf(m_pStream, _T("\t%s\t%d\n"),
		_T("#SCL_TRACK_LIST"),
		pMesh->SclTrack.size());	// 신축 트랙 길이

	for (DWORD dwSclTrack = 0; dwSclTrack < pMesh->SclTrack.size(); dwSclTrack++)
	{
		_ftprintf(m_pStream, _T("\t\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n"),
			pMesh->SclTrack[dwSclTrack].iTick,			// 틱
			pMesh->SclTrack[dwSclTrack].vValue.x,		// 벡터
			pMesh->SclTrack[dwSclTrack].vValue.y,
			pMesh->SclTrack[dwSclTrack].vValue.z,
			pMesh->SclTrack[dwSclTrack].qValue.x,		// 사원수
			pMesh->SclTrack[dwSclTrack].qValue.y,
			pMesh->SclTrack[dwSclTrack].qValue.z,
			pMesh->SclTrack[dwSclTrack].qValue.w);
	}

	_ftprintf(m_pStream, _T("\t%s\t%d\n"),
		_T("#ALP_TRACK_LIST"),
		pMesh->AlpTrack.size());	// 알파 트랙 길이

	for (DWORD dwAlpTrack = 0; dwAlpTrack < pMesh->AlpTrack.size(); dwAlpTrack++)
	{
		_ftprintf(m_pStream, _T("\t\t%d\t%f\n"),
			pMesh->AlpTrack[dwAlpTrack].iTick,			// 틱
			pMesh->AlpTrack[dwAlpTrack].vValue.x);		// 알파
	}
}
