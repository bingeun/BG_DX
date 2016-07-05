#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgCore.h"

struct P3VERTEX
{
	float fPositionX;
	float fPositionY;
	float fPositionZ;
};

class dx01 : public bgCore
{
public:
	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11Buffer*			m_pVertexBuffer;
	ID3D11Buffer*			m_pIndexBuffer;
	ID3D11Buffer*			m_pConstantBuffer;

	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	D3D_PRIMITIVE_TOPOLOGY	m_PrimType;

	D3DXMATRIX				m_matWorld;
	D3DXMATRIX				m_matView;
	D3DXMATRIX				m_matProj;

public:
	bool	m_bWireFrameRender;
	int		m_iPrimitiveType;

	ID3D11RasterizerState*	m_pRasterizerStateNoCull;

public:
	HRESULT		CreateTriangle(D3D11_PRIMITIVE_TOPOLOGY iTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	HRESULT		CreateVertexBuffer();
	HRESULT		CreateIndexBuffer();
	HRESULT		CreateConstantBuffer();
	HRESULT		LoadShaderAndInputLayout();


public:
	HRESULT		SetRasterizerState(D3D11_FILL_MODE d3dFillMode = D3D11_FILL_SOLID);
	LRESULT		MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool		Init();
	bool		Frame();
	bool		Render();
	bool		Release();

public:
	dx01();
	virtual ~dx01();
};
