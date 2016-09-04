#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgTextureMgr.h"
#include "bgParser.h"
#include "bgModel.h"

enum ASE_NODE_TYPE
{
	ASE_NODE_TYPE_GEOMOBJECT = 0,
	ASE_NODE_TYPE_HELPEROBJECT,
};

struct FaceInfo
{
	union
	{
		struct
		{
			int iA;
			int iB;
			int iC;
			int iID;
		};
		int i[4];
	};
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
	TCHAR	szMaterialName[MAX_PATH];			// *MATERIAL_NAME
	TCHAR	szBitmap[MAX_PATH];					// *BITMAP
};

struct MaterialInfo
{
	TCHAR	szMaterialName[MAX_PATH];			// *MATERIAL_NAME
	TCHAR	szBitmap[MAX_PATH];					// *BITMAP
	int		iMapSubno;							// *MAP_SUBNO
	vector<SubMaterialInfo>	SubMaterialList;	// *SUBMATERIAL
};

struct NodeTM
{
	TCHAR			szNodeName[MAX_PATH];		// *NODE_NAME
	D3DXMATRIX		matWorld;					// *TM_ROW0~3
	D3DXVECTOR3		vPos;						// *TM_POS
	D3DXVECTOR3		vRotAxis;					// *TM_ROTAXIS
	float			fRotAngle;					// *TM_ROTANGLE
	D3DXVECTOR3		vScale;						// *TM_SCALE
	D3DXVECTOR3		vScaleAxis;					// *TM_SCALEAXIS
	float			fScaleAxisAngle;			// *TM_SCALEAXISANG
};

struct AnimTrackInfo
{
	int				iTick;
	D3DXVECTOR3		vVector;
	D3DXVECTOR4		vRotAxis;
	AnimTrackInfo*	pPrev;
	AnimTrackInfo*	pNext;

public:
	AnimTrackInfo()
	{
		iTick = 0;
		vVector = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		vRotAxis = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		pPrev = pNext = NULL;
	}
};

struct TMAnimation
{
	vector<AnimTrackInfo>	PosTrack;			// *CONTROL_POS_TRACK
	vector<AnimTrackInfo>	RotTrack;			// *CONTROL_ROT_TRACK
	vector<AnimTrackInfo>	ScaleTrack;			// *CONTROL_SCALE_TRACK
};

struct GeomObject
{
	vector<D3DXVECTOR3>		VertexList;			// *MESH_VERTEX_LIST
	vector<FaceInfo>		FaceList;			// *MESH_FACE_LIST >> A: B: C: MTLID:
	vector<D3DXVECTOR3>		TexVertexList;		// *MESH_TVERTLIST
	vector<FaceInfo>		TexFaceList;		// *MESH_TFACELIST >> iID = i[3] 요소 사용안함
	vector<D3DXVECTOR3>		ColVertexList;		// *MESH_CVERTLIST
	vector<FaceInfo>		ColFaceList;		// *MESH_CFACELIST >> iID = i[3] 요소 사용안함
	vector<D3DXVECTOR3>		NorVertexList;		// *MESH_VERTEXNORMAL
	vector<D3DXVECTOR3>		NorFaceList;		// *MESH_FACENORMAL
	int						iMaterialRef;		// *MATERIAL_REF
};

struct ASENode
{
	ASE_NODE_TYPE	eNodeType;					// Node 종류
	TCHAR			szNodeName[MAX_PATH];		// *NODE_NAME
	TCHAR			szNodeParent[MAX_PATH];		// *NODE_PARENT
	ASENode*		pNodeParent;				// 부모 Node 에 대한 포인터

	NodeTM			nodeTM;						// 월드 행렬 정보
	void*			vpObj;						// Node 종류에 해당하는 데이터 포인터
	TMAnimation		Anim;						// 애니메이션 정보

public:
	virtual ~ASENode()
	{
		SAFE_DEL(vpObj);
	}
};

struct ASEObject
{
	SceneInfo				Scene;				// *SCENE
	vector<MaterialInfo>	MaterialList;		// *MATERIAL_LIST
	vector<ASENode>			ObjectList;			// *XXXXXOBJECT
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

	bool	Read();				// 부모객체의 필수 재정의 함수 virtual Read() = 0;
	bool	ReadScene();
	bool	ReadMaterial();
	bool	ReadGeomObject();
	bool	ReadHelperObject();
	bool	ReadShapeObject();
	bool	ReadCameraObject();
	bool	ReadLightObject();

	bool	ReadTMAnimation(int iNumGeom);

	bool	ConvertToModel(bgModel* pModel);
};
