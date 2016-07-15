#pragma once
#include "bgStd.h"

class bgDevice
{
public:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
	ID3D11RenderTargetView*	m_pRenderTargetView;
	IDXGISwapChain*			m_pSwapChain;

	ID3D11Texture2D*		m_pDepthStencilBuffer;
	ID3D11DepthStencilState*m_pDepthStencilState;
	ID3D11DepthStencilView*	m_pDepthStencilView;
	ID3D11RasterizerState*	m_pRasterizerState;

	ID3D11VertexShader*     m_pVertexShader;
	ID3D11PixelShader*      m_pPixelShader;

	D3D_DRIVER_TYPE			m_DriverType;
	BOOL					m_bVsync;

public:
	D3DXMATRIX		m_matWorld;
	D3DXMATRIX		m_matProj;
	D3DXMATRIX		m_matOrtho;

	float	m_fFieldOfView;
	float	m_fAspect;
	float	m_fScreenNear;
	float	m_fScreenFar;

public:
	bgDevice();
	virtual ~bgDevice();

public:
	HRESULT	InitDevice(HWND hWnd, UINT iWidth = 800, UINT iHeight = 600, BOOL bFullScreen = FALSE, BOOL bVsync = FALSE);
	void	ReleaseDevice();
};
