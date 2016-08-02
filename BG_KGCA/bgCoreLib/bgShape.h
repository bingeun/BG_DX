#pragma once
#include "bgStd.h"

struct MATRIX_BUFFER
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
	ID3D11DeviceContext*	m_pDContext;
	ID3D11RasterizerState*	m_pRasterizerState;

public:
	ID3D11InputLayout*		m_pInputLayout;
	D3D_PRIMITIVE_TOPOLOGY	m_ePrimitiveTopology;

	ID3D11VertexShader*		m_pVS; // Vertex Shader
	ID3D11PixelShader*		m_pPS; // Pixel Shader
	ID3D11GeometryShader*	m_pGS; // Geometry Shader
	ID3D11HullShader*		m_pHS; // Hull Shader
	ID3D11DomainShader*		m_pDS; // Domain Shader
	ID3D11ComputeShader*	m_pCS; // Compute Shader

	ID3D11Buffer*	m_pVB; // Vertex Buffer;
	ID3D11Buffer*	m_pIB; // Index Buffer;
	ID3D11Buffer*	m_pCB; // Constant Buffer;

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

	virtual HRESULT	CreateBuffer() { return true; }
	virtual HRESULT	LoadShader() { return true; }

public:
	bool	SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11RasterizerState* pRasterizerState, ID3D11Buffer* pCB = NULL);
};
