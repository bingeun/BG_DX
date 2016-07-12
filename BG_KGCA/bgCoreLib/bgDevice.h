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

	D3D_DRIVER_TYPE			m_DriverType;
	BOOL					m_bVsync;
	
public:
	HRESULT		InitDevice(HWND hWnd, UINT iWidth = 1024, UINT iHeight = 768, BOOL bFullScreen = FALSE, BOOL bVsync = FALSE);

public:
	bool	Release();

public:
	bgDevice();
	virtual ~bgDevice();
};
