#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgTextureMgr.h"
#include "bgParser.h"
#include "bgModel.h"

enum ASE_SECTIONS
{
	ASE_END_OF_SECTION = 0,
	ASE_SCENE,
	ASE_SCENE_FIRSTFRAME,
	ASE_SCENE_LASTFRAME,
	ASE_SCENE_FRAMESPEED,
	ASE_SCENE_TICKSPERFRAME,
	ASE_MATERIAL_LIST,
	ASE_MATERIAL_COUNT,
	ASE_MATERIAL,
	ASE_MATERIAL_NAME,
	ASE_NUMSUBMTLS,
	ASE_SUBMATERIAL,
	ASE_MAP_DIFFUSE,
	ASE_MAP_SUBNO,
	ASE_BITMAP,
	ASE_GEOMOBJECT,
	ASE_NODE_TM,
	ASE_TM_ROW0,
	ASE_TM_ROW1,
	ASE_TM_ROW2,
	ASE_TM_ROW3,
	ASE_MESH,
	ASE_MESH_NUMVERTEX,
	ASE_MESH_NUMFACES,
	ASE_MESH_VERTEX_LIST,
	ASE_MESH_VERTEX,
	ASE_MESH_FACE_LIST,
	ASE_MESH_FACE,
	ASE_MESH_NUMTVERTEX,
	ASE_MESH_TVERTLIST,
	ASE_MESH_TVERT,
	ASE_MESH_NUMTVFACES,
	ASE_MESH_TFACELIST,
	ASE_MESH_TFACE,
	ASE_MESH_NUMCVERTEX,
	ASE_MESH_CVERTLIST,
	ASE_MESH_VERTCOL,
	ASE_MESH_NUMCVFACES,
	ASE_MESH_CFACELIST,
	ASE_MESH_CFACE,
	ASE_MESH_NORMALS,
	ASE_MESH_FACENORMAL,
	ASE_MESH_VERTEXNORMAL,
	ASE_MATERIAL_REF,
};

struct SceneInfo
{
	int iFirstFrame;		// *SCENE_FIRSTFRAME
	int iLastFrame;			// *SCENE_LASTFRAME
	int iFrameSpeed;		// *SCENE_FRAMESPEED
	int iTicksPerFrame;		// *SCENE_TICKSPERFRAME
};

struct SubMaterialInfo
{
	int		iMapSubno;				// *MAP_SUBNO
	TCHAR	szBitmap[MAX_PATH];		// *BITMAP
};

struct MaterialInfo
{
	TCHAR	szMaterialName[MAX_PATH];			// *MATERIAL_NAME
	vector<SubMaterialInfo>	SubMaterialList;	// *SUBMATERIAL
};

struct FaceInfo
{
	int		i[4];	// *MESH_FACE_LIST 에서만 [3]요소 사용(*MESH_MTLID)
};

struct GeomObject
{
	D3DXMATRIX				matWorld;		// *TM_ROW0~3
	vector<D3DXVECTOR3>		VertexList;		// *MESH_VERTEX_LIST
	vector<FaceInfo>		FaceList;		// *MESH_FACE_LIST >> i[0] = A:, i[1] = B:, i[2] = C:, i[3] = *MESH_MTLID
	vector<D3DXVECTOR3>		TexVertexList;	// *MESH_TVERTLIST
	vector<FaceInfo>		TexFaceList;	// *MESH_TFACELIST >> i[3] 요소 사용하지 않음
	vector<D3DXVECTOR3>		ColVertexList;	// *MESH_CVERTLIST
	vector<FaceInfo>		ColFaceList;	// *MESH_CFACELIST >> i[3] 요소 사용하지 않음
	vector<D3DXVECTOR3>		NorVertexList;	// *MESH_VERTEXNORMAL
	vector<D3DXVECTOR3>		NorFaceList;	// *MESH_FACENORMAL
	int						iMaterialRef;	// *MATERIAL_REF
};

struct ASEObject
{
	SceneInfo				Scene;
	vector<MaterialInfo>	MaterialList;
	vector<GeomObject>		ObjectList;
};

//////////////////////////////////////////////////////
class bgParserASE : public bgParser
{
public:
	ASEObject	m_ASE;

public:
	bgParserASE();
	virtual ~bgParserASE();

public:
	bool	Init();
	bool	Read();		// virtual bool bgParser::Read() = 0;
	bool	ConvertToModel(bgModel* pModel);
};
