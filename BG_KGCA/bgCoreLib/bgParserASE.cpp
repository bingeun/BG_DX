#include "bgParserASE.h"

TCHAR*	szWord[] =
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
	_T("*MAP_DIFFUSE"),
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
	int iLoop, iLoopMax, iData;
	FaceInfo i4Data;
	D3DXVECTOR3 v3Data;

	///// SCENE - 씬 데이터
	if (!FindWord(szWord[ASE_SCENE])) return false;					//	*SCENE {
	if (!FindWord(szWord[ASE_SCENE_FIRSTFRAME])) return false;		//		*SCENE_FIRSTFRAME
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &m_ASE.Scene.iFirstFrame);
	if (!FindWord(szWord[ASE_SCENE_LASTFRAME])) return false;		//		*SCENE_LASTFRAME
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &m_ASE.Scene.iLastFrame);
	if (!FindWord(szWord[ASE_SCENE_FRAMESPEED])) return false;		//		*SCENE_FRAMESPEED
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &m_ASE.Scene.iFrameSpeed);
	if (!FindWord(szWord[ASE_SCENE_TICKSPERFRAME])) return false;	//		*SCENE_TICKSPERFRAME
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &m_ASE.Scene.iTicksPerFrame);
	if (!FindWord(szWord[ASE_END_OF_SECTION])) return false;			//	}

	///// MATERIAL_LIST - 메터리얼 리스트
	if (!FindWord(szWord[ASE_MATERIAL_LIST])) return false;			//	*MATERIAL_LIST {
	if (!FindWord(szWord[ASE_MATERIAL_COUNT])) return false;			//		*MATERIAL_COUNT
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_ASE.MaterialList.resize(iData);

	iLoopMax = m_ASE.MaterialList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		if (!FindWord(szWord[ASE_MATERIAL])) return false;			//		*MATERIAL {
		if (!FindWord(szWord[ASE_MATERIAL_NAME])) return false;		//			*MATERIAL_NAME
		ZeroMemory(m_ASE.MaterialList[iLoop].szName, MAX_PATH);
		_stscanf(m_szLine, _T("%s%s"), m_szWord, m_ASE.MaterialList[iLoop].szName);

		if (!FindWord(szWord[ASE_MAP_DIFFUSE])) return false;		//			*MAP_DIFFUSE {
		if (!FindWord(szWord[ASE_BITMAP])) return false;				//				*BITMAP
		ZeroMemory(m_ASE.MaterialList[iLoop].szBitmap, MAX_PATH);
		_stscanf(m_szLine, _T("%s%s"), m_szWord, m_ASE.MaterialList[iLoop].szBitmap);

		if (!FindWord(szWord[ASE_END_OF_SECTION])) return false;		//			}
		if (!FindWord(szWord[ASE_END_OF_SECTION])) return false;		//		}
	}
	if (!FindWord(szWord[ASE_END_OF_SECTION])) return false;			//	}

	///// GEOMOBJECT - 지오메트리 오브젝트
	m_ASE.ObjectList.resize(1);
	if (!FindWord(szWord[ASE_GEOMOBJECT])) return false;				//	*GEOMOBJECT {
	if (!FindWord(szWord[ASE_NODE_TM])) return false;				//		*NODE_TM {
	if (!FindWord(szWord[ASE_TM_ROW0])) return false;				//			*TM_ROW0
	_stscanf(m_szLine, _T("%s%f%f%f"), m_szWord, &v3Data.x, &v3Data.y, &v3Data.z);
	m_ASE.ObjectList[0].matWorld._11 = v3Data.x;
	m_ASE.ObjectList[0].matWorld._12 = v3Data.y;
	m_ASE.ObjectList[0].matWorld._13 = v3Data.z;
	m_ASE.ObjectList[0].matWorld._14 = 0.0f;

	if (!FindWord(szWord[ASE_TM_ROW1])) return false;				//			*TM_ROW1
	_stscanf(m_szLine, _T("%s%f%f%f"), m_szWord, &v3Data.x, &v3Data.y, &v3Data.z);
	m_ASE.ObjectList[0].matWorld._21 = v3Data.x;
	m_ASE.ObjectList[0].matWorld._22 = v3Data.y;
	m_ASE.ObjectList[0].matWorld._23 = v3Data.z;
	m_ASE.ObjectList[0].matWorld._24 = 0.0f;

	if (!FindWord(szWord[ASE_TM_ROW2])) return false;				//			*TM_ROW2
	_stscanf(m_szLine, _T("%s%f%f%f"), m_szWord, &v3Data.x, &v3Data.y, &v3Data.z);
	m_ASE.ObjectList[0].matWorld._31 = v3Data.x;
	m_ASE.ObjectList[0].matWorld._32 = v3Data.y;
	m_ASE.ObjectList[0].matWorld._33 = v3Data.z;
	m_ASE.ObjectList[0].matWorld._34 = 0.0f;

	if (!FindWord(szWord[ASE_TM_ROW3])) return false;				//			*TM_ROW3
	_stscanf(m_szLine, _T("%s%f%f%f"), m_szWord, &v3Data.x, &v3Data.y, &v3Data.z);
	m_ASE.ObjectList[0].matWorld._41 = v3Data.x;
	m_ASE.ObjectList[0].matWorld._42 = v3Data.y;
	m_ASE.ObjectList[0].matWorld._43 = v3Data.z;
	m_ASE.ObjectList[0].matWorld._44 = 1.0f;

	if (!FindWord(szWord[ASE_END_OF_SECTION])) return false;			//		}

	///// MESH - 메쉬 데이터
	if (!FindWord(szWord[ASE_MESH])) return false;					//		*MESH {
	if (!FindWord(szWord[ASE_MESH_NUMVERTEX])) return false;			//			*MESH_NUMVERTEX
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_ASE.ObjectList[0].VertexList.resize(iData);

	if (!FindWord(szWord[ASE_MESH_NUMFACES])) return false;			//			*MESH_NUMFACES
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_ASE.ObjectList[0].FaceList.resize(iData);

	///// VERTEX_LIST - 버텍스 리스트
	if (!FindWord(szWord[ASE_MESH_VERTEX_LIST])) return false;		//			*MESH_VERTEX_LIST {
	iLoopMax = m_ASE.ObjectList[0].VertexList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		if (!FindWord(szWord[ASE_MESH_VERTEX])) return false;		//				*MESH_VERTEX
		_stscanf(m_szLine, _T("%s%d%f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.y, &v3Data.z);
		m_ASE.ObjectList[0].VertexList[iLoop].x = v3Data.x;
		m_ASE.ObjectList[0].VertexList[iLoop].y = v3Data.y;
		m_ASE.ObjectList[0].VertexList[iLoop].z = v3Data.z;
	}
	if (!FindWord(szWord[ASE_END_OF_SECTION])) return false;			//			}

	///// FACE_LIST - 페이스 리스트
	if (!FindWord(szWord[ASE_MESH_FACE_LIST])) return false;			//			*MESH_FACE_LIST {
	iLoopMax = m_ASE.ObjectList[0].FaceList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		if (!FindWord(szWord[ASE_MESH_FACE])) return false;			//				*MESH_FACE
		_stscanf(m_szLine, _T("%s%s %s%d %s%d %s%d %s%d %s%d %s%d %s%d %s%d"),
			m_szWord, m_szWord,			// *MESH_FACE  0:
			m_szWord, &i4Data.i[0],
			m_szWord, &i4Data.i[1],
			m_szWord, &i4Data.i[2],		// A:  B:  C:
			m_szWord, &iData,
			m_szWord, &iData,
			m_szWord, &iData,			// AB: BC: CA:
			m_szWord, &iData,			// *MESH_SMOOTHING
			m_szWord, &i4Data.i[3]);	// *MESH_MTLID
		m_ASE.ObjectList[0].FaceList[iLoop] = i4Data;
	}
	if (!FindWord(szWord[ASE_END_OF_SECTION])) return false;			//			}

	if (!FindWord(szWord[ASE_MESH_NUMTVERTEX])) return false;		//			*MESH_NUMTVERTEX
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_ASE.ObjectList[0].TexVertexList.resize(iData);

	///// TVERTLIST - 텍스쳐 버텍스 리스트
	if (!FindWord(szWord[ASE_MESH_TVERTLIST])) return false;			//			*MESH_TVERTLIST {
	iLoopMax = m_ASE.ObjectList[0].TexVertexList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		if (!FindWord(szWord[ASE_MESH_TVERT])) return false;			//				*MESH_TVERT
		_stscanf(m_szLine, _T("%s%d%f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.y, &v3Data.z);
		m_ASE.ObjectList[0].TexVertexList[iLoop].x = v3Data.x;
		m_ASE.ObjectList[0].TexVertexList[iLoop].y = v3Data.y;
		m_ASE.ObjectList[0].TexVertexList[iLoop].z = v3Data.z;
	}
	if (!FindWord(szWord[ASE_END_OF_SECTION])) return false;			//			}

	if (!FindWord(szWord[ASE_MESH_NUMTVFACES])) return false;		//			*MESH_NUMTVFACES
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_ASE.ObjectList[0].TexFaceList.resize(iData);

	///// TFACELIST - 텍스쳐 페이스 리스트
	if (!FindWord(szWord[ASE_MESH_TFACELIST])) return false;			//			*MESH_TFACELIST {
	iLoopMax = m_ASE.ObjectList[0].TexFaceList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		if (!FindWord(szWord[ASE_MESH_TFACE])) return false;			//				*MESH_TFACE
		_stscanf(m_szLine, _T("%s%d%d%d%d"), m_szWord, &iData, &i4Data.i[0], &i4Data.i[1], &i4Data.i[2]);
		i4Data.i[3] = 0;
		m_ASE.ObjectList[0].TexFaceList[iLoop] = i4Data;
	}
	if (!FindWord(szWord[ASE_END_OF_SECTION])) return false;			//			}

	if (!FindWord(szWord[ASE_MESH_NUMCVERTEX])) return false;		//			*MESH_NUMCVERTEX
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_ASE.ObjectList[0].ColVertexList.resize(iData);

	///// NORMALS - 노말 버텍스&페이스 리스트
	if (!FindWord(szWord[ASE_MESH_NORMALS])) return false;			//			*MESH_NORMALS {
	iLoopMax = m_ASE.ObjectList[0].FaceList.size();
	m_ASE.ObjectList[0].NormFaceList.resize(iLoopMax);
	m_ASE.ObjectList[0].NormVertexList.resize(iLoopMax * 3);
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		if (!FindWord(szWord[ASE_MESH_FACENORMAL])) return false;	//				*MESH_FACENORMAL
		_stscanf(m_szLine, _T("%s%d%f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.y, &v3Data.z);
		m_ASE.ObjectList[0].NormFaceList[iLoop] = v3Data;

		if (!FindWord(szWord[ASE_MESH_VERTEXNORMAL])) return false;	//					*MESH_VERTEXNORMAL
		_stscanf(m_szLine, _T("%s%d%f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.y, &v3Data.z);
		m_ASE.ObjectList[0].NormVertexList[iLoop * 3 + 0] = v3Data;

		if (!FindWord(szWord[ASE_MESH_VERTEXNORMAL])) return false;	//					*MESH_VERTEXNORMAL
		_stscanf(m_szLine, _T("%s%d%f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.y, &v3Data.z);
		m_ASE.ObjectList[0].NormVertexList[iLoop * 3 + 1] = v3Data;

		if (!FindWord(szWord[ASE_MESH_VERTEXNORMAL])) return false;	//					*MESH_VERTEXNORMAL
		_stscanf(m_szLine, _T("%s%d%f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.y, &v3Data.z);
		m_ASE.ObjectList[0].NormVertexList[iLoop * 3 + 2] = v3Data;
	}
	if (!FindWord(szWord[ASE_END_OF_SECTION])) return false;			//			}
	if (!FindWord(szWord[ASE_END_OF_SECTION])) return false;			//		}

	if (!FindWord(szWord[ASE_MATERIAL_REF])) return false;			//		*MATERIAL_REF
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &m_ASE.ObjectList[0].iMaterialRef);

	if (!FindWord(szWord[ASE_END_OF_SECTION])) return false;			//	}

	return true;
}
