#include "dx01.h"



struct VS_CONSTANT_BUFFER
{
	D3DXVECTOR4 vColor;
	float fC5_X;
	float fTime;
	float fC5_Z;
	float fC5_W;
};


HRESULT dx01::CreateTriangle(D3D11_PRIMITIVE_TOPOLOGY iTopology)
{
	HRESULT hr = S_OK;

	if (FAILED(hr = CreateVertexBuffer()))
		return hr;
	if (FAILED(hr = CreateIndexBuffer()))
		return hr;
	if (FAILED(hr = LoadShaderAndInputLayout()))
		return hr;

	m_pImmediateContext->IASetPrimitiveTopology(iTopology);
	return hr;
}

HRESULT dx01::LoadShaderAndInputLayout()
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob* pVSBuf = NULL;

#if defined( _DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pBufferErrors = NULL;
	if (FAILED(hr = D3DX11CompileFromFile(L"dx01_vs.hlsl", NULL, NULL, "VS", "vs_5_0", dwShaderFlags, NULL, NULL, &pVSBuf, &pBufferErrors, NULL)))
	{
		TCHAR pMessage[500];
		mbstowcs(pMessage, (CHAR*)pBufferErrors->GetBufferPointer(), 500);
		MessageBox(NULL, pMessage, _T("ERROR"), MB_OK);
		return hr;
	}
	if (FAILED(hr = m_pd3dDevice->CreateVertexShader((DWORD*)pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		return hr;
	}

	// Compile the PS from the file
	ID3DBlob* pPSBuf = NULL;
	if (FAILED(hr = D3DX11CompileFromFile(L"dx01_ps.hlsl", NULL, NULL, "PS", "ps_5_0", dwShaderFlags, NULL, NULL, &pPSBuf, NULL, NULL)))
	{
		return hr;
	}
	if (FAILED(hr = m_pd3dDevice->CreatePixelShader((DWORD*)pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPixelShader)))
	{
		return hr;
	}

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	if (FAILED(hr = m_pd3dDevice->CreateInputLayout(layout, 1, pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), &m_pVertexLayout)))
	{
		return hr;
	}
	SAFE_RELEASE(pVSBuf);
	SAFE_RELEASE(pPSBuf);
	return hr;
}

HRESULT dx01::CreateVertexBuffer()
{
	HRESULT hr = S_OK;

	P3VERTEX vertices[] =
	{
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
	};

	UINT numVertices = sizeof(vertices) / sizeof(vertices[0]);

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(P3VERTEX) *numVertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	CD3D11_BUFFER_DESC cbc(sizeof(P3VERTEX) * 4, D3D11_BIND_VERTEX_BUFFER);

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(hr = m_pd3dDevice->CreateBuffer(&cbc, &InitData, &m_pVertexBuffer)))
	{
		return hr;
	}
	return hr;
}

HRESULT dx01::CreateIndexBuffer()
{
	HRESULT hr = S_OK;

	WORD indices[] =
	{
		0,1,2,
		0,2,3,
	};

	UINT iNumIndex = sizeof(indices) / sizeof(indices[0]);

	D3D11_BUFFER_DESC ibDesc;
	ibDesc.ByteWidth = iNumIndex * sizeof(WORD);
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA ibInitData;
	ZeroMemory(&ibInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	ibInitData.pSysMem = indices;
	if (FAILED(hr = m_pd3dDevice->CreateBuffer(&ibDesc, &ibInitData, &m_pIndexBuffer)))
	{
		return hr;
	}
	return hr;
}

HRESULT dx01::CreateConstantBuffer()
{
	HRESULT hr;

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	if (FAILED(hr = m_pd3dDevice->CreateBuffer(&cbDesc, NULL, &m_pConstantBuffer)))
	{
		return hr;
	}		
	return hr;
}

LRESULT	dx01::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
	}
	break;

	case WM_ACTIVATEAPP:
	{
		g_pWindow->m_bActivate = wParam;
	}
	break;

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case '0': // 풀스크린 or 윈도우
		{
			if (m_pSwapChain)
			{
				BOOL IsScreenMode = FALSE;
				m_pSwapChain->GetFullscreenState(&IsScreenMode, NULL);
				m_pSwapChain->SetFullscreenState(!IsScreenMode, NULL);
				SetFullScreenFlag(IsScreenMode);
				assert(GetFullScreenFlag() == IsScreenMode);
				if (IsScreenMode)
				{
					ShowWindow(hWnd, SW_SHOW);
				}
			}
		}
		break;
		case VK_F3:
		{
			m_bWireFrameRender = !m_bWireFrameRender;
			(m_bWireFrameRender) ? SetRasterizerState(D3D11_FILL_WIREFRAME) : SetRasterizerState(D3D11_FILL_SOLID);
		}
		break;
		case VK_F4:
		{
			++m_iPrimitiveType;
			m_iPrimitiveType = min(m_iPrimitiveType, 5);
			m_pImmediateContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_iPrimitiveType);
		}
		break;
		case VK_F5:
		{
			--m_iPrimitiveType;
			m_iPrimitiveType = max(m_iPrimitiveType, 1);
			m_pImmediateContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_iPrimitiveType);
		}
		break;
		case VK_UP:
		{

		}
		break;
		case VK_DOWN:
		{

		}
		break;
		case VK_ESCAPE:
		{
			PostQuitMessage(0);
		}
		break;
		}
	}
	break;

	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

HRESULT dx01::SetRasterizerState(D3D11_FILL_MODE d3d11FillMode)
{
	HRESULT hr = S_OK;
	SAFE_RELEASE(m_pRasterizerStateNoCull);
	D3D11_RASTERIZER_DESC RSDesc;
	memset(&RSDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
	RSDesc.DepthClipEnable = TRUE;
	RSDesc.FillMode = d3d11FillMode;
	RSDesc.CullMode = D3D11_CULL_NONE;
	if (FAILED(hr = m_pd3dDevice->CreateRasterizerState(&RSDesc, &m_pRasterizerStateNoCull)))
	{
		return hr;
	}
	return hr;
}

bool dx01::Init()
{
	if (FAILED(CreateTriangle()))
	{
		MessageBox(0, _T("CreateTrangle 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateConstantBuffer()))
	{
		MessageBox(0, _T("CreateConstantBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixLookAtLH(&m_matView, &D3DXVECTOR3(0.0f, 0.0f, -2.0f), &D3DXVECTOR3(0.0f, 0.0f, 2.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	D3DXMatrixPerspectiveFovLH(&m_matProj, (float)D3DX_PI * 0.5f, m_iWindowW / (FLOAT)m_iWindowH, 0.1f, 100.0f);

	m_Timer.Start();
	return true;
}

bool dx01::Frame()
{
	float fTime = m_Timer.GetElapsedTime();
	float fBoundedTime = cosf(fTime)*0.5f + 0.5f;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pImmediateContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	VS_CONSTANT_BUFFER* pConstData = (VS_CONSTANT_BUFFER*)MappedResource.pData;
	pConstData->vColor = D3DXVECTOR4(cosf(fTime), sinf(fTime), 1.0f - cosf(fTime), 1.0f);
	pConstData->fTime = fBoundedTime;
	m_pImmediateContext->Unmap(m_pConstantBuffer, 0);
	return true;
}

bool dx01::Render()
{
	m_pImmediateContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pImmediateContext->PSSetShader(m_pPixelShader, NULL, 0);
	m_pImmediateContext->GSSetShader(NULL, NULL, 0);
	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

	UINT stride = sizeof(P3VERTEX);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->IASetPrimitiveTopology(m_PrimType);
	m_pImmediateContext->DrawIndexed(6, 0, 0);
	return true;
}

bool dx01::Release()
{
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);

	return true;
}

dx01::dx01()
{
	m_pVertexLayout = NULL;
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_pConstantBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_PrimType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

dx01::~dx01()
{
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	dx01 win;
	if (win.InitWindow(hInstance, L"BG Project", 1024, 768, TRUE) == true)
	{
		win.Run();
	}
	return 1;
}
