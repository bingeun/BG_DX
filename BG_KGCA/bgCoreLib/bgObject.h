#pragma once
#include "bgStd3D.h"

struct bgMesh
{
	D3DXMATRIX			matWorld;
	bgTexMap			Texture;
	vector<PNCT_VERTEX>	VertexList;
	vector<UINT>		IndexList;
	D3DXMATRIX			matCalculation;

	ID3D11Buffer*		pVertexBuffer;
	ID3D11Buffer*		pIndexBuffer;
};

class bgObject
{
public:
	bgScene				m_Scene;
	vector<bgMesh>		m_MeshList;
	vector<D3DXMATRIX>	m_matNodeList;

public:
	bgObject();
	virtual ~bgObject();

public:
	void	Clear();

	void	Interpolate(bgMesh* pMesh, float fTick, D3DXMATRIX* matParent = NULL);
};
