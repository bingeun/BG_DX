#include "bgDevice.h"
#include "bgSys.h"

bgDevice::bgDevice()
{
}

bgDevice::~bgDevice()
{
	ReleaseDevice();
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

	SAFE_DEL_ARRAY(pDisplayModeList);
	SAFE_RELEASE(pAdapterOutput);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pFactory);

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //_DEBUG
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
	SwapChainDesc.Windowed = !bFullScreen;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Flags = 0;
	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	m_DriverType = D3D_DRIVER_TYPE_HARDWARE;
	HR_RETURN(D3D11CreateDeviceAndSwapChain(NULL, m_DriverType, NULL, createDeviceFlags, &FeatureLevel, 1, D3D11_SDK_VERSION,
											&SwapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDContext));

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

	m_pDContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZeroMemory(&DepthStencilViewDesc, sizeof(DepthStencilViewDesc));
	DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;
	HR_RETURN(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &DepthStencilViewDesc, &m_pDepthStencilView));

	m_pDContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	int iViewport;
	iViewport = 0; // 0 = 단일모드
	m_Viewport[iViewport].Width = (float)iWidth;
	m_Viewport[iViewport].Height = (float)iHeight;
	m_Viewport[iViewport].MinDepth = 0.0f;
	m_Viewport[iViewport].MaxDepth = 1.0f;
	m_Viewport[iViewport].TopLeftX = 0.0f;
	m_Viewport[iViewport].TopLeftY = 0.0f;
	m_CameraViewport[iViewport].m_Eye = D3DXVECTOR3(1.0f, 1.0f, -5.0f);
	m_CameraViewport[iViewport].m_At = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_CameraViewport[iViewport].m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	////////////////////////////////////

	iViewport++; // 1 = 2x2모드 ↖
	m_Viewport[iViewport].Width = (float)iWidth / 2.0f;
	m_Viewport[iViewport].Height = (float)iHeight / 2.0f;
	m_Viewport[iViewport].MinDepth = 0.0f;
	m_Viewport[iViewport].MaxDepth = 1.0f;
	m_Viewport[iViewport].TopLeftX = 0.0f;
	m_Viewport[iViewport].TopLeftY = 0.0f;
	m_CameraViewport[iViewport].m_Eye = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_CameraViewport[iViewport].m_At = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_CameraViewport[iViewport].m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	iViewport++; // 2 = 2x2모드 ↗
	m_Viewport[iViewport].Width = (float)iWidth / 2.0f;
	m_Viewport[iViewport].Height = (float)iHeight / 2.0f;
	m_Viewport[iViewport].MinDepth = 0.0f;
	m_Viewport[iViewport].MaxDepth = 1.0f;
	m_Viewport[iViewport].TopLeftX = (float)iWidth / 2.0f;
	m_Viewport[iViewport].TopLeftY = 0.0f;
	m_CameraViewport[iViewport].m_Eye = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_CameraViewport[iViewport].m_At = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_CameraViewport[iViewport].m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	iViewport++; // 3 = 2x2모드 ↙
	m_Viewport[iViewport].Width = (float)iWidth / 2.0f;
	m_Viewport[iViewport].Height = (float)iHeight / 2.0f;
	m_Viewport[iViewport].MinDepth = 0.0f;
	m_Viewport[iViewport].MaxDepth = 1.0f;
	m_Viewport[iViewport].TopLeftX = 0.0f;
	m_Viewport[iViewport].TopLeftY = (float)iHeight / 2.0f;
	m_CameraViewport[iViewport].m_Eye = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_CameraViewport[iViewport].m_At = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_CameraViewport[iViewport].m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	iViewport++; // 4 = 2x2모드 ↘
	m_Viewport[iViewport].Width = (float)iWidth / 2.0f;
	m_Viewport[iViewport].Height = (float)iHeight / 2.0f;
	m_Viewport[iViewport].MinDepth = 0.0f;
	m_Viewport[iViewport].MaxDepth = 1.0f;
	m_Viewport[iViewport].TopLeftX = (float)iWidth / 2.0f;
	m_Viewport[iViewport].TopLeftY = (float)iHeight / 2.0f;
	m_CameraViewport[iViewport].m_Eye = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_CameraViewport[iViewport].m_At = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_CameraViewport[iViewport].m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	////////////////////////////////////

	iViewport++; // 5 = 4+1모드 좌측 [0]123
	m_Viewport[iViewport].Width = (float)iWidth / 4.0f;
	m_Viewport[iViewport].Height = (float)iHeight / 4.0f;
	m_Viewport[iViewport].MinDepth = 0.0f;
	m_Viewport[iViewport].MaxDepth = 1.0f;
	m_Viewport[iViewport].TopLeftX = 0.0f;
	m_Viewport[iViewport].TopLeftY = 0.0f;
	m_CameraViewport[iViewport].m_Eye = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_CameraViewport[iViewport].m_At = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_CameraViewport[iViewport].m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	iViewport++; // 6 = 4+1모드 좌측 0[1]23
	m_Viewport[iViewport].Width = (float)iWidth / 4.0f;
	m_Viewport[iViewport].Height = (float)iHeight / 4.0f;
	m_Viewport[iViewport].MinDepth = 0.0f;
	m_Viewport[iViewport].MaxDepth = 1.0f;
	m_Viewport[iViewport].TopLeftX = 0.0f;
	m_Viewport[iViewport].TopLeftY = (float)iHeight / 4.0f;
	m_CameraViewport[iViewport].m_Eye = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_CameraViewport[iViewport].m_At = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_CameraViewport[iViewport].m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	iViewport++; // 7 = 4+1모드 좌측 01[2]3
	m_Viewport[iViewport].Width = (float)iWidth / 4.0f;
	m_Viewport[iViewport].Height = (float)iHeight / 4.0f;
	m_Viewport[iViewport].MinDepth = 0.0f;
	m_Viewport[iViewport].MaxDepth = 1.0f;
	m_Viewport[iViewport].TopLeftX = 0.0f;
	m_Viewport[iViewport].TopLeftY = (float)iHeight / 4.0f * 2.0f;
	m_CameraViewport[iViewport].m_Eye = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_CameraViewport[iViewport].m_At = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_CameraViewport[iViewport].m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	iViewport++; // 8 = 4+1모드 좌측 012[3]
	m_Viewport[iViewport].Width = (float)iWidth / 4.0f;
	m_Viewport[iViewport].Height = (float)iHeight / 4.0f;
	m_Viewport[iViewport].MinDepth = 0.0f;
	m_Viewport[iViewport].MaxDepth = 1.0f;
	m_Viewport[iViewport].TopLeftX = 0.0f;
	m_Viewport[iViewport].TopLeftY = (float)iHeight / 4.0f * 3.0f;
	m_CameraViewport[iViewport].m_Eye = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_CameraViewport[iViewport].m_At = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_CameraViewport[iViewport].m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	m_pDContext->RSSetViewports(1, &m_Viewport[0]);

	D3D11_RASTERIZER_DESC RSDesc;
	ZeroMemory(&RSDesc, sizeof(D3D11_RASTERIZER_DESC));
	RSDesc.FillMode = D3D11_FILL_WIREFRAME;
	RSDesc.CullMode = D3D11_CULL_NONE;
	m_pDevice->CreateRasterizerState(&RSDesc, &m_pRSWireNone);

	RSDesc.FillMode = D3D11_FILL_SOLID;
	RSDesc.CullMode = D3D11_CULL_NONE;
	m_pDevice->CreateRasterizerState(&RSDesc, &m_pRSSolidNone);

	RSDesc.FillMode = D3D11_FILL_SOLID;
	RSDesc.CullMode = D3D11_CULL_FRONT;
	m_pDevice->CreateRasterizerState(&RSDesc, &m_pRSSolidFront);



	m_fFieldOfView = (float)D3DX_PI / 4.0f;
	m_fAspect = (float)iWidth / (float)iHeight;
	m_fScreenNear = 0.1f;
	m_fScreenFar = 1000.0f;
	D3DXMatrixIdentity(&g_MatrixBuffer.matWorld);
	D3DXMatrixLookAtLH(&g_MatrixBuffer.matView, &m_CameraViewport[0].m_Eye, &m_CameraViewport[0].m_At, &m_CameraViewport[0].m_Up);
	D3DXMatrixPerspectiveFovLH(&g_MatrixBuffer.matProj, m_fFieldOfView, m_fAspect, m_fScreenNear, m_fScreenFar);

	D3DXMatrixTranspose(&g_MatrixBuffer.matWorld, &g_MatrixBuffer.matWorld);
	D3DXMatrixTranspose(&g_MatrixBuffer.matView, &g_MatrixBuffer.matView);
	D3DXMatrixTranspose(&g_MatrixBuffer.matProj, &g_MatrixBuffer.matProj);

	return hr;
}

void bgDevice::ReleaseDevice()
{
	if (m_pSwapChain)
	{
		m_pSwapChain->SetFullscreenState(FALSE, NULL);
	}

	SAFE_RELEASE(m_pRSWireNone);
	SAFE_RELEASE(m_pRSSolidNone);
	SAFE_RELEASE(m_pRSSolidFront);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pDepthStencilState);
	SAFE_RELEASE(m_pDepthStencilBuffer);

	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pDContext);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pSwapChain);
}

HRESULT bgDevice::CreateCB()
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC CBDesc;
	CBDesc.ByteWidth = sizeof(MATRIX_BUFFER);
	CBDesc.Usage = D3D11_USAGE_DYNAMIC;
	CBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CBDesc.MiscFlags = 0;
	CBDesc.StructureByteStride = 0;
	HR_RETURN(m_pDevice->CreateBuffer(&CBDesc, NULL, &m_pMatrixBuffer));

	return hr;
}
