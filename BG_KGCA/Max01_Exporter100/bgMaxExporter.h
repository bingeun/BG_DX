#pragma once
#include "Max01_Exporter100.h"
#include <vector>

using namespace std;

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

struct PNCT_VERTEX
{
	Point3  pos;
	Point3  nor;
	Point4  col;
	Point2  tex;
};

struct bgMaxAnimTrack
{
	int		iTick;
	Quat	qValue;
	Point3	vValue;
};

struct bgMaxTri
{
	PNCT_VERTEX		v[3];
	int				iSubIndex;
};

struct bgMaxScene
{
	int		iVersion;
	int		iFirstFrame;
	int		iLastFrame;
	int		iFrameSpeed;
	int		iTickPerFrame;
	int		iNumMtl;
	int		iNumObject;
};

struct bgMaxTex
{
	int		iMapType;
	TSTR	szTexName;
};

struct bgMaxMtl
{
	TSTR				szName;
	vector<bgMaxTex>	TexList;
	vector<bgMaxMtl>	SubMtlsList;
};

struct bgMaxMesh
{
	int						iType;
	TSTR					NodeName;
	TSTR					ParentNodeName;
	D3D_MATRIX				matWorld;
	vector<bgMaxTri>		TriList;
	vector<bgMaxMesh>		SubMeshList;
	vector<bgMaxAnimTrack>	PosTrackList;
	vector<bgMaxAnimTrack>	RotTrackList;
	vector<bgMaxAnimTrack>	SclTrackList;
	Box3					box;
	int						iRef;
};

class bgMaxExporter
{
public:
	TSTR		m_szFileName;
	FILE*		m_pFile;
	Interface*	m_p3DSMax;
	Interval	m_Interval;
	INode*		m_pRootNode;

public:
	bgMaxScene			m_Scene;
	vector<bgMaxMtl>	m_MtlList;
	vector<Mtl*>		m_pMtlList;
	vector<bgMaxMesh>	m_ObjectList;
	vector<INode*>		m_pObjectList;

public:
	bgMaxExporter();
	virtual ~bgMaxExporter();

public:
	void	Init(TSTR name, Interface* ip);
	void	Release();

	bool	Exporter();
	bool	ExportScene();
	bool	ExportMaterial();
	bool	ExportObject();

	void	NodeProcess(INode* pNode);

	void	AddMtl(INode* pNode);
	void	AddObject(INode* pNode);
	void	GetTexture(Mtl* pMtl, bgMaxMtl& MtlData);
	void	GetMesh(INode* pNode, bgMaxMesh& tMesh);
	void	GetAnimation(INode* pNode, bgMaxMesh& tMesh);
	TriObject*	GetTriObjectFromNode(INode* pNode, int iTick, BOOL &needDel);

public:
	void	DumpPoint2(Point2& v, Point2& Vertex);
	void	DumpPoint3(Point3& v, Point3& Vertex);
	void	DumpPoint4(Point4& v, Point4& Vertex);
	void	DumpQuat(Quat& v, Quat& q);
	void	DumpMatrix3(Matrix3& matWorld, D3D_MATRIX& d3dWorld);
	TSTR	FixupName(TSTR name);
};
