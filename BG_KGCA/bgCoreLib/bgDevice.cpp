#include "bgDevice.h"

bgDevice::bgDevice()
{
}

bgDevice::~bgDevice()
{
	Release();
}

bool bgDevice::Init()
{
	return true;
}

bool bgDevice::Release()
{
	if (m_pSwapChain)
	{
		m_pSwapChain->SetFullscreenState(FALSE, NULL);
	}

	SAFE_RELEASE(m_pRasterizerState);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pDepthStencilState);
	SAFE_RELEASE(m_pDepthStencilBuffer);

	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pSwapChain);

	return true;
}

HRESULT bgDevice::InitDevice(HWND hWnd, UINT iWidth, UINT iHeight, BOOL bFullScreen, BOOL bVsync)
{
	HRESULT hr = S_OK;

	m_bVsync = bVsync;

	IDXGIFactory* pFactory;
	HR_RETURN(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory));

	IDXGIAdapter* pAdapter;
	HR_RETURN(pFactory->EnumAdapters(0, &pAdapter));

	IDXGIOutput* pAdapterOutput;
	HR_RETURN(pAdapter->EnumOutputs(0, &pAdapterOutput));

	UINT iNumModes;
	HR_RETURN(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &iNumModes, NULL));

	DXGI_MODE_DESC* pDisplayModeList;
	pDisplayModeList = new DXGI_MODE_DESC[iNumModes];
	if (!pDisplayModeList)
		return S_FALSE;

	HR_RETURN(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &iNumModes, pDisplayModeList));

	UINT iNumerator, iDenominator;
	for (UINT iMode = 0; iMode < iNumModes; iMode++)
	{
		if (pDisplayModeList[iMode].Width == iWidth && pDisplayModeList[iMode].Height == iHeight)
		{
			iNumerator = pDisplayModeList[iMode].RefreshRate.Numerator;
			iDenominator = pDisplayModeList[iMode].RefreshRate.Denominator;
		}
	}

	DXGI_ADAPTER_DESC AdapterDesc;
	HR_RETURN(pAdapter->GetDesc(&AdapterDesc));

	// 비디오카드 메모리(MB) 계산
	// m_videoCardMemory = (int)(AdapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Release!
	SAFE_DEL_ARRAY(pDisplayModeList);
	SAFE_RELEASE(pAdapterOutput);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pFactory);
	// Release!

	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));
	SwapChainDesc.BufferDesc.Width = iWidth;
	SwapChainDesc.BufferDesc.Height = iHeight;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = (bVsync) ? iNumerator : 0;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = (bVsync) ? iDenominator : 1;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.OutputWindow = hWnd;
	SwapChainDesc.Windowed = bFullScreen;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Flags = 0;
	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	m_DriverType = D3D_DRIVER_TYPE_HARDWARE;
	HR_RETURN(D3D11CreateDeviceAndSwapChain(NULL, m_DriverType, NULL, 0, &FeatureLevel, 1, D3D11_SDK_VERSION,
											&SwapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext));

	ID3D11Texture2D* pBackBuffer;
	HR_RETURN(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));
	HR_RETURN(m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView));
	SAFE_RELEASE(pBackBuffer);

	D3D11_TEXTURE2D_DESC DepthBufferDesc;
	ZeroMemory(&DepthBufferDesc, sizeof(DepthBufferDesc));
	DepthBufferDesc.Width = iWidth;
	DepthBufferDesc.Height = iHeight;
	DepthBufferDesc.MipLevels = 1;
	DepthBufferDesc.ArraySize = 1;
	DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthBufferDesc.SampleDesc.Count = 1;
	DepthBufferDesc.SampleDesc.Quality = 0;
	DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthBufferDesc.CPUAccessFlags = 0;
	DepthBufferDesc.MiscFlags = 0;
	HR_RETURN(m_pDevice->CreateTexture2D(&DepthBufferDesc, NULL, &m_pDepthStencilBuffer));

	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));
	DepthStencilDesc.DepthEnable = true;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	DepthStencilDesc.StencilEnable = true;
	DepthStencilDesc.StencilReadMask = 0xFF;
	DepthStencilDesc.StencilWriteMask = 0xFF;
	DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	DepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR_RETURN(m_pDevice->CreateDepthStencilState(&DepthStencilDesc, &m_pDepthStencilState));

	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZeroMemory(&DepthStencilViewDesc, sizeof(DepthStencilViewDesc));
	DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;
	HR_RETURN(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &DepthStencilViewDesc, &m_pDepthStencilView));

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	D3D11_RASTERIZER_DESC RasterizerDesc;
	RasterizerDesc.AntialiasedLineEnable = false;
	RasterizerDesc.CullMode = D3D11_CULL_BACK;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.DepthClipEnable = true;
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.FrontCounterClockwise = false;
	RasterizerDesc.MultisampleEnable = false;
	RasterizerDesc.ScissorEnable = false;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	HR_RETURN(m_pDevice->CreateRasterizerState(&RasterizerDesc, &m_pRasterizerState));

	m_pDeviceContext->RSSetState(m_pRasterizerState);

	D3D11_VIEWPORT Viewport;
	Viewport.Width = (float)iWidth;
	Viewport.Height = (float)iHeight;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;
	m_pDeviceContext->RSSetViewports(1, &Viewport);

	return hr;
}
