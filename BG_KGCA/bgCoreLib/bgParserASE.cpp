#include "bgParserASE.h"

TCHAR*	szWordASE[] =
{
	_T("}"),		// ASE_END_OF_SECTION
	_T("*SCENE"),
	_T("*SCENE_FIRSTFRAME"),
	_T("*SCENE_LASTFRAME"),
	_T("*SCENE_FRAMESPEED"),
	_T("*SCENE_TICKSPERFRAME"),
	_T("*MATERIAL_LIST"),
	_T("*MATERIAL_COUNT"),
	_T("*MATERIAL"),
	_T("*MATERIAL_NAME"),
	_T("*NUMSUBMTLS"),
	_T("*SUBMATERIAL"),
	_T("*MAP_DIFFUSE"),
	_T("*MAP_SUBNO"),
	_T("*BITMAP"),
	_T("*GEOMOBJECT"),
	_T("*NODE_TM"),
	_T("*TM_ROW0"),
	_T("*TM_ROW1"),
	_T("*TM_ROW2"),
	_T("*TM_ROW3"),
	_T("*MESH"),
	_T("*MESH_NUMVERTEX"),
	_T("*MESH_NUMFACES"),
	_T("*MESH_VERTEX_LIST"),
	_T("*MESH_VERTEX"),
	_T("*MESH_FACE_LIST"),
	_T("*MESH_FACE"),
	_T("*MESH_NUMTVERTEX"),
	_T("*MESH_TVERTLIST"),
	_T("*MESH_TVERT"),
	_T("*MESH_NUMTVFACES"),
	_T("*MESH_TFACELIST"),
	_T("*MESH_TFACE"),
	_T("*MESH_NUMCVERTEX"),
	_T("*MESH_CVERTLIST"),
	_T("*MESH_VERTCOL"),
	_T("*MESH_NUMCVFACES"),
	_T("*MESH_CFACELIST"),
	_T("*MESH_CFACE"),
	_T("*MESH_NORMALS"),
	_T("*MESH_FACENORMAL"),
	_T("*MESH_VERTEXNORMAL"),
	_T("*MATERIAL_REF"),
};

bgParserASE::bgParserASE()
{
}

bgParserASE::~bgParserASE()
{
}

bool bgParserASE::Init()
{
	return true;
}

bool bgParserASE::Read()
{
	int iLoop, iLoopMax;
	int iLoopSub, iLoopSubMax;
	int iFindWordIndex;

	int iData;
	FaceInfo i4Data;
	D3DXVECTOR3 v3Data;

	///// SCENE - 씬 데이터
	if (!FindWord(szWordASE[ASE_SCENE])) return false;						//	*SCENE {
	if (!FindWord(szWordASE[ASE_SCENE_FIRSTFRAME])) return false;			//		*SCENE_FIRSTFRAME
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &m_ASE.Scene.iFirstFrame);
	if (!FindWord(szWordASE[ASE_SCENE_LASTFRAME])) return false;			//		*SCENE_LASTFRAME
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &m_ASE.Scene.iLastFrame);
	if (!FindWord(szWordASE[ASE_SCENE_FRAMESPEED])) return false;			//		*SCENE_FRAMESPEED
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &m_ASE.Scene.iFrameSpeed);
	if (!FindWord(szWordASE[ASE_SCENE_TICKSPERFRAME])) return false;		//		*SCENE_TICKSPERFRAME
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &m_ASE.Scene.iTicksPerFrame);
	if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;				//	}

	///// MATERIAL_LIST - 메터리얼 리스트
	if (!FindWord(szWordASE[ASE_MATERIAL_LIST])) return false;				//	*MATERIAL_LIST {
	if (!FindWord(szWordASE[ASE_MATERIAL_COUNT])) return false;				//		*MATERIAL_COUNT
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_ASE.MaterialList.resize(iData);

	TCHAR* pszToken;
	TCHAR* pszMaterialWords[2];
	iLoopMax = m_ASE.MaterialList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		if (!FindWord(szWordASE[ASE_MATERIAL])) return false;				//		*MATERIAL {
		if (!FindWord(szWordASE[ASE_MATERIAL_NAME])) return false;			//			*MATERIAL_NAME
		ZeroMemory(m_ASE.MaterialList[iLoop].szMaterialName, MAX_PATH);
		pszToken = _tcstok(m_szLine, _T("\""));
		pszToken = _tcstok(NULL, _T("\""));
		_tcscpy(m_ASE.MaterialList[iLoop].szMaterialName, pszToken);

		// 서브 메터리얼!!!!!
		pszMaterialWords[0] = szWordASE[ASE_NUMSUBMTLS];
		pszMaterialWords[1] = szWordASE[ASE_MAP_SUBNO];
		iFindWordIndex = FindWordArray(pszMaterialWords, 2);
		// 만약 서브 메터리얼이 있다면
		if (iFindWordIndex == 0)
		{
			_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
			m_ASE.MaterialList[iLoop].SubMaterialList.resize(iData);
			iLoopSubMax = iData;
			for (iLoopSub = 0; iLoopSub < iLoopSubMax; iLoopSub++)
			{
				if (!FindWord(szWordASE[ASE_SUBMATERIAL])) return false;	//			*SUBMATERIAL {
				if (!FindWord(szWordASE[ASE_MAP_SUBNO])) return false;		//				*MAP_??? { *MAP_SUBNO

				// 비트맵이 있는 메터리얼인지 검사
				pszMaterialWords[0] = szWordASE[ASE_BITMAP];
				pszMaterialWords[1] = szWordASE[ASE_END_OF_SECTION];
				iFindWordIndex = FindWordArray(pszMaterialWords, 2);
				// 만약 비트맵이 있다면
				if (iFindWordIndex == 0)
				{
					ZeroMemory(m_ASE.MaterialList[iLoop].SubMaterialList[iLoopSub].szBitmap, MAX_PATH);
					pszToken = _tcstok(m_szLine, _T("\""));
					pszToken = _tcstok(NULL, _T("\""));
					_tcscpy(m_ASE.MaterialList[iLoop].SubMaterialList[iLoopSub].szBitmap, pszToken);
				}
				// 비트맵이 없는 메터리얼이면
				else if (iFindWordIndex == 1)
				{
					if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;	//	}
				}
			}
		}
		// 서브 메터리얼 없이 곧바로 비트맵이 있다면
		else if (iFindWordIndex == 1)
		{
			m_ASE.MaterialList[iLoop].SubMaterialList.resize(1);

			if (!FindWord(szWordASE[ASE_BITMAP])) return false;				//			*MAP_??? { *BITMAP
			ZeroMemory(m_ASE.MaterialList[iLoop].SubMaterialList[0].szBitmap, MAX_PATH);
			pszToken = _tcstok(m_szLine, _T("\""));
			pszToken = _tcstok(NULL, _T("\""));
			_tcscpy(m_ASE.MaterialList[iLoop].SubMaterialList[0].szBitmap, pszToken);

			if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;		//			}
			if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;		//		}
		}
	}
	if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;				//	}

	///// GEOMOBJECT - 지오메트리 오브젝트
	m_ASE.ObjectList.resize(1);
	if (!FindWord(szWordASE[ASE_GEOMOBJECT])) return false;					//	*GEOMOBJECT {
	if (!FindWord(szWordASE[ASE_NODE_TM])) return false;					//		*NODE_TM {
	if (!FindWord(szWordASE[ASE_TM_ROW0])) return false;					//			*TM_ROW0
	_stscanf(m_szLine, _T("%s%f%f%f"), m_szWord, &v3Data.x, &v3Data.z, &v3Data.y);
	m_ASE.ObjectList[0].matWorld._11 = v3Data.x;
	m_ASE.ObjectList[0].matWorld._12 = v3Data.y;
	m_ASE.ObjectList[0].matWorld._13 = v3Data.z;
	m_ASE.ObjectList[0].matWorld._14 = 0.0f;

	if (!FindWord(szWordASE[ASE_TM_ROW1])) return false;					//			*TM_ROW1
	_stscanf(m_szLine, _T("%s%f%f%f"), m_szWord, &v3Data.x, &v3Data.z, &v3Data.y);
	m_ASE.ObjectList[0].matWorld._31 = v3Data.x;
	m_ASE.ObjectList[0].matWorld._32 = v3Data.y;
	m_ASE.ObjectList[0].matWorld._33 = v3Data.z;
	m_ASE.ObjectList[0].matWorld._34 = 0.0f;

	if (!FindWord(szWordASE[ASE_TM_ROW2])) return false;					//			*TM_ROW2
	_stscanf(m_szLine, _T("%s%f%f%f"), m_szWord, &v3Data.x, &v3Data.z, &v3Data.y);
	m_ASE.ObjectList[0].matWorld._21 = v3Data.x;
	m_ASE.ObjectList[0].matWorld._22 = v3Data.y;
	m_ASE.ObjectList[0].matWorld._23 = v3Data.z;
	m_ASE.ObjectList[0].matWorld._24 = 0.0f;

	if (!FindWord(szWordASE[ASE_TM_ROW3])) return false;					//			*TM_ROW3
	_stscanf(m_szLine, _T("%s%f%f%f"), m_szWord, &v3Data.x, &v3Data.z, &v3Data.y);
	m_ASE.ObjectList[0].matWorld._41 = v3Data.x;
	m_ASE.ObjectList[0].matWorld._42 = v3Data.y;
	m_ASE.ObjectList[0].matWorld._43 = v3Data.z;
	m_ASE.ObjectList[0].matWorld._44 = 1.0f;

	if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;				//		}

	///// MESH - 메쉬 데이터
	if (!FindWord(szWordASE[ASE_MESH])) return false;						//		*MESH {
	if (!FindWord(szWordASE[ASE_MESH_NUMVERTEX])) return false;				//			*MESH_NUMVERTEX
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_ASE.ObjectList[0].VertexList.resize(iData);

	if (!FindWord(szWordASE[ASE_MESH_NUMFACES])) return false;				//			*MESH_NUMFACES
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_ASE.ObjectList[0].FaceList.resize(iData);

	///// VERTEX_LIST - 버텍스 리스트
	D3DXMATRIX matInverse;
	D3DXMatrixInverse(&matInverse, NULL, &m_ASE.ObjectList[0].matWorld);

	if (!FindWord(szWordASE[ASE_MESH_VERTEX_LIST])) return false;			//			*MESH_VERTEX_LIST {
	iLoopMax = m_ASE.ObjectList[0].VertexList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		if (!FindWord(szWordASE[ASE_MESH_VERTEX])) return false;			//				*MESH_VERTEX
		_stscanf(m_szLine, _T("%s%d%f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		D3DXVec3TransformCoord(&v3Data, &v3Data, &matInverse);
		m_ASE.ObjectList[0].VertexList[iLoop] = v3Data;
	}
	if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;				//			}

	///// FACE_LIST - 페이스 리스트
	if (!FindWord(szWordASE[ASE_MESH_FACE_LIST])) return false;				//			*MESH_FACE_LIST {
	iLoopMax = m_ASE.ObjectList[0].FaceList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		if (!FindWord(szWordASE[ASE_MESH_FACE])) return false;				//				*MESH_FACE
		_stscanf(m_szLine, _T("%s%s %s%d %s%d %s%d %s%d %s%d %s%d %s%d %s%d"),
			m_szWord, m_szWord,			// *MESH_FACE  0:
			m_szWord, &i4Data.iA,
			m_szWord, &i4Data.iC,
			m_szWord, &i4Data.iB,		// A:  B:  C:
			m_szWord, &iData,
			m_szWord, &iData,
			m_szWord, &iData,			// AB: BC: CA:
			m_szWord, &iData,			// *MESH_SMOOTHING
			m_szWord, &i4Data.iID);		// *MESH_MTLID
		m_ASE.ObjectList[0].FaceList[iLoop] = i4Data;
	}
	if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;				//			}

	if (!FindWord(szWordASE[ASE_MESH_NUMTVERTEX])) return false;			//			*MESH_NUMTVERTEX
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_ASE.ObjectList[0].TexVertexList.resize(iData);

	///// TVERTLIST - 텍스쳐 버텍스 리스트
	if (!FindWord(szWordASE[ASE_MESH_TVERTLIST])) return false;				//			*MESH_TVERTLIST {
	iLoopMax = m_ASE.ObjectList[0].TexVertexList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		if (!FindWord(szWordASE[ASE_MESH_TVERT])) return false;				//				*MESH_TVERT
		_stscanf(m_szLine, _T("%s%d%f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.y, &v3Data.z);
		m_ASE.ObjectList[0].TexVertexList[iLoop] = v3Data;
	}
	if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;				//			}

	if (!FindWord(szWordASE[ASE_MESH_NUMTVFACES])) return false;			//			*MESH_NUMTVFACES
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_ASE.ObjectList[0].TexFaceList.resize(iData);

	///// TFACELIST - 텍스쳐 페이스 리스트
	if (!FindWord(szWordASE[ASE_MESH_TFACELIST])) return false;				//			*MESH_TFACELIST {
	iLoopMax = m_ASE.ObjectList[0].TexFaceList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		if (!FindWord(szWordASE[ASE_MESH_TFACE])) return false;				//				*MESH_TFACE
		_stscanf(m_szLine, _T("%s%d%d%d%d"), m_szWord, &iData, &i4Data.i[0], &i4Data.i[2], &i4Data.i[1]);
		i4Data.i[3] = 0;
		m_ASE.ObjectList[0].TexFaceList[iLoop] = i4Data;
	}
	if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;				//			}

	///// CVERTLIST - 컬러 버텍스&페이스 리스트
	if (!FindWord(szWordASE[ASE_MESH_NUMCVERTEX])) return false;			//			*MESH_NUMCVERTEX
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	if (iData == 0) // ASE 파일에 색상 정보가 없다면 임의색상 추가
	{
		// 버텍스 컬러
		iLoopMax = m_ASE.ObjectList[0].VertexList.size();
		m_ASE.ObjectList[0].ColVertexList.resize(iLoopMax);
		for (iLoop = 0; iLoop < iLoopMax; iLoop++)
		{
			m_ASE.ObjectList[0].ColVertexList[iLoop] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		}

		// 컬러의 인덱스
		iLoopMax = m_ASE.ObjectList[0].FaceList.size();
		m_ASE.ObjectList[0].ColFaceList.resize(iLoopMax);
		for (iLoop = 0; iLoop < iLoopMax; iLoop++)
		{
			m_ASE.ObjectList[0].ColFaceList[iLoop] = m_ASE.ObjectList[0].FaceList[iLoop];
		}
	}
	else // ASE 파일에 색상 정보가 있다면 해당색상 저장
	{
		// 버텍스 컬러
		iLoopMax = iData;
		m_ASE.ObjectList[0].ColVertexList.resize(iLoopMax);
		if (!FindWord(szWordASE[ASE_MESH_CVERTLIST])) return false;			//			*MESH_CVERTLIST {
		for (iLoop = 0; iLoop < iLoopMax; iLoop++)
		{
			if (!FindWord(szWordASE[ASE_MESH_VERTCOL])) return false;		//				*MESH_VERTCOL
			_stscanf(m_szLine, _T("%s%d%f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.y, &v3Data.z);
			m_ASE.ObjectList[0].ColVertexList[iLoop] = v3Data;
		}
		if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;			//			}

		// 컬러의 인덱스
		if (!FindWord(szWordASE[ASE_MESH_NUMCVFACES])) return false;		//			*MESH_NUMCVFACES
		_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
		iLoopMax = iData;
		m_ASE.ObjectList[0].ColFaceList.resize(iLoopMax);
		if (!FindWord(szWordASE[ASE_MESH_CFACELIST])) return false;			//			*MESH_CFACELIST {
		for (iLoop = 0; iLoop < iLoopMax; iLoop++)
		{
			if (!FindWord(szWordASE[ASE_MESH_CFACE])) return false;			//				*MESH_CFACE
			_stscanf(m_szLine, _T("%s%d%d%d%d"), m_szWord, &iData, &i4Data.i[0], &i4Data.i[2], &i4Data.i[1]);
			i4Data.i[3] = 0;
			m_ASE.ObjectList[0].ColFaceList[iLoop] = i4Data;
		}
		if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;			//			}
	}

	///// NORMALS - 노말 버텍스&페이스 리스트
	if (!FindWord(szWordASE[ASE_MESH_NORMALS])) return false;				//			*MESH_NORMALS {
	iLoopMax = m_ASE.ObjectList[0].FaceList.size();
	m_ASE.ObjectList[0].NorFaceList.resize(iLoopMax);
	m_ASE.ObjectList[0].NorVertexList.resize(iLoopMax * 3);
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		if (!FindWord(szWordASE[ASE_MESH_FACENORMAL])) return false;		//				*MESH_FACENORMAL
		_stscanf(m_szLine, _T("%s%d%f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		m_ASE.ObjectList[0].NorFaceList[iLoop] = v3Data;

		if (!FindWord(szWordASE[ASE_MESH_VERTEXNORMAL])) return false;		//					*MESH_VERTEXNORMAL
		_stscanf(m_szLine, _T("%s%d%f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		m_ASE.ObjectList[0].NorVertexList[iLoop * 3 + 0] = v3Data;

		if (!FindWord(szWordASE[ASE_MESH_VERTEXNORMAL])) return false;		//					*MESH_VERTEXNORMAL
		_stscanf(m_szLine, _T("%s%d%f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		m_ASE.ObjectList[0].NorVertexList[iLoop * 3 + 2] = v3Data;

		if (!FindWord(szWordASE[ASE_MESH_VERTEXNORMAL])) return false;		//					*MESH_VERTEXNORMAL
		_stscanf(m_szLine, _T("%s%d%f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		m_ASE.ObjectList[0].NorVertexList[iLoop * 3 + 1] = v3Data;
	}
	if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;				//			}
	if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;				//		}

	if (!FindWord(szWordASE[ASE_MATERIAL_REF])) return false;				//		*MATERIAL_REF
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &m_ASE.ObjectList[0].iMaterialRef);

	if (!FindWord(szWordASE[ASE_END_OF_SECTION])) return false;				//	}

	return true;
}

// ASE 파일에서 읽은 데이터를 모델(출력용) 데이터로 변환
bool bgParserASE::ConvertToModel(bgModel* pModel)
{
	pModel->m_pVBList;		// 버텍스버퍼 *
	pModel->m_pIBList;		// 인덱스버퍼 *
	pModel->m_VertexList;	// 버텍스 데이터 {}
	pModel->m_IndexList;	// 인덱스 데이터 {}
	pModel->m_TexIDList;	// 메터리얼.서브메터리얼 텍스쳐ID

	int iLoop, iLoopSub;
	int iFace, iTri;
	int iNumMaterial;
	int iNumSubMaterial;

	// 메터리얼 텍스쳐 로드
	pModel->m_TexIDList.resize(m_ASE.MaterialList.size());
	iNumMaterial = 0;
	for (iLoop = 0; iLoop < m_ASE.MaterialList.size(); iLoop++)
	{
		// 서브 메터리얼이 있으면
		if (m_ASE.MaterialList[iLoop].SubMaterialList.size())
		{	
			iNumSubMaterial = m_ASE.MaterialList[iLoop].SubMaterialList.size();
			pModel->m_TexIDList[iLoop].SubIDList.resize(iNumSubMaterial);
			for (iLoopSub = 0; iLoopSub < iNumSubMaterial; iLoopSub++)
			{
				iNumMaterial++;
				pModel->m_TexIDList[iLoop].SubIDList[iLoopSub].iID =
					I_TextureMgr.Add(m_ASE.MaterialList[iLoop].SubMaterialList[iLoopSub].szBitmap);
			}
		}
		// 서브 메터리얼이 없으면
		else
		{
			iNumMaterial++;
			pModel->m_TexIDList[iLoop].iID = I_TextureMgr.Add(m_ASE.MaterialList[iLoop].szBitmap);
		}
	}
	
	// 종류별로 분류하여 데이터 추가
	VertexPNCT vertexTemp;
	int iTexID, iTriIndex;
	pModel->m_VertexList.resize(iNumMaterial);
	for (iFace = 0; iFace < m_ASE.ObjectList[0].FaceList.size(); iFace++)
	{
		for (iTri = 0; iTri < 3; iTri++)
		{
			iTexID = m_ASE.ObjectList[0].FaceList[iFace].iID;

			iTriIndex = m_ASE.ObjectList[0].FaceList[iFace].i[iTri];
			vertexTemp.pos = m_ASE.ObjectList[0].VertexList[iTriIndex];
			vertexTemp.nor = m_ASE.ObjectList[0].NorVertexList[iFace * 3 + iTri];
			iTriIndex = m_ASE.ObjectList[0].ColFaceList[iFace].i[iTri];
			vertexTemp.col = D3DXVECTOR4(m_ASE.ObjectList[0].ColVertexList[iTriIndex], 1.0f);
			iTriIndex = m_ASE.ObjectList[0].TexFaceList[iFace].i[iTri];
			vertexTemp.tex = D3DXVECTOR2(m_ASE.ObjectList[0].TexVertexList[iTriIndex].x,
				1.0f - m_ASE.ObjectList[0].TexVertexList[iTriIndex].y);
			pModel->m_VertexList[iTexID].push_back(vertexTemp);
		}
	}

	// 텍스쳐 종류별(iLoop => MTLID)로 해당종류의 갯수(iLoopSub)만큼 반복하며 인덱스번호 카운트
	pModel->m_IndexList.resize(iNumMaterial);
	for (iLoop = 0; iLoop < iNumMaterial; iLoop++)
	{
		for (iLoopSub = 0; iLoopSub < pModel->m_VertexList[iLoop].size(); iLoopSub++)
		{
			pModel->m_IndexList[iLoop].push_back(iLoopSub);
		}
	}

	return true;
}

/*
ship.ase

*MESH_NUM     VERTEX 2656
*MESH_NUM     FACES  4158
*MESH_NUM Tex VERTEX 3803
*MESH_NUM Tex VFACES 4158
*MESH_NUM Col VERTEX 2656
*MESH_NUM Col VFACES 4158
*MESH_  FACE  NORMAL 4158
*MESH_ VERTEX NORMAL 4158*3
*/
