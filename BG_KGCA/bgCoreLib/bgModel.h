#pragma once
#include "bgShape.h"
#include "bgTextureMgr.h"
#include "bgSys.h"

class bgModel : public bgShape
{
public:
	vector<ID3D11Buffer*>		m_pVBList;		// Vertex Buffer;
	vector<ID3D11Buffer*>		m_pIBList;		// Index Buffer;

	vector<vector<VertexPNCT>>	m_VertexList;
	vector<vector<UINT>>		m_IndexList;
	vector<MaterialTexID>		m_TexIDList;

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
