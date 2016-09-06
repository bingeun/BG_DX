#pragma once
#include "Max01_Exporter100.h"
#include <vector>

using namespace std;

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
	int		iMapType;
	TSTR	szTexName;

	Mtl*	pMtl;
	int		iNumSubMtls;
	vector<bgMaxTex>	TexList;
	vector<bgMaxMtl>	SubMtlsList;
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
	vector<INode*>		m_ObjectList;

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
};
