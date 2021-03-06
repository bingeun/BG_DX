#pragma once
#include "bgShape.h"
#include "bgTextureMgr.h"
#include "bgSys.h"

typedef basic_string<TCHAR> T_STR;

enum OBJECT_NODE_TYPE
{
	OBJECT_NODE_TYPE_GEOMOBJECT = 0,
	OBJECT_NODE_TYPE_BONE,
	OBJECT_NODE_TYPE_DUMMY,
	OBJECT_NODE_TYPE_BIPED,
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
	int iVersion;		// 버전
	int iFirstFrame;		// *SCENE_FIRSTFRAME
	int iLastFrame;			// *SCENE_LASTFRAME
	int iFrameSpeed;		// *SCENE_FRAMESPEED
	int iTicksPerFrame;		// *SCENE_TICKSPERFRAME
	int iNumMesh;		// 메쉬오브젝트 개수
	int iMaxWeight;		// 정점 당 가중치
	int iBindPose;		// 바인딩 포즈 에니메이션 여부
};

struct VersionMark
{
	int		iVersion;
	TCHAR	description[128];
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

public:
	NodeTM()
	{
		Init();
	}
	void Init()
	{
		szNodeName[0] = _T('\0');
		D3DXMatrixIdentity(&matWorld);
		vPos = vRotAxis = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		vScale = vScaleAxis = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		fRotAngle = fScaleAxisAngle = 0.0f;
	}
};

struct AnimTrackInfo
{
	int				iTick;
	D3DXVECTOR3		vVector;
	D3DXQUATERNION	qRotate;

public:
	AnimTrackInfo()
	{
		Init();
	}
	void Init()
	{
		iTick = 0;
		vVector = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		qRotate = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 0.0f);
	}
};

struct TMAnimation
{
	vector<AnimTrackInfo>	PosTrack;			// *CONTROL_POS_TRACK
	vector<AnimTrackInfo>	RotTrack;			// *CONTROL_ROT_TRACK
	vector<AnimTrackInfo>	SclTrack;			// *CONTROL_SCALE_TRACK
	vector<AnimTrackInfo>	AlpTrack;			// *VISIBLE_TRACK
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
	D3DXVECTOR3				vBBoxMin;			// *BOUNDINGBOX_MIN
	D3DXVECTOR3				vBBoxMax;			// *BOUNDINGBOX_MAX
};

struct HelperObject
{
	D3DXVECTOR3			vBBoxMin;				// *BOUNDINGBOX_MIN
	D3DXVECTOR3			vBBoxMax;				// *BOUNDINGBOX_MAX
};

struct ObjectNode
{
	//OBJECT_NODE_TYPE	eNodeType;				// Node 종류
	int					eNodeType;				// Node 종류
	TCHAR				szNodeName[MAX_PATH];	// *NODE_NAME
	TCHAR				szNodeParent[MAX_PATH];	// *NODE_PARENT
	ObjectNode*			pNodeParent;			// 부모 Node 에 대한 포인터
	vector<ObjectNode*>	pNodeChildList;			// 자식 Node들에 대한 포인터

	NodeTM				nodeTM;					// 월드 행렬 정보
	void*				vpObj;					// Node 종류에 해당하는 데이터 포인터
	TMAnimation			Anim;					// 애니메이션 정보
	bool				bAnim;					// 애니메이션 존재 여부

	D3DXMATRIX			matWorldPos;			// 월드 이동행렬
	D3DXMATRIX			matWorldRot;			// 월드 회전행렬
	D3DXMATRIX			matWorldScl;			// 월드 신축행렬
	D3DXMATRIX			matCalculation;			// 행렬연산 결과

	vector<ID3D11Buffer*>		m_pVBList;		// 버텍스 버퍼
	vector<ID3D11Buffer*>		m_pIBList;		// 인덱스 버퍼

	vector<vector<VertexPNCT>>	m_VertexList;	// 메터리얼별 버텍스 리스트
	vector<vector<UINT>>		m_IndexList;	// 메터리얼별 인덱스 리스트

public:
	virtual ~ObjectNode()
	{
		//SAFE_DEL(vpObj);
	}
};

class bgModelBG3D : public bgShape
{
public:
	SceneInfo				m_Scene;			// *SCENE
	vector<MaterialInfo>	m_MaterialList;		// *MATERIAL_LIST
	vector<ObjectNode>		m_ObjectList;		// *XxxOBJECT

	vector<MaterialTexID>	m_TexIDList;
	MATRIX_BUFFER			m_MatrixBuffer;
	float					m_fCurrentTick;

public:
	bgModelBG3D();
	virtual ~bgModelBG3D();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	HRESULT	CreateBuffer();
	HRESULT	LoadShader(CHAR* szVS = "VS", CHAR* szPS = "PS");
	void	SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);

	void	Interpolate(ObjectNode* pNodeObject, float fFrameTick, D3DXMATRIX* matWorld);
	int		GetTrackIndex(float fTick, vector<AnimTrackInfo>* pTrackList);
};
