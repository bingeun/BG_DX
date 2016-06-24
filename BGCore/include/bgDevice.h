#pragma once
#include "bgStd.h"

class bgDevice
{
public:
	ID3D11Device*			m_pd3dDevice;
	ID3D11DeviceContext*	m_pImmediateContext;
	IDXGISwapChain*			m_pSwapChain;
	ID3D11RenderTargetView*	m_pRenderTargetView;
	IDXGIFactory*			m_pFactory;

	D3D_FEATURE_LEVEL       m_FeatureLevel;
	D3D_DRIVER_TYPE         m_DriverType;
	DXGI_SWAP_CHAIN_DESC	m_SwapChainDesc;
	D3D11_VIEWPORT			m_ViewPort;

	BOOL					m_IsFullScreen;

public:
	HRESULT		InitDevice(HWND hWnd, UINT iWidth = 1024, UINT iHeight = 768, BOOL IsFullScreen = FALSE);

	HRESULT		CreateDevice();
	HRESULT		CreateSwapChain(HWND hWnd, UINT iWidth, UINT iHeight, BOOL IsFullScreen);
	HRESULT		CreateGIFactory();
	HRESULT		SetRenderTargetView();
	HRESULT		SetViewPort();

	void		SetFullScreenFlag(BOOL bFlag) { m_IsFullScreen = bFlag; }
	BOOL		GetFullScreenFlag() { return m_IsFullScreen; }
	void		ClearD3D11DeviceContext(ID3D11DeviceContext* pd3dDeviceContext);

	virtual	HRESULT	CreateDxResource() { return S_OK; }
	virtual	HRESULT	DeleteDxResource() { return S_OK; }

public:
	bgDevice();
	virtual ~bgDevice();
};
