#include "bgWindow.h"

bgWindow*	g_pWindow;
HINSTANCE	g_hInstance;
HWND		g_hWnd;

bgWindow::bgWindow()
{
	g_pWindow = this;
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

	// 프로그램 실행중 루프
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

	GameRelease();

	return true;
}

bool bgWindow::InitWindow(HINSTANCE hInstance, TCHAR* pszTitle, int iWidth, int iHeight, BOOL bFullScreen, bool bCenter)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = pszTitle;
	wc.hIconSm = wc.hIcon;
	RegisterClassEx(&wc);

	m_dwStyle = WS_OVERLAPPED;
	m_dwStyleEx = WS_EX_APPWINDOW;
	m_hWnd = CreateWindowEx(m_dwStyleEx, pszTitle, pszTitle, m_dwStyle, 0, 0, iWidth, iHeight, NULL, NULL, hInstance, NULL);
	if (m_hWnd == NULL)
		return false;

	SetWindowSize(m_hWnd, iWidth, iHeight);
	ShowWindow(m_hWnd, SW_SHOW);

	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	//ShowCursor(false);

	g_hInstance = m_hInstance = hInstance;
	g_hWnd = m_hWnd;

	return true;
}

bool bgWindow::SetWindowSize(HWND hWnd, int iWidth, int iHeight)
{
	if (hWnd == NULL)
		return false;

	m_iScreenW = GetSystemMetrics(SM_CXSCREEN);
	m_iScreenH = GetSystemMetrics(SM_CYSCREEN);
	m_iClientW = iWidth;
	m_iClientH = iHeight;

	bool bMenu = (GetMenu(hWnd) != NULL);
	m_rtWindow = { 0, 0, iWidth, iHeight };

	AdjustWindowRectEx(&m_rtWindow, m_dwStyle, bMenu, m_dwStyleEx);
	
	m_rtWindow.right += (m_dwStyle & WS_VSCROLL) ? GetSystemMetrics(SM_CXVSCROLL) : 0;
	m_rtWindow.bottom += (m_dwStyle & WS_HSCROLL) ? GetSystemMetrics(SM_CYVSCROLL) : 0;
	m_iWindowW = m_rtWindow.right - m_rtWindow.left;
	m_iWindowH = m_rtWindow.bottom - m_rtWindow.top;

	SetWindowPos(hWnd, NULL, m_rtWindow.left, m_rtWindow.top, m_iWindowW, m_iWindowH, NULL);
	return true;
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
