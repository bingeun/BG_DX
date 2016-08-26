#pragma once
#include "bgParser.h"

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
	ASE_MAP_DIFFUSE,
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

struct MaterialInfo
{
	TCHAR	szName[MAX_PATH];		// *MATERIAL_NAME
	TCHAR	szBitmap[MAX_PATH];		// *MAP_DIFFUSE >> *BITMAP
};

struct FaceInfo
{
	int		i[4];	// *MESH_FACE_LIST 에서는 >> A:, B:, C:, *MESH_MTLID 이고, *MESH_TFACELIST 에서는 i[3] 요소 사용하지 않음
};

struct GeomObject
{
	D3DXMATRIX				matWorld;		// *TM_ROW0~3
	vector<D3DXVECTOR3>		VertexList;		// *MESH_VERTEX_LIST
	vector<FaceInfo>		FaceList;		// *MESH_FACE_LIST >> i[0] = A:, i[1] = B:, i[2] = C:, i[3] = *MESH_MTLID
	vector<D3DXVECTOR3>		TexVertexList;	// *MESH_TVERTLIST
	vector<FaceInfo>		TexFaceList;	// *MESH_TFACELIST >> i[3] 요소 사용하지 않음
	vector<D3DXVECTOR4>		ColVertexList;	// 
	vector<D3DXVECTOR3>		NormVertexList;	// *MESH_NORMALS >> *MESH_VERTEXNORMAL
	vector<D3DXVECTOR3>		NormFaceList;	// *MESH_NORMALS >> *MESH_FACENORMAL
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
};
