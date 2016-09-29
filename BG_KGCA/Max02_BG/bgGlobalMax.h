#pragma once
#include "bgTemplateMap.h"

#define CTL_CHARS		31			// ( US ) - Unit Separator
#define SINGLE_QUOTE	39			// ( ' )
#define ALMOST_ZERO		1.0e-3f

enum OBJECT_CLASS_TYPE
{
	CLASS_GEOM = 0,
	CLASS_BONE,
	CLASS_DUMMY,
	CLASS_BIPED,
};

typedef struct _D3D_MATRIX
{
	union
	{
		struct
		{
			float	_11, _12, _13, _14;
			float	_21, _22, _23, _24;
			float	_31, _32, _33, _34;
			float	_41, _42, _43, _44;
		};
		float m[4][4];
	};
} D3D_MATRIX, *LPD3D_MATRIX;

struct P3NC_Vertex
{
	float	posX, posY, posZ;
	float	norX, norY, norZ;
	Point4	col;
};

struct PNCT_Vertex
{
	Point3	pos;
	Point3	nor;
	Point4	col;
	Point2	tex;
};

struct bgBoundBox
{
	Point3	vMin;
	Point3	vMax;
};

struct bgTriangle
{
	PNCT_Vertex		vertex[3];
	int				iSubIndex;
};

struct bgAnimTrack
{
	int		iType;
	int		iTick;
	Quat	qValue;
	Point3	vValue;
};

struct bgScene
{
	int		iVersion;
	int		iFirstFrame;
	int		iLastFrame;
	int		iFrameSpeed;
	int		iTickPerFrame;
	int		iNumMesh;
	int		iMaxWeight;
	int		iBindPose;
};

struct bgTextureMap
{
	int		iIndex;
	int		iType;
	TSTR	strTextureName;
};

struct bgMaterial
{
	int		iIndex;
	int		iNumSubMaterial;
	int		iNumTextureMap;
	TSTR	strName;
	TSTR	strClassName;
	vector<bgTextureMap>	TextureMaps;
	vector<bgMaterial>		SubMaterials;
};

struct bgMeshHeader
{
	OBJECT_CLASS_TYPE	ClassType;
	Box3				Box;
	D3D_MATRIX			matWorld;
	int					iNumVertex;
	int					iNumFace;
	int					iMaterialRef;
	int					iNumTrack[4];
};

struct bgMesh : public bgMeshHeader
{
	TSTR				strNodeName;
	TSTR				strParentName;
	vector<bgAnimTrack>	PosTrack;
	vector<bgAnimTrack>	RotTrack;
	vector<bgAnimTrack>	SclTrack;
	vector<bgAnimTrack>	VisTrack;
	vector<bgTriangle>	Triangles;
	vector<bgMesh*>		pSubMeshes;

	bgMesh()
	{
		iMaterialRef = -1;
		iNumFace = 0;
		ClassType = CLASS_GEOM;
	}
	~bgMesh()
	{
		PosTrack.clear();
		RotTrack.clear();
		SclTrack.clear();
		VisTrack.clear();
		Triangles.clear();
	}
};

struct bgMatrixIndex
{
	OBJECT_CLASS_TYPE	m_ClassType;
	int					m_iIndex;
	TCHAR				m_szName[MAX_PATH];
	vector<P3NC_Vertex>	m_VertexList;
	Matrix3				m_NodeTM;
	Matrix3				m_InvNodeTM;
	INode*				m_pINode;
	Object*				m_pObject;
	Control*			m_pControl;
	bgMesh				m_Mesh;

	void	Add(int iIndex, TCHAR* szName);
	void	SetNode(INode* pNode, int iTick, bgMatrixIndex* pParentIndex = NULL);
	void	Release();
};

class bgGlobalMax
{
public:
	Interface*		m_p3DMax;
	INode*			m_pRootNode;
	Interval		m_Interval;

public:
	bgScene							m_Scene;
	bgTemplateMap<bgMatrixIndex>	m_MatrixMap;
	vector<Mtl*>					m_pMtlList;
	vector<bgMaterial>				m_MaterialList;

	TSTR			m_strCurrentFileName;
	TCHAR			m_szTemp[MAX_PATH];

public:
	bgGlobalMax();
	virtual ~bgGlobalMax();

public:
	void	DumpMatrix3(Matrix3* m, D3D_MATRIX& mat);
	void	DumpPoint3(Point3& pDest, Point3& pSrc);
	bool	EqualPoint2(Point2 p1, Point2 p2);
	bool	EqualPoint3(Point3 p1, Point3 p2);
	bool	EqualPoint4(Point4 p1, Point4 p2);

	bool	CheckNegativeTM(Matrix3 &m);
	TCHAR*	FixupName(MSTR name);

public:
	bool	Initialize(Interface* p3DMax = NULL);
	void	PreProcess(INode* pNode);

	bool	CheckFile(Interface* p3DMax);
	void	AddObject(INode* pNode);
	void	AddMtl(INode* pNode);
	int		GetMtlRef(Mtl* pMtl);
	int		GetIndex(const TCHAR* szNodeName) { return m_MatrixMap.GetID(szNodeName); }
	void	SetBindPose(bool bBindPose = false) { m_Scene.iBindPose = bBindPose; }
};
