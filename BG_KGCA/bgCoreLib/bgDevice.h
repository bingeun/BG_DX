#pragma once
#include "bgStd.h"
#include "bgCamera.h"
#include "bgShape.h"

#define SIZE_VIEWPORT	(1+4+4)

class bgDevice
{
public:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDContext;
	ID3D11RenderTargetView*	m_pRenderTargetView;
	IDXGISwapChain*			m_pSwapChain;

	ID3D11Texture2D*		m_pBackBuffer;
	D3D11_TEXTURE2D_DESC	m_descBackBuffer;
	ID3D11Texture2D*		m_pDepthStencilBuffer;
	ID3D11DepthStencilState*m_pDepthStencilState;
	ID3D11DepthStencilView*	m_pDepthStencilView;

	ID3D11BlendState*		m_pAlphaBlend;
	ID3D11SamplerState*		m_pSamplerState;
	D3D11_FILTER			m_TexFilter;
	//UINT					m_iFilterCount;
	//D3D11_TEXTURE_ADDRESS_MODE	m_TexAddressMode;
	UINT					m_iTexAddressMode;

	ID3D11RasterizerState*	m_pRSWireNone;
	ID3D11RasterizerState*	m_pRSSolidNone;
	ID3D11RasterizerState*	m_pRSSolidFront;

	D3D11_VIEWPORT			m_Viewport[SIZE_VIEWPORT];
	D3D_DRIVER_TYPE			m_DriverType;
	BOOL					m_bVsync;

public:
	ID3D11Buffer*	m_pMatrixBuffer;
	bgCamera		m_CameraViewport[SIZE_VIEWPORT];

	float	m_fFieldOfView;
	float	m_fAspect;
	float	m_fScreenNear;
	float	m_fScreenFar;

	float	m_fSpeedCamera;

public:
	bgDevice();
	virtual ~bgDevice();

public:
	HRESULT	InitDevice(HWND hWnd, UINT iWidth = 800, UINT iHeight = 600, BOOL bFullScreen = FALSE, BOOL bVsync = FALSE);
	void	ReleaseDevice();

	void	TransMatrixBuffer(MATRIX_BUFFER* pMatrixBuffer, bgCamera* pCamera);
};
