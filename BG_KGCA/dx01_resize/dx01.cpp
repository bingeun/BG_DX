#include "dx01.h"



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

dx01::dx01()
{
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
