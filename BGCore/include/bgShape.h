#pragma once
#include "bgStd.h"
#include "bgSys.h"

struct MatrixBuffer
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
};

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
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11RasterizerState*	m_pRasterizerState;

	D3D_PRIMITIVE_TOPOLOGY	m_uPrimitiveTopology;

	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11GeometryShader*	m_pGeometryShader;

	ID3D11Buffer*	m_pVertexBuffer;
	ID3D11Buffer*	m_pIndexBuffer;
	ID3D11Buffer*	m_pConstantBuffer;

	MatrixBuffer	m_ConstantData;
	D3DXMATRIX		m_matWorld;
	D3DXMATRIX		m_matView;
	D3DXMATRIX		m_matProj;

	UINT		m_iNumVertex;
	UINT		m_iNumIndex;

public:
	bgShape();
	virtual ~bgShape();

public:
	virtual bool	Init();
	virtual bool	Frame();
	virtual bool	Render();
	virtual bool	Release();

	virtual HRESULT	Create() = 0;
	virtual HRESULT	Load() = 0;

public:
	bool	Set(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT	CreateCB();
};
