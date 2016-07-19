#include "dx01.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	dx01 core;
	if (core.InitWindow(hInstance, L"BG Project!", 800, 600))
	{
		core.AppRun();
	}
	return 1;
}

dx01::dx01()
{
}

dx01::~dx01()
{
}

bool dx01::Init()
{
	m_objBox.Init();
	m_objBox.Set(m_pDevice, m_pDeviceContext);
	m_objBox.Create();
	m_objBox.Load();

	m_objPlane.Init();
	m_objPlane.Set(m_pDevice, m_pDeviceContext);
	m_objPlane.Create();
	m_objPlane.Load();

	m_objGuideAxis.Init();
	m_objGuideAxis.Set(m_pDevice, m_pDeviceContext);
	m_objGuideAxis.Create();
	m_objGuideAxis.Load();
	return true;
}

bool dx01::Frame()
{
	return true;
}

bool dx01::Render()
{
	m_objBox.Render();
	m_objPlane.Render();
	m_objGuideAxis.Render();
	return true;
}

bool dx01::Release()
{
	return true;
}

LRESULT CALLBACK dx01::AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case '1': // 윈도우 크기 변경 (1024x768 <=> 800x600)
		{
			if (m_pSwapChain && !m_bFullScreen)
			{
				m_bSizeLarge = !m_bSizeLarge;
				if (m_bSizeLarge)
					g_pWindow->SetWindowSize(m_hWnd, 1024, 768);
				else
					g_pWindow->SetWindowSize(m_hWnd, 800, 600);

				DXGI_MODE_DESC desc;
				ZeroMemory(&desc, sizeof(desc));
				desc.Width = (m_bSizeLarge) ? 1024 : 800;
				desc.Height = (m_bSizeLarge) ? 768 : 600;
				desc.RefreshRate.Numerator = 0;
				desc.RefreshRate.Denominator = 0;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				m_pSwapChain->ResizeTarget(&desc);
			}
		}
		break;

		case '0': // 윈도우 <=> 전체화면 모드변경
		{
			if (m_pSwapChain)
			{
				m_bFullScreen = !m_bFullScreen;
				if (m_bFullScreen)
					m_pSwapChain->SetFullscreenState(TRUE, NULL);
				else
					m_pSwapChain->SetFullscreenState(FALSE, NULL);
			}
		}
		break;

		case VK_ESCAPE:
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
		}
	}
	break;

	default:
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	break;
	}
}
