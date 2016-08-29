#pragma once
#include "bgShape.h"
#include "bgTextureMgr.h"
#include "bgSys.h"

class bgModel : public bgShape
{
public:
	vector<VertexPNCT>	m_VertexList;
	vector<UINT>		m_IndexList;
	vector<UINT>		m_TextureIDList;

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
};
