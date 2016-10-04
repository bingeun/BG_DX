#pragma once
#include "bgAnimMax.h"

#define I_Exporter bgExporterMax::GetInstance()

class bgExporterMax : public bgAnimMax, public bgSingleton<bgExporterMax>
{
public:
	FILE*	m_pStream;
	TSTR	m_strFileName;

public:
	bgExporterMax();
	virtual ~bgExporterMax();

public:
	bool	ExportBG3D();
	bool	ExportBGSKN();
	bool	ExportBGMTX();
	bool	ExportBGACT();

public:
	bool 	Init(Interface* p3DMax, TSTR strFileName);
	bool	Release();

	TCHAR*	SaveFileDialog(TCHAR* szExt, TCHAR* szTitle = _T("BG 3DSMax Export"));
	int		IsEqulVertexList(vector<PNCT_Vertex>& VertexArray, PNCT_Vertex& Vertex);
	int		SetUniqueBuffer(bgMesh* pMesh, int iMtrl, int iStartTri, vector<PNCT_Vertex>& VertexArray, vector<int>& IndexArray);

	void	ExpScene();
	void	ExpMaterial();
	void	ExpTexture(vector<bgTextureMap>& TexMap, TCHAR* szLineHead = _T("\t"));
	void	ExpObject();
	int		ExpMesh(bgMesh* pMesh, int iMtl = -1, int iAddCount = 0);
	void	ExpAnimation(bgMesh* pMesh);
};
