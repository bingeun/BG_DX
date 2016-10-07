#pragma once
#include "bgExporterMax.h"

#define I_ExporterSkin bgExporterSkin::GetInstance()

#define MAX_WEIGHT_BIPED	8

struct bgBipedVertex
{
	int				m_dwNumWeight;
	vector<BYTE>	m_BipIDList;
	vector<float>	m_fWeightList;
};

struct P3NCT3_Vertex
{
	Point3		pos;
	Point3		nor;
	Point4		col;
	Point2		tex;
	float		w[4];	// W0, fW1, fW2, fNumWeight
	float		i[4];	// I0, I1, I2, I3
};

struct PNCT5_Vertex
{
	Point3		pos;
	Point3		nor;
	Point4		col;
	Point2		tex;
	float		w1[4];	// W0, fW1, fW2, fW3
	float		i1[4];	// I0, I1, I2, I3
	float		w2[4];	// W4, fW5, fW6, fNumWeight
	float		i2[4];	// I4, I5, I6, I7
};

struct bgSkinTri : public bgTriangle
{
	PNCT5_Vertex	m_vVertex[3];
};

struct bgSkinMesh : public bgMesh
{
	vector<bgSkinTri>		m_SkinTriList;
	vector<bgSkinMesh*>		m_pSkinSubMesh;
	int						m_iNumTex;
	int						m_iTexType[13];
	TCHAR					m_szTexName[13][30];

	bgSkinMesh() {}
	virtual ~bgSkinMesh() {}
};

class bgExporterSkin : public bgSingleton<bgExporterSkin>
{
public:
	FILE*	m_pStream;
	TSTR	m_strFileName;
	TCHAR	m_szTemp[MAX_PATH];

	bgScene					m_Scene;
	vector<bgBipedVertex>	m_BipedList;
	vector<bgSkinMesh*>		m_SkinMeshList;

public:
	bgExporterSkin();
	virtual ~bgExporterSkin();

public:
	bool	ExportBGSKN();

public:
	bool	Init();
	bool	Release();

	TCHAR*	SaveFileDialog(TCHAR* szExt, TCHAR* szTitle = _T("BG 3DSMax Export"));
	TCHAR*	FixupName(MSTR name);
	int		IsEqualVertexList(vector<PNCT5_Vertex>& VertexArray, PNCT5_Vertex& Vertex);
	bool	GetNodeInfo(INode* node, TimeValue t);
	void	SetBipedInfo(INode* node);
	void	GenerateGroup(INode* node, Mesh* mesh, bgSkinMesh* pSkinMesh);
	void	LoadMaterial(bgSkinMesh* pMesh, Mtl* mtl);
	int		GetMapID(Class_ID cid, int subNo);
	void	SetTriangle(INode* node, Mesh* mesh, bgSkinMesh* pSkinMesh, bgSkinMesh* pSubMesh = NULL, int iMtrl = 0);
	void	SetVertexBiped(INode* node, Face*	face, int v0, int v1, int v2, bgSkinTri* pTri);
	Modifier*	FindModifier(INode *nodePtr, Class_ID classID);
	
	void	ExportPhysiqueData(INode* node, Modifier* phyMod);
	void	ExportSkinData(INode* node, Modifier* skinMod);

	void	ExpScene();
	void	ExpSkinMesh();
	bool	ExpMesh(bgSkinMesh* pMesh, int iNum);
	void	ExpNodeMatrix();
};
