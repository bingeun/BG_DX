#include "bgDevice.h"



HRESULT bgDevice::InitDevice(HWND hWnd, UINT iWidth, UINT iHeight, BOOL IsFullScreen)
{
	HRESULT hr = S_OK;
	if (FAILED(hr = CreateDevice()))
	{
		MessageBox(0, _T("CreateDevice FAILED"), _T("Fatal error"), MB_OK);
		return hr;
	}
	if (FAILED(hr = CreateSwapChain(hWnd, iWidth, iHeight, IsFullScreen)))
	{
		MessageBox(0, _T("CreateSwapChain FAILED"), _T("Fatal error"), MB_OK);
		return hr;
	}
	if (FAILED(hr = SetRenderTargetView()))
	{
		MessageBox(0, _T("SetRenderTargetView FAILED"), _T("Fatal error"), MB_OK);
		return hr;
	}
	if (FAILED(hr = SetViewPort()))
	{
		MessageBox(0, _T("SetViewPort FAILED"), _T("Fatal error"), MB_OK);
		return hr;
	}
	if (FAILED(hr = CreateDxResource()))
	{
		return hr;
	}
	if (FAILED(hr = m_pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER)))
	{
		return hr;
	}
	return hr;
}

HRESULT bgDevice::CreateDevice()
{
	HRESULT hr = S_OK;
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //_DEBUG

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_DriverType = driverTypes[driverTypeIndex];
		if (SUCCEEDED(hr = D3D11CreateDevice(NULL, m_DriverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
											D3D11_SDK_VERSION, &m_pd3dDevice, &m_FeatureLevel, &m_pImmediateContext)))
		{
			if (FAILED(hr) || (m_FeatureLevel < D3D_FEATURE_LEVEL_11_0))
			{
				if (m_pImmediateContext)
					m_pImmediateContext->Release();
				if (m_pd3dDevice)
					m_pd3dDevice->Release();
				continue;
			}
			break;
		}
	}
	if (FAILED(hr))
		return hr;

	return CreateGIFactory();
}

HRESULT bgDevice::CreateSwapChain(HWND hWnd, UINT iWidth, UINT iHeight, BOOL IsFullScreen)
{
	HRESULT hr = S_OK;

	SetFullScreenFlag(IsFullScreen);
	if (m_pFactory == NULL)
		return S_FALSE;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = iWidth;
	sd.BufferDesc.Height = iHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = !m_IsFullScreen;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (FAILED(hr = m_pFactory->CreateSwapChain(m_pd3dDevice, &sd, &m_pSwapChain)))
	{
		return hr;
	}
	return hr;
}

HRESULT bgDevice::CreateGIFactory()
{
	if (m_pd3dDevice == NULL)
		return E_FAIL;

	HRESULT hr;
	IDXGIDevice* pDXGIDevice;
	hr = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);

	IDXGIAdapter* pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

	//IDXGIFactory* pIDXGIFactory;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&m_pFactory);

	pDXGIAdapter->Release();
	pDXGIDevice->Release();

	return S_OK;
}

HRESULT bgDevice::SetRenderTargetView()
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT bgDevice::SetViewPort()
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* pBackBuffer;
	if (FAILED(hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
		return hr;

	if (FAILED(hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView)))
		return hr;

	pBackBuffer->Release();
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

	return hr;
}

void bgDevice::ClearD3D11DeviceContext(ID3D11DeviceContext* pd3dDeviceContext)
{
	if (pd3dDeviceContext == NULL)
		return;

	ID3D11DepthStencilView* pDSV        = NULL;
	ID3D11ShaderResourceView* pSRVs[16] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
	ID3D11RenderTargetView* pRTVs[16]   = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
	ID3D11Buffer* pBuffers[16]          = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
	ID3D11SamplerState* pSamplers[16]   = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
	UINT StrideOffset[16]               = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };

	// Shaders
	pd3dDeviceContext->VSSetShader(NULL, NULL, 0);
	pd3dDeviceContext->HSSetShader(NULL, NULL, 0);
	pd3dDeviceContext->DSSetShader(NULL, NULL, 0);
	pd3dDeviceContext->GSSetShader(NULL, NULL, 0);
	pd3dDeviceContext->PSSetShader(NULL, NULL, 0);

	// IA clear
	pd3dDeviceContext->IASetVertexBuffers(0, 16, pBuffers, StrideOffset, StrideOffset);
	pd3dDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R16_UINT, 0);
	pd3dDeviceContext->IASetInputLayout(NULL);

	// Constant buffers
	pd3dDeviceContext->VSSetConstantBuffers(0, 14, pBuffers);
	pd3dDeviceContext->HSSetConstantBuffers(0, 14, pBuffers);
	pd3dDeviceContext->DSSetConstantBuffers(0, 14, pBuffers);
	pd3dDeviceContext->GSSetConstantBuffers(0, 14, pBuffers);
	pd3dDeviceContext->PSSetConstantBuffers(0, 14, pBuffers);

	// Resources
	pd3dDeviceContext->VSSetShaderResources(0, 16, pSRVs);
	pd3dDeviceContext->HSSetShaderResources(0, 16, pSRVs);
	pd3dDeviceContext->DSSetShaderResources(0, 16, pSRVs);
	pd3dDeviceContext->GSSetShaderResources(0, 16, pSRVs);
	pd3dDeviceContext->PSSetShaderResources(0, 16, pSRVs);

	// Samplers
	pd3dDeviceContext->VSSetSamplers(0, 16, pSamplers);
	pd3dDeviceContext->HSSetSamplers(0, 16, pSamplers);
	pd3dDeviceContext->DSSetSamplers(0, 16, pSamplers);
	pd3dDeviceContext->GSSetSamplers(0, 16, pSamplers);
	pd3dDeviceContext->PSSetSamplers(0, 16, pSamplers);

	// Render targets
	pd3dDeviceContext->OMSetRenderTargets(8, pRTVs, pDSV);

	// States
	FLOAT blendFactor[4] = { 0,0,0,0 };
	pd3dDeviceContext->OMSetBlendState(NULL, blendFactor, 0xFFFFFFFF);
	pd3dDeviceContext->OMSetDepthStencilState(NULL, 0);
	pd3dDeviceContext->RSSetState(NULL);
}

bgDevice::bgDevice()
{
	m_DriverType = D3D_DRIVER_TYPE_NULL;
	m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	m_pd3dDevice = NULL;
	m_pSwapChain = NULL;
	m_pRenderTargetView = NULL;
	m_pImmediateContext = NULL;
}

bgDevice::~bgDevice()
{
	ClearD3D11DeviceContext(m_pImmediateContext);

	if (m_pImmediateContext)
		m_pImmediateContext->ClearState();
	if (m_pImmediateContext)
		m_pImmediateContext->Flush();
	if (m_pRenderTargetView)
		m_pRenderTargetView->Release();
	if (m_pSwapChain)
		m_pSwapChain->Release();
	if (m_pImmediateContext)
		m_pImmediateContext->Release();
	if (m_pd3dDevice)
		m_pd3dDevice->Release();
	if (m_pFactory)
		m_pFactory->Release();

	m_pd3dDevice = NULL;
	m_pSwapChain = NULL;
	m_pRenderTargetView = NULL;
	m_pImmediateContext = NULL;
	m_pFactory = NULL;
}
