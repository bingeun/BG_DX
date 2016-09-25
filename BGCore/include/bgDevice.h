#pragma once
#include "bgStd.h"
#include "bgCamera.h"
#include "bgShape.h"

#define SIZE_VIEWPORT	(1+4+4)

class bgDevice
{
public:
	ID3D11Device*				m_pDevice;
	ID3D11DeviceContext*		m_pDContext;
	IDXGISwapChain*				m_pSwapChain;
	IDXGIFactory*				m_pGIFactory;
	ID3D11RenderTargetView*		m_pRenderTargetView;
	ID3D11DepthStencilView*		m_pDepthStencilView;

	D3D11_VIEWPORT				m_Viewport;
	bgCamera					m_Camera;

public:
	ID3D11DepthStencilState*	m_pDSSDepth;
	ID3D11SamplerState*			m_pSSTexture;
	ID3D11BlendState*			m_pBSAlpha;

	ID3D11RasterizerState*		m_pRSWireNone;
	ID3D11RasterizerState*		m_pRSWireFront;
	ID3D11RasterizerState*		m_pRSSolidNone;
	ID3D11RasterizerState*		m_pRSSolidFront;

public:
	D3D_DRIVER_TYPE				m_iDriverType;
	D3D_FEATURE_LEVEL			m_iFeatureLevel;
	BOOL						m_bVsync;
	UINT						m_iNumerator;
	UINT						m_iDenominator;

public:
	bgDevice();
	virtual ~bgDevice();

public:
	HRESULT	InitDevice(HWND hWnd, UINT iWidth = 800, UINT iHeight = 600, BOOL bFullScreen = FALSE, BOOL bVsync = FALSE);
	void	ReleaseDevice();
	
	HRESULT	CreateDXGI(UINT iWidth, UINT iHeight);
	HRESULT	CreateDevice();
	HRESULT	CreateSwapChain(HWND hWnd, UINT iWidth, UINT iHeight, BOOL bFullScreen = FALSE);
	HRESULT	CreateRenderTargetView();
	HRESULT	CreateDepthStencilView();
	HRESULT	CreateState();

	void	SetViewport(UINT iWidth, UINT iHeight);
	void	SetRenderTarget();
};
