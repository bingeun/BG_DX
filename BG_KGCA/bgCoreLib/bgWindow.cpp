#include "bgWindow.h"

bgWindow*	g_pWindow;
HINSTANCE	g_hInstance;
HWND		g_hWnd;

bgWindow::bgWindow()
{
	return;
}

bgWindow::~bgWindow()
{
	return;
}

bool bgWindow::AppRun()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	GameInit();

	// =============== 프로그램 실행 루프 ===============
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			GameRun();
		}
	}
	// ================================================

	GameRelease();

	return true;
}

bool bgWindow::InitWindow(HINSTANCE hInstance, TCHAR* pszTitle, int iWidth, int iHeight, BOOL bFullScreen, bool bCenter)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = pszTitle;
	wc.hIconSm = wc.hIcon;
	RegisterClassEx(&wc);

	m_iWindowX = 0;
	m_iWindowY = 0;
	m_iWindowW = 0;
	m_iWindowH = 0;
	m_iClientW = 0;
	m_iClientH = 0;
	m_iScreenW = GetSystemMetrics(SM_CXSCREEN);
	m_iScreenH = GetSystemMetrics(SM_CYSCREEN);

	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, pszTitle, pszTitle, WS_OVERLAPPED,
							m_iWindowX, m_iWindowY, m_iWindowW, m_iWindowH, NULL, NULL, hInstance, NULL);

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	// 마우스 숨기기
	//ShowCursor(false);

	g_hInstance = m_hInstance = hInstance;
	g_hWnd = m_hWnd;
	g_pWindow = this;

	return true;
}

void bgWindow::SetWindowSize(int iWidth, int iHeight, DWORD dwStyle)
{
	return;
}

LRESULT CALLBACK bgWindow::AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// 전역 프로시져 함수
LRESULT	CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;

	default:
	{
		return g_pWindow->AppProc(hWnd, uMsg, wParam, lParam);
	}
	break;
	}
}
