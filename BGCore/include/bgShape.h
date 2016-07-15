#pragma once
#include "bgStd.h"

struct VertexPC
{
	D3DXVECTOR3 pos;
	D3DXVECTOR4 col;
};

struct VertexPCT
{
	D3DXVECTOR3 pos;
	D3DXVECTOR4 col;
	D3DXVECTOR2 tex;
};

struct VertexPCTN
{
	D3DXVECTOR3 pos;
	D3DXVECTOR4 col;
	D3DXVECTOR2 tex;
	D3DXVECTOR3 norm;
};

class bgShape
{
public:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
	D3D_PRIMITIVE_TOPOLOGY	m_uPrimitiveTopology;

	ID3D11Buffer*	m_pBufferVertex;
	ID3D11Buffer*	m_pBufferIndex;

	UINT	m_iNumVertex;
	UINT	m_iNumIndex;
	
public:
	bgShape();
	virtual ~bgShape();

public:
	virtual bool	Init();
	virtual bool	Frame();
	virtual bool	Render();
	virtual bool	Release();

	virtual HRESULT	Create() = 0;

public:
	bool	Set(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
};
