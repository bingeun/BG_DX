#include "bgDevice.h"

bgDevice* g_pDevice;

bgDevice::bgDevice()
{
	g_pDevice = this;
}

bgDevice::~bgDevice()
{
	ReleaseDevice();
}

HRESULT bgDevice::InitDevice(HWND hWnd, UINT iWidth, UINT iHeight, BOOL bFullScreen, BOOL bVsync)
{
	HRESULT hr = S_OK;

	m_bVsync = bVsync;

	HR_RETURN(CreateDXGI(iWidth, iHeight));
	HR_RETURN(CreateDevice());
	HR_RETURN(CreateSwapChain(hWnd, iWidth, iHeight));
	HR_RETURN(CreateRenderTargetView());
	HR_RETURN(CreateDepthStencilView());
	HR_RETURN(CreateState());

	SetViewport(iWidth, iHeight);
	SetRenderTarget();
	
	m_Camera.Init(m_pDevice, m_pDContext);
	
	return hr;
}

void bgDevice::ReleaseDevice()
{
	m_pSwapChain->SetFullscreenState(FALSE, NULL);

	SAFE_RELEASE(m_pDSSDepth);
	SAFE_RELEASE(m_pSSTexture);
	SAFE_RELEASE(m_pBSAlpha);
	SAFE_RELEASE(m_pRSWireNone);
	SAFE_RELEASE(m_pRSWireFront);
	SAFE_RELEASE(m_pRSSolidNone);
	SAFE_RELEASE(m_pRSSolidFront);

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDContext);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pGIFactory);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pDepthStencilView);
}

HRESULT bgDevice::CreateDXGI(UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;

	HR_RETURN(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pGIFactory));

	IDXGIAdapter* pAdapter;
	HR_RETURN(m_pGIFactory->EnumAdapters(0, &pAdapter));

	IDXGIOutput* pAdapterOutput;
	HR_RETURN(pAdapter->EnumOutputs(0, &pAdapterOutput));

	UINT iNumModes;
	HR_RETURN(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &iNumModes, NULL));

	DXGI_MODE_DESC* pDisplayModeList = new DXGI_MODE_DESC[iNumModes];
	if (!pDisplayModeList)
		return S_FALSE;

	HR_RETURN(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &iNumModes, pDisplayModeList));

	for (UINT iMode = 0; iMode < iNumModes; iMode++)
	{
		if (pDisplayModeList[iMode].Width == iWidth && pDisplayModeList[iMode].Height == iHeight)
		{
			m_iNumerator = pDisplayModeList[iMode].RefreshRate.Numerator;
			m_iDenominator = pDisplayModeList[iMode].RefreshRate.Denominator;
		}
	}

	if (m_iDenominator == 0)
	{
		m_iNumerator = 60;
		m_iDenominator = 1;
	}
	DXGI_ADAPTER_DESC AdapterDesc;
	HR_RETURN(pAdapter->GetDesc(&AdapterDesc));

	SAFE_DEL_ARRAY(pDisplayModeList);
	SAFE_RELEASE(pAdapterOutput);
	SAFE_RELEASE(pAdapter);

	return hr;
}

HRESULT bgDevice::CreateDevice()
{
	HRESULT hr = S_OK;

	UINT iCreateDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	iCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //_DEBUG

	D3D_DRIVER_TYPE DriverTypes[] =
	{
		D3D_DRIVER_TYPE_UNKNOWN,
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	D3D_FEATURE_LEVEL FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
	};

	for (UINT iDriver = 0; iDriver < COUNTOF(DriverTypes); iDriver++)
	{
		m_iDriverType = DriverTypes[iDriver];

		hr = D3D11CreateDevice(NULL, m_iDriverType, NULL, iCreateDeviceFlags, FeatureLevels, COUNTOF(FeatureLevels),
								D3D11_SDK_VERSION, &m_pDevice, &m_iFeatureLevel, &m_pDContext);
		if (SUCCEEDED(hr))
		{
			if (m_iFeatureLevel < D3D_FEATURE_LEVEL_11_0)
			{
				if (m_pDContext)
					m_pDContext->Release();
				if (m_pDevice)
					m_pDevice->Release();
				continue;
			}
			else
				break;
		}

	}

	return hr;
}

HRESULT bgDevice::CreateSwapChain(HWND hWnd, UINT iWidth, UINT iHeight, BOOL bFullScreen)
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));
	SwapChainDesc.BufferDesc.Width = iWidth;
	SwapChainDesc.BufferDesc.Height = iHeight;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = (m_bVsync) ? m_iNumerator : 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = (m_bVsync) ? m_iDenominator : 1;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.OutputWindow = hWnd;
	SwapChainDesc.Windowed = !bFullScreen;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	HR_RETURN(m_pGIFactory->CreateSwapChain(m_pDevice, &SwapChainDesc, &m_pSwapChain));

	return hr;
}

HRESULT bgDevice::CreateRenderTargetView()
{
	HRESULT hr = S_OK;

	ID3D11Texture2D* pBackBuffer = NULL;
	HR_RETURN(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));
	HR_RETURN(m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView));
	pBackBuffer->Release();

	return hr;
}

HRESULT bgDevice::CreateDepthStencilView()
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	m_pSwapChain->GetDesc(&SwapChainDesc);

	ID3D11Texture2D* pBackBuffer = NULL;
	D3D11_TEXTURE2D_DESC DepthBufferDesc;
	ZeroMemory(&DepthBufferDesc, sizeof(DepthBufferDesc));
	DepthBufferDesc.Width = SwapChainDesc.BufferDesc.Width;
	DepthBufferDesc.Height = SwapChainDesc.BufferDesc.Height;
	DepthBufferDesc.MipLevels = 1;
	DepthBufferDesc.ArraySize = 1;
	DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthBufferDesc.SampleDesc.Count = 1;
	DepthBufferDesc.SampleDesc.Quality = 0;
	DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthBufferDesc.CPUAccessFlags = 0;
	DepthBufferDesc.MiscFlags = 0;
	HR_RETURN(m_pDevice->CreateTexture2D(&DepthBufferDesc, NULL, &pBackBuffer));

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZeroMemory(&DepthStencilViewDesc, sizeof(DepthStencilViewDesc));
	DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;
	HR_RETURN(m_pDevice->CreateDepthStencilView(pBackBuffer, &DepthStencilViewDesc, &m_pDepthStencilView));

	SAFE_RELEASE(pBackBuffer);

	return hr;
}

HRESULT bgDevice::CreateState()
{
	HRESULT hr = S_OK;

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
	HR_RETURN(m_pDevice->CreateDepthStencilState(&DepthStencilDesc, &m_pDSSDepth));

	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(D3D11_BLEND_DESC));
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR_RETURN(m_pDevice->CreateBlendState(&BlendDesc, &m_pBSAlpha));

	D3D11_SAMPLER_DESC SamplerDesc;
	ZeroMemory(&SamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.MaxAnisotropy = 16;
	SamplerDesc.BorderColor[0] = 1.0f;
	SamplerDesc.BorderColor[1] = 0.0f;
	SamplerDesc.BorderColor[2] = 0.0f;
	SamplerDesc.BorderColor[3] = 1.0f;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = 1;
	SamplerDesc.MipLODBias = 0;
	HR_RETURN(m_pDevice->CreateSamplerState(&SamplerDesc, &m_pSSTexture));

	D3D11_RASTERIZER_DESC RasterizerDesc;
	ZeroMemory(&RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	RasterizerDesc.CullMode = D3D11_CULL_NONE;
	HR_RETURN(m_pDevice->CreateRasterizerState(&RasterizerDesc, &m_pRSWireNone));

	RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	RasterizerDesc.CullMode = D3D11_CULL_BACK;
	HR_RETURN(m_pDevice->CreateRasterizerState(&RasterizerDesc, &m_pRSWireFront));

	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_NONE;
	HR_RETURN(m_pDevice->CreateRasterizerState(&RasterizerDesc, &m_pRSSolidNone));

	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_BACK;
	HR_RETURN(m_pDevice->CreateRasterizerState(&RasterizerDesc, &m_pRSSolidFront));

	return hr;
}

void bgDevice::SetViewport(UINT iWidth, UINT iHeight)
{
	m_Viewport.Width = (float)iWidth;
	m_Viewport.Height = (float)iHeight;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;
	m_Viewport.TopLeftX = 0.0f;
	m_Viewport.TopLeftY = 0.0f;
	m_pDContext->RSSetViewports(1, &m_Viewport);
}

void bgDevice::SetRenderTarget()
{
	m_pDContext->OMSetDepthStencilState(m_pDSSDepth, 1);
	m_pDContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
}
