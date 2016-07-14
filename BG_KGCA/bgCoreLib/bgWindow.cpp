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

	m_dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX; // | WS_MAXIMIZEBOX; // | WS_THICKFRAME;
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

	// 윈도우 스타일 얻기
	bool bMenu = (GetMenu(hWnd) != NULL);
	m_dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	m_dwStyleEx = GetWindowLong(hWnd, GWL_EXSTYLE);

	// 스크린 크기 계산
	m_iScreenW = GetSystemMetrics(SM_CXSCREEN);
	m_iScreenH = GetSystemMetrics(SM_CYSCREEN);

	// 클라이언트 영역 크기(인자값) 설정
	m_iClientW = iWidth;
	m_iClientH = iHeight;

	m_rtWindow = { 0, 0, iWidth, iHeight };
	AdjustWindowRectEx(&m_rtWindow, m_dwStyle, bMenu, m_dwStyleEx);

	// 윈도우의 크기(W, H) 먼저 계산
	m_rtWindow.bottom += (m_dwStyle & WS_CAPTION) ? GetSystemMetrics(SM_CYCAPTION) : 0;
	m_rtWindow.right += (m_dwStyle & WS_BORDER) ? GetSystemMetrics(SM_CXBORDER) : 0;
	m_rtWindow.bottom += (m_dwStyle & WS_BORDER) ? GetSystemMetrics(SM_CYBORDER) : 0;
	m_rtWindow.right += (m_dwStyle & WS_DLGFRAME) ? GetSystemMetrics(SM_CXDLGFRAME) : 0;
	m_rtWindow.bottom += (m_dwStyle & WS_DLGFRAME) ? GetSystemMetrics(SM_CYDLGFRAME) : 0;

	///////////////////////////////////////
	// -1 필요여부...? ★★★★★★★★ 윈도우 스타일에서 잘못 계산되었는지 확인필요
	////////////////////////
	m_iWindowW = m_rtWindow.right - 1;
	m_iWindowH = m_rtWindow.bottom - 1;

	// 계산된 크기로 스크린에 출력될 위치(X, Y) 계산
	m_rtWindow.left = (m_iScreenW - m_iWindowW) / 2;
	m_rtWindow.top = (m_iScreenH - m_iWindowH) / 2;
	m_iWindowX = m_rtWindow.left;
	m_iWindowY = m_rtWindow.top;

	SetWindowPos(hWnd, NULL, m_iWindowX, m_iWindowY, m_iWindowW, m_iWindowH, NULL);
	return true;
}

// virtual 함수
LRESULT CALLBACK bgWindow::AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	default:
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	break;
	}
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
