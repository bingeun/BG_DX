#include "bgParserASE.h"

bgParserASE::bgParserASE()
{
}

bgParserASE::~bgParserASE()
{
}

bool bgParserASE::Init(bgModel* pModel)
{
	m_pModel = pModel;
	return true;
}

bool bgParserASE::Read()
{
	bool hr = true;

	TCHAR szWordArray[2][MAX_PATH];

	// m_ASE 데이터 초기화
	ZeroMemory(&m_pModel->m_Scene, sizeof(SceneInfo));
	m_pModel->m_MaterialList.clear();
	m_pModel->m_ObjectList.clear();

	// 파일 앞부분 1회만 등장하는 섹션 읽기
	IF_FALSE_RETURN(FindWord(_T("*SCENE")));
	IF_FALSE_RETURN(ReadScene());
	IF_FALSE_RETURN(FindWord(_T("*MATERIAL_LIST")));
	IF_FALSE_RETURN(ReadMaterial());

	// 여러번 등장하는 섹션 반복해서 읽기
	_tcscpy(szWordArray[0], _T("*GEOMOBJECT"));
	_tcscpy(szWordArray[1], _T("*HELPEROBJECT"));
	while (!feof(m_pFile))
	{
		switch (FindWordArray(szWordArray, 2))
		{
		case 0:	IF_FALSE_RETURN(ReadGeomObject());		break;
		case 1:	IF_FALSE_RETURN(ReadHelperObject());	break;
		case -1:	// 찾는 단어 없음 (파일의 끝)
		{
			// 읽은 데이터를 모델용 데이터로 컨버팅
			IF_FALSE_RETURN(ConvertToModel());
		}
		break;
		default:	// 나머지 (배열 요소가 2개이므로 나올 수 없음)
			return false;
			break;
		}
	}

	return hr;
}

bool bgParserASE::ReadScene()
{
	bool hr = true;

	IF_FALSE_RETURN(FindWord(_T("*SCENE_FIRSTFRAME")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &m_pModel->m_Scene.iFirstFrame);
	IF_FALSE_RETURN(FindWord(_T("*SCENE_LASTFRAME")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &m_pModel->m_Scene.iLastFrame);
	IF_FALSE_RETURN(FindWord(_T("*SCENE_FRAMESPEED")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &m_pModel->m_Scene.iFrameSpeed);
	IF_FALSE_RETURN(FindWord(_T("*SCENE_TICKSPERFRAME")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &m_pModel->m_Scene.iTicksPerFrame);

	IF_FALSE_RETURN(FindWord(_T("}"))); // SCENE 탈출

	return hr;
}

bool bgParserASE::ReadMaterial()
{
	bool hr = true;

	TCHAR szWordArray[2][MAX_PATH];
	TCHAR* szToken;
	int iData;
	int iLoop, iLoopMax;
	int iLoopSub, iLoopSubMax;

	// 최상위 메터리얼 갯수
	IF_FALSE_RETURN(FindWord(_T("*MATERIAL_COUNT")));
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_pModel->m_MaterialList.resize(iData);

	// 최상위 메터리얼 갯수만큼 반복하며 읽기
	iLoopMax = m_pModel->m_MaterialList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		IF_FALSE_RETURN(FindWord(_T("*MATERIAL")));

		IF_FALSE_RETURN(FindWord(_T("*MATERIAL_NAME")));
		ZeroMemory(m_pModel->m_MaterialList[iLoop].szMaterialName, MAX_PATH);
		szToken = _tcstok(m_szLine, _T("\""));
		szToken = _tcstok(NULL, _T("\""));
		_tcscpy(m_pModel->m_MaterialList[iLoop].szMaterialName, szToken);

		// 서브 메터리얼이 있는지 여부 확인
		_tcscpy(szWordArray[0], _T("*NUMSUBMTLS"));
		_tcscpy(szWordArray[1], _T("*MAP_SUBNO"));
		switch (FindWordArray(szWordArray, 2))
		{
		case 0:		// *NUMSUBMTLS		서브 메터리얼이 있다면 =======================
		{
			_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
			m_pModel->m_MaterialList[iLoop].SubMaterialList.resize(iData);

			iLoopSubMax = m_pModel->m_MaterialList[iLoop].SubMaterialList.size();
			for (iLoopSub = 0; iLoopSub < iLoopSubMax; iLoopSub++)
			{
				IF_FALSE_RETURN(FindWord(_T("*MAP_SUBNO")));

				_tcscpy(szWordArray[0], _T("*BITMAP"));
				_tcscpy(szWordArray[1], _T("}"));
				switch (FindWordArray(szWordArray, 2))
				{
				case 0:		// *BITMAP		비트맵이 있다면 ------------------------
				{
					ZeroMemory(m_pModel->m_MaterialList[iLoop].SubMaterialList[iLoopSub].szBitmap, MAX_PATH);
					szToken = _tcstok(m_szLine, _T("\""));
					szToken = _tcstok(NULL, _T("\""));
					_tcscpy(m_pModel->m_MaterialList[iLoop].SubMaterialList[iLoopSub].szBitmap, GetPathToFileName(szToken));
					
					// 하나의 *MAP_XXX { 탈출
					IF_FALSE_RETURN(FindWord(_T("}")));
				}
				break;
				case 1:		// }			비트맵이 없는 메터리얼이면 --------------
				{
					// 해당 메터리얼 작업
				}
				break;
				case -1:	// 찾는 단어 없음 -------------------------------------
				default:	// 나머지 (배열 요소가 2개이므로 나올 수 없음)------------
					return false;
					break;
				}
				// 하나의 *SUBMATERIAL { 탈출
				IF_FALSE_RETURN(FindWord(_T("}")));
			}
		}
		break;
		case 1:		// *MAP_SUBNO		단일 메터리얼이라면 ===========================
		{
			m_pModel->m_MaterialList.resize(1);

			_tcscpy(szWordArray[0], _T("*BITMAP"));
			_tcscpy(szWordArray[1], _T("}"));
			switch (FindWordArray(szWordArray, 2))
			{
			case 0:		// *BITMAP		비트맵이 있다면 ------------------------
			{
				ZeroMemory(m_pModel->m_MaterialList[iLoop].szBitmap, MAX_PATH);
				szToken = _tcstok(m_szLine, _T("\""));
				szToken = _tcstok(NULL, _T("\""));
				_tcscpy(m_pModel->m_MaterialList[iLoop].szBitmap, GetPathToFileName(szToken));

				// 하나의 *MAP_XXX { 탈출
				IF_FALSE_RETURN(FindWord(_T("}")));
			}
			break;
			case 1:		// }			비트맵이 없는 메터리얼이면 --------------
			{
				// 해당 메터리얼 작업
			}
			break;
			case -1:			// 찾는 단어 없음 ------------------------------
			default:			// 나머지 (배열 요소가 2개이므로 나올 수 없음)
				return false;
				break;
			}
			// 하나의 *MATERIAL { 탈출
			IF_FALSE_RETURN(FindWord(_T("}")));
		}
		break;
		case -1:	// 찾는 단어 없음 =================================================
		default:	// 나머지 (배열 요소가 2개이므로 나올 수 없음)
			return false;
			break;
		}
	}
	
	IF_FALSE_RETURN(FindWord(_T("}"))); // MATERIAL_LIST 탈출

	return hr;
}

bool bgParserASE::ReadGeomObject()
{
	bool hr = true;

	TCHAR szWordArray[2][MAX_PATH];
	D3DXVECTOR3 v3Data;
	FaceInfo i4Data;
	int iData;
	int iLoop, iLoopMax;

	int iNumGeom = m_pModel->m_ObjectList.size();
	m_pModel->m_ObjectList.resize(iNumGeom + 1);
	m_pModel->m_ObjectList[iNumGeom].vpObj = new GeomObject;

	// 월드행렬 정보 저장
	IF_FALSE_RETURN(FindWord(_T("*NODE_TM")));

	IF_FALSE_RETURN(FindWord(_T("*TM_ROW0")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._11,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._13,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._12);
	m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._14 = 0.0f;

	IF_FALSE_RETURN(FindWord(_T("*TM_ROW1")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._31,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._33,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._32);
	m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._34 = 0.0f;

	IF_FALSE_RETURN(FindWord(_T("*TM_ROW2")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._21,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._23,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._22);
	m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._24 = 0.0f;

	IF_FALSE_RETURN(FindWord(_T("*TM_ROW3")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._41,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._43,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._42);
	m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld._44 = 1.0f;

	// 분해된 월드행렬 정보 저장
	IF_FALSE_RETURN(FindWord(_T("*TM_POS")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.vPos.x,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.vPos.z,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.vPos.y);

	IF_FALSE_RETURN(FindWord(_T("*TM_ROTAXIS")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.vRotAxis.x,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.vRotAxis.z,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.vRotAxis.y);

	IF_FALSE_RETURN(FindWord(_T("*TM_ROTANGLE")));
	_stscanf(m_szLine, _T("%s %f"), m_szWord, &m_pModel->m_ObjectList[iNumGeom].nodeTM.fRotAngle);

	IF_FALSE_RETURN(FindWord(_T("*TM_SCALE")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.vScale.x,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.vScale.z,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.vScale.y);

	IF_FALSE_RETURN(FindWord(_T("*TM_SCALEAXIS")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.vScaleAxis.x,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.vScaleAxis.z,
		&m_pModel->m_ObjectList[iNumGeom].nodeTM.vScaleAxis.y);

	IF_FALSE_RETURN(FindWord(_T("*TM_SCALEAXISANG")));
	_stscanf(m_szLine, _T("%s %f"), m_szWord, &m_pModel->m_ObjectList[iNumGeom].nodeTM.fScaleAxisAngle);

	IF_FALSE_RETURN(FindWord(_T("}"))); // NODE_TM 탈출

	///// MESH - 메쉬 데이터
	IF_FALSE_RETURN(FindWord(_T("*MESH")));
	IF_FALSE_RETURN(FindWord(_T("*MESH_NUMVERTEX")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &iData);
	static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->VertexList.resize(iData);

	IF_FALSE_RETURN(FindWord(_T("*MESH_NUMFACES")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &iData);
	static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->FaceList.resize(iData);
	
	///// VERTEX_LIST - 버텍스 리스트
	D3DXMATRIX matInverse;
	D3DXMatrixInverse(&matInverse, NULL, &m_pModel->m_ObjectList[iNumGeom].nodeTM.matWorld);

	IF_FALSE_RETURN(FindWord(_T("*MESH_VERTEX_LIST")));
	iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->VertexList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		IF_FALSE_RETURN(FindWord(_T("*MESH_VERTEX")));
		_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		D3DXVec3TransformCoord(&v3Data, &v3Data, &matInverse);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->VertexList[iLoop] = v3Data;
	}
	IF_FALSE_RETURN(FindWord(_T("}")));

	///// FACE_LIST - 페이스 리스트
	IF_FALSE_RETURN(FindWord(_T("*MESH_FACE_LIST")));
	iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->FaceList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		IF_FALSE_RETURN(FindWord(_T("*MESH_FACE")));
		_stscanf(m_szLine, _T("%s %s %s%d %s%d %s%d %s%d %s%d %s%d %s%d %s%d"),
			m_szWord, m_szWord,			// *MESH_FACE  0:
			m_szWord, &i4Data.iA,		// A:
			m_szWord, &i4Data.iC,		// B:
			m_szWord, &i4Data.iB,		// C:
			m_szWord, &iData,			// AB:
			m_szWord, &iData,			// BC:
			m_szWord, &iData,			// CA:
			m_szWord, &iData,			// *MESH_SMOOTHING
			m_szWord, &i4Data.iID);		// *MESH_MTLID
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->FaceList[iLoop] = i4Data;
	}
	IF_FALSE_RETURN(FindWord(_T("}")));

	///// TVERTLIST - 텍스쳐 버텍스 리스트
	IF_FALSE_RETURN(FindWord(_T("*MESH_NUMTVERTEX")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &iData);
	static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->TexVertexList.resize(iData);

	IF_FALSE_RETURN(FindWord(_T("*MESH_TVERTLIST")));
	iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->TexVertexList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		IF_FALSE_RETURN(FindWord(_T("*MESH_TVERT")));
		_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.y, &v3Data.z);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->TexVertexList[iLoop] = v3Data;
	}
	IF_FALSE_RETURN(FindWord(_T("}")));

	///// TFACELIST - 텍스쳐 페이스 리스트
	IF_FALSE_RETURN(FindWord(_T("*MESH_NUMTVFACES")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &iData);
	static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->TexFaceList.resize(iData);

	IF_FALSE_RETURN(FindWord(_T("*MESH_TFACELIST")));
	iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->TexFaceList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		IF_FALSE_RETURN(FindWord(_T("*MESH_TFACE")));
		_stscanf(m_szLine, _T("%s %d %d%d%d"), m_szWord, &iData, &i4Data.i[0], &i4Data.i[2], &i4Data.i[1]);
		i4Data.i[3] = 0;
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->TexFaceList[iLoop] = i4Data;
	}
	IF_FALSE_RETURN(FindWord(_T("}")));

	///// CVERTLIST - 컬러 버텍스&페이스 리스트
	IF_FALSE_RETURN(FindWord(_T("*MESH_NUMCVERTEX")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &iData);
	// ASE 파일에 색상 정보가 없다면 임의색상 추가
	if (iData == 0)
	{
		// 버텍스 컬러
		iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->VertexList.size();
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->ColVertexList.resize(iLoopMax);
		for (iLoop = 0; iLoop < iLoopMax; iLoop++)
		{
			static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->ColVertexList[iLoop] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		}

		// 페이스 인덱스
		iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->FaceList.size();
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->ColFaceList.resize(iLoopMax);
		for (iLoop = 0; iLoop < iLoopMax; iLoop++)
		{
			// 페이스의 인덱스 번호와 동일하게 지정
			static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->ColFaceList[iLoop] =
				static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->FaceList[iLoop];
		}
	}
	// ASE 파일에 색상 정보가 있다면 해당색상 저장
	else
	{
		// 버텍스 컬러
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->ColVertexList.resize(iData);

		IF_FALSE_RETURN(FindWord(_T("*MESH_CVERTLIST")));
		iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->ColVertexList.size();
		for (iLoop = 0; iLoop < iLoopMax; iLoop++)
		{
			IF_FALSE_RETURN(FindWord(_T("*MESH_VERTCOL")));
			_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.y, &v3Data.z);
			static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->ColVertexList[iLoop] = v3Data;
		}
		IF_FALSE_RETURN(FindWord(_T("}")));

		// 페이스 인덱스
		IF_FALSE_RETURN(FindWord(_T("*MESH_NUMCVFACES")));
		_stscanf(m_szLine, _T("%s %d"), m_szWord, &iData);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->ColFaceList.resize(iData);

		i4Data.i[3] = 0; // 쓰지않는 [3]요소 항상 0으로 지정
		IF_FALSE_RETURN(FindWord(_T("*MESH_CFACELIST")));
		iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->ColFaceList.size();
		for (iLoop = 0; iLoop < iLoopMax; iLoop++)
		{
			IF_FALSE_RETURN(FindWord(_T("*MESH_CFACE")));
			_stscanf(m_szLine, _T("%s %d %d%d%d"), m_szWord, &iData, &i4Data.i[0], &i4Data.i[2], &i4Data.i[1]);
			static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->ColFaceList[iLoop] = i4Data;
		}
		IF_FALSE_RETURN(FindWord(_T("}")));
	}

	///// NORMALS - 노말 버텍스&페이스 리스트
	IF_FALSE_RETURN(FindWord(_T("*MESH_NORMALS")));
	iData = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->FaceList.size();
	static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->NorFaceList.resize(iData);
	static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->NorVertexList.resize(iData * 3);

	iLoopMax = iData;
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		IF_FALSE_RETURN(FindWord(_T("*MESH_FACENORMAL")));
		_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->NorFaceList[iLoop] = v3Data;

		IF_FALSE_RETURN(FindWord(_T("*MESH_VERTEXNORMAL")));
		_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->NorVertexList[iLoop * 3 + 0] = v3Data;

		IF_FALSE_RETURN(FindWord(_T("*MESH_VERTEXNORMAL")));
		_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->NorVertexList[iLoop * 3 + 2] = v3Data;

		IF_FALSE_RETURN(FindWord(_T("*MESH_VERTEXNORMAL")));
		_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->NorVertexList[iLoop * 3 + 1] = v3Data;
	}
	IF_FALSE_RETURN(FindWord(_T("}"))); // MESH_NORMALS 탈출

	IF_FALSE_RETURN(FindWord(_T("}"))); // MESH 탈출

	///// TM_ANIMATION - 애니메이션
	_tcscpy(szWordArray[0], _T("*TM_ANIMATION"));
	_tcscpy(szWordArray[1], _T("*MATERIAL_REF"));
	switch (FindWordArray(szWordArray, 2))
	{
	case 0:		// *TM_ANIMATION	애니메이션이 있다면 ====================================
	{
		IF_FALSE_RETURN(ReadTMAnimation(iNumGeom));

		IF_FALSE_RETURN(FindWord(_T("*MATERIAL_REF")));
		_stscanf(m_szLine, _T("%s %d"), m_szWord, &static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->iMaterialRef);
	}
	break;
	case 1:		// *MATERIAL_REF	애니메이션 없이 곧바로 매터리얼 REF이면 =================
	{
		_stscanf(m_szLine, _T("%s %d"), m_szWord, &static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumGeom].vpObj)->iMaterialRef);
	}
	break;
	case -1:	// 찾는 단어 없음 =========================================================
	default:	// 나머지 (배열 요소가 2개이므로 나올 수 없음)
		return false;
		break;
	}

	IF_FALSE_RETURN(FindWord(_T("}"))); // GEOMOBJECT 탈출

	return hr;
}

bool bgParserASE::ReadHelperObject()
{
	bool hr = true;
	return hr;
}

bool bgParserASE::ReadShapeObject()
{
	bool hr = true;
	return hr;
}

bool bgParserASE::ReadCameraObject()
{
	bool hr = true;
	return hr;
}

bool bgParserASE::ReadLightObject()
{
	bool hr = true;
	return hr;
}

bool bgParserASE::ReadTMAnimation(int iNumGeom)
{
	bool hr = true;

	TCHAR szWordArray[2][MAX_PATH];
	TCHAR szWordTrack[4][MAX_PATH];
	AnimTrackInfo animData;
	bool bAnimEnd, bTrackEnd;

	m_pModel->m_ObjectList[iNumGeom].Anim.PosTrack.clear();
	m_pModel->m_ObjectList[iNumGeom].Anim.RotTrack.clear();
	m_pModel->m_ObjectList[iNumGeom].Anim.ScaleTrack.clear();

	_tcscpy(szWordTrack[0], _T("*CONTROL_POS_TRACK"));
	_tcscpy(szWordTrack[1], _T("*CONTROL_ROT_TRACK"));
	_tcscpy(szWordTrack[2], _T("*CONTROL_SCALE_TRACK"));
	_tcscpy(szWordTrack[3], _T("}"));

	bAnimEnd = false;
	while (bAnimEnd == false)
	{
		switch (FindWordArray(szWordTrack, 4))
		{
		case 0:		// *CONTROL_POS_TRACK ==========================================
		{
			_tcscpy(szWordArray[0], _T("*CONTROL_POS_SAMPLE"));
			_tcscpy(szWordArray[1], _T("}"));
			animData.Init();
			bTrackEnd = false;
			while (bTrackEnd == false)
			{
				switch (FindWordArray(szWordArray, 2))
				{
				case 0:		// *CONTROL_POS_SAMPLE	----------------------------
				{
					// 트랙 추가
					_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &animData.iTick,
						&animData.vVector.x, &animData.vVector.z, &animData.vVector.y);
					m_pModel->m_ObjectList[iNumGeom].Anim.PosTrack.push_back(animData);
					continue;
				}
				break;
				case 1:		// }					----------------------------
				{
					// 트랙 종료 처리
					bTrackEnd = true;
				}
				break;
				case -1:	// 찾는 단어 없음 -----------------------------------
				default:	// 나머지 (배열 요소가 2개이므로 나올 수 없음)
					return false;
					break;
				}
			}
		}
		break;
		case 1:		// *CONTROL_ROT_TRACK ==========================================
		{
			_tcscpy(szWordArray[0], _T("*CONTROL_ROT_SAMPLE"));
			_tcscpy(szWordArray[1], _T("}"));
			animData.Init();
			bTrackEnd = false;
			while (bTrackEnd == false)
			{
				switch (FindWordArray(szWordArray, 2))
				{
				case 0:		// *CONTROL_ROT_SAMPLE	----------------------------
				{
					// 트랙 추가
					_stscanf(m_szLine, _T("%s %d %f%f%f %f"), m_szWord, &animData.iTick,
						&animData.vRotAxis.x, &animData.vRotAxis.z, &animData.vRotAxis.y, &animData.vRotAxis.w);
					m_pModel->m_ObjectList[iNumGeom].Anim.RotTrack.push_back(animData);
					continue;
				}
				break;
				case 1:		// }					----------------------------
				{
					// 트랙 종료 처리
					bTrackEnd = true;
				}
				break;
				case -1:	// 찾는 단어 없음 -----------------------------------
				default:	// 나머지 (배열 요소가 2개이므로 나올 수 없음)
					return false;
					break;
				}
			}
		}
		break;
		case 2:		// *CONTROL_SCALE_TRACK ========================================
		{
			_tcscpy(szWordArray[0], _T("*CONTROL_SCALE_SAMPLE"));
			_tcscpy(szWordArray[1], _T("}"));
			animData.Init();
			bTrackEnd = false;
			while (bTrackEnd == false)
			{
				switch (FindWordArray(szWordArray, 2))
				{
				case 0:		// *CONTROL_SCALE_SAMPLE	------------------------
				{
					// 트랙 추가
					_stscanf(m_szLine, _T("%s %d %f%f%f %f%f%f%f"), m_szWord, &animData.iTick,
						&animData.vVector.x, &animData.vVector.z, &animData.vVector.y,
						&animData.vRotAxis.x, &animData.vRotAxis.z, &animData.vRotAxis.y, &animData.vRotAxis.w);
					m_pModel->m_ObjectList[iNumGeom].Anim.ScaleTrack.push_back(animData);
					continue;
				}
				break;
				case 1:		// }					----------------------------
				{
					// 트랙 종료 처리
					bTrackEnd = true;
				}
				break;
				case -1:	// 찾는 단어 없음 -----------------------------------
				default:	// 나머지 (배열 요소가 2개이므로 나올 수 없음)
					return false;
					break;
				}
			}
		}
		break;
		case 3:		// }	========================================================
		{
			// 애니메이션 종료 처리
			bAnimEnd = true;
		}
		break;
		case -1:	// 찾는 단어 없음 ===============================================
		default:	// 나머지 (배열 요소가 4개이므로 나올 수 없음)
			return false;
			break;
		}
	}

	return hr;
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

// ASE 파일에서 읽은 데이터를 모델(출력용) 데이터로 변환
bool bgParserASE::ConvertToModel()
{
	int iLoop, iLoopSub;
	int iFace, iTri;
	int iNumMaterial;
	int iNumSubMaterial;

	// 메터리얼 텍스쳐 로드
	m_pModel->m_TexIDList.resize(m_pModel->m_MaterialList.size());
	iNumMaterial = 0;
	for (iLoop = 0; iLoop < m_pModel->m_MaterialList.size(); iLoop++)
	{
		// 서브 메터리얼이 있으면
		if (m_pModel->m_MaterialList[iLoop].SubMaterialList.size() > 1)
		{
			iNumSubMaterial = m_pModel->m_MaterialList[iLoop].SubMaterialList.size();
			m_pModel->m_TexIDList[iLoop].SubIDList.resize(iNumSubMaterial);
			for (iLoopSub = 0; iLoopSub < iNumSubMaterial; iLoopSub++)
			{
				iNumMaterial++;
				m_pModel->m_TexIDList[iLoop].SubIDList[iLoopSub].iID =
					I_TextureMgr.Add(m_pModel->m_MaterialList[iLoop].SubMaterialList[iLoopSub].szBitmap);
			}
		}
		// 서브 메터리얼이 없으면
		else
		{
			iNumMaterial++;
			m_pModel->m_TexIDList[iLoop].iID = I_TextureMgr.Add(m_pModel->m_MaterialList[iLoop].szBitmap);
		}
	}

	// 종류별로 분류하여 데이터 추가
	VertexPNCT vertexTemp;
	int iTexID, iTriIndex;
	m_pModel->m_VertexList.resize(iNumMaterial);
	for (iFace = 0; iFace < static_cast<GeomObject*>(m_pModel->m_ObjectList[0].vpObj)->FaceList.size(); iFace++)
	{
		// 서브메터리얼이 없으면 단일 ID로 사용
		if (iNumMaterial == 1)
			iTexID = m_pModel->m_TexIDList[0].iID;
		else
			iTexID = static_cast<GeomObject*>(m_pModel->m_ObjectList[0].vpObj)->FaceList[iFace].iID;

		for (iTri = 0; iTri < 3; iTri++)
		{
			iTriIndex = static_cast<GeomObject*>(m_pModel->m_ObjectList[0].vpObj)->FaceList[iFace].i[iTri];
			vertexTemp.pos = static_cast<GeomObject*>(m_pModel->m_ObjectList[0].vpObj)->VertexList[iTriIndex];
			vertexTemp.nor = static_cast<GeomObject*>(m_pModel->m_ObjectList[0].vpObj)->NorVertexList[iFace * 3 + iTri];
			iTriIndex = static_cast<GeomObject*>(m_pModel->m_ObjectList[0].vpObj)->ColFaceList[iFace].i[iTri];
			vertexTemp.col = D3DXVECTOR4(static_cast<GeomObject*>(m_pModel->m_ObjectList[0].vpObj)->ColVertexList[iTriIndex], 1.0f);
			iTriIndex = static_cast<GeomObject*>(m_pModel->m_ObjectList[0].vpObj)->TexFaceList[iFace].i[iTri];
			vertexTemp.tex = D3DXVECTOR2(static_cast<GeomObject*>(m_pModel->m_ObjectList[0].vpObj)->TexVertexList[iTriIndex].x,
				1.0f - static_cast<GeomObject*>(m_pModel->m_ObjectList[0].vpObj)->TexVertexList[iTriIndex].y);
			m_pModel->m_VertexList[iTexID].push_back(vertexTemp);
		}
	}

	// 텍스쳐 종류별(iLoop => MTLID)로 해당종류의 갯수(iLoopSub)만큼 반복하며 인덱스번호 카운트
	m_pModel->m_IndexList.resize(iNumMaterial);
	for (iLoop = 0; iLoop < iNumMaterial; iLoop++)
	{
		for (iLoopSub = 0; iLoopSub < m_pModel->m_VertexList[iLoop].size(); iLoopSub++)
		{
			m_pModel->m_IndexList[iLoop].push_back(iLoopSub);
		}
	}

	return true;
}

void ASENode::Interpolate(float fFrameTick)
{

}
