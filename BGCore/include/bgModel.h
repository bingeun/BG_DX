#pragma once
#include "bgShape.h"
#include "bgTextureMgr.h"
#include "bgSys.h"

enum OBJECT_NODE_TYPE
{
	OBJECT_NODE_TYPE_GEOMOBJECT = 0,
	OBJECT_NODE_TYPE_HELPEROBJECT,
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
};

struct GeomObject
{
	vector<D3DXVECTOR3>		VertexList;			// *MESH_VERTEX_LIST
	vector<FaceInfo>		FaceList;			// *MESH_FACE_LIST >> A: B: C: MTLID:
	vector<D3DXVECTOR3>		TexVertexList;		// *MESH_TVERTLIST
	vector<FaceInfo>		TexFaceList;		// *MESH_TFACELIST >> iID = i[3] ��� ������
	vector<D3DXVECTOR3>		ColVertexList;		// *MESH_CVERTLIST
	vector<FaceInfo>		ColFaceList;		// *MESH_CFACELIST >> iID = i[3] ��� ������
	vector<D3DXVECTOR3>		NorVertexList;		// *MESH_VERTEXNORMAL
	vector<D3DXVECTOR3>		NorFaceList;		// *MESH_FACENORMAL
	int						iMaterialRef;		// *MATERIAL_REF
};

struct HelperObject
{
	D3DXVECTOR3			vBBoxMin;				// *BOUNDINGBOX_MIN
	D3DXVECTOR3			vBBoxMax;				// *BOUNDINGBOX_MAX
};

struct ObjectNode
{
	OBJECT_NODE_TYPE	eNodeType;				// Node ����
	TCHAR				szNodeName[MAX_PATH];	// *NODE_NAME
	TCHAR				szNodeParent[MAX_PATH];	// *NODE_PARENT
	ObjectNode*			pNodeParent;			// �θ� Node �� ���� ������
	vector<ObjectNode*>	pNodeChildList;			// �ڽ� Node�鿡 ���� ������

	NodeTM				nodeTM;					// ���� ��� ����
	void*				vpObj;					// Node ������ �ش��ϴ� ������ ������
	TMAnimation			Anim;					// �ִϸ��̼� ����
	bool				bAnim;					// �ִϸ��̼� ���� ����

	D3DXMATRIX			matWorldPos;			// ���� �̵����
	D3DXMATRIX			matWorldRot;			// ���� ȸ�����
	D3DXMATRIX			matWorldScl;			// ���� �������
	D3DXMATRIX			matCalculation;			// ��Ŀ��� ���

public:
	virtual ~ObjectNode()
	{
		//SAFE_DEL(vpObj);
	}
};

class bgModel : public bgShape
{
public:
	vector<ID3D11Buffer*>		m_pVBList;			// Vertex Buffer;
	vector<ID3D11Buffer*>		m_pIBList;			// Index Buffer;

	vector<vector<VertexPNCT>>	m_VertexList;
	vector<vector<UINT>>		m_IndexList;
	vector<MaterialTexID>		m_TexIDList;

public:
	SceneInfo					m_Scene;			// *SCENE
	vector<MaterialInfo>		m_MaterialList;		// *MATERIAL_LIST
	vector<ObjectNode>			m_ObjectList;		// *XxxOBJECT

	MATRIX_BUFFER	m_MatrixBuffer;
	float			m_fCurrentTick;


public:
	bgModel();
	virtual ~bgModel();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	HRESULT	CreateBuffer();
	HRESULT	LoadShader(CHAR* szVS = "VS", CHAR* szPS = "PS");
	void	SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);

	void	Interpolate(ObjectNode* pNodeObject, float fFrameTick, D3DXMATRIX* matParent);
	int		GetTrackIndex(float fTick, vector<AnimTrackInfo>* pTrackList);
};
