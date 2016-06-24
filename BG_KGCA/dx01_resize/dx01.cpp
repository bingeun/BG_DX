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
		case VK_UP:
		{

		}
		break;
		case VK_DOWN:
		{

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

dx01::dx01()
{
}

dx01::~dx01()
{
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	dx01 win;
	if (win.InitWindow(hInstance, L"BG Project", 800, 600, TRUE) == true)
	{
		win.Run();
	}
	return 1;
}
