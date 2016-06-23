#include "bgWindow.h"

// 윈도우 관련 전역변수
bgWindow*	g_pWindow;
HWND		g_hWnd;
HINSTANCE	g_hInstance;



// 메인 프로시져 (전역함수)
LRESULT WINAPI MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	assert(g_pWindow);
	return g_pWindow->MsgProc(hWnd, msg, wParam, lParam);
}

// 메세지 프로시져 (멤버함수)
LRESULT	bgWindow::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

bool bgWindow::InitWindow(HINSTANCE hInstance, TCHAR* titleName, int iWidth, int iHeight, BOOL bFullScreen, bool bCenter)
{
	m_hInstance = hInstance;
	m_iClientW = iWidth;
	m_iClientH = iHeight;

	// 윈도우 클래스 객체 등록
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.lpszClassName = L"BG";
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = MainProc;
	if (!RegisterClassEx(&wc))
		return false;

	// 클라이언트 영역을 계산하여 윈도우 생성
	m_iWindowW = m_iClientW + 2 * GetSystemMetrics(SM_CXFRAME);
	m_iWindowH = m_iClientH + (2 * GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION));
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"BG", titleName, m_dwStyle, 0, 0, m_iWindowW, m_iWindowH, NULL, NULL, hInstance, NULL);

	// 윈도우와 클라이언트 각 영역의 크기를 얻음
	GetWindowRect(m_hWnd, &m_rtWindow);
	GetClientRect(m_hWnd, &m_rtClient);

	// 화면에 윈도우 보이기
	if (m_hWnd != NULL)
	{
		if(bCenter)
			MoveCenter();
		ShowWindow(m_hWnd, SW_SHOW);
	}

	// 전역변수 설정
	g_hWnd = m_hWnd;
	g_hInstance = m_hInstance;
	return true;
}

void bgWindow::MoveCenter()
{
	m_iWindowX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (m_iClientW / 2) - GetSystemMetrics(SM_CXFRAME);
	m_iWindowY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (m_iClientH / 2) - ((2 * GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION)) / 2);
	m_iWindowW = m_iClientW + 2 * GetSystemMetrics(SM_CXFRAME);
	m_iWindowH = m_iClientH + (2 * GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION));

	MoveWindow(m_hWnd, m_iWindowX, m_iWindowY, m_iWindowW, m_iWindowH, true);
}

bool bgWindow::Run()
{
	MSG msg;

	GameInit();
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			MsgEvent(msg);
		}
		else
		{
			GameRun();
		}
	}
	GameRelease();
	return true;
}

bool bgWindow::Release()
{
	return true;
}

bgWindow::bgWindow()
{
	m_dwStyle = WS_BORDER | WS_MINIMIZEBOX | WS_SYSMENU; //WS_OVERLAPPED;
	m_bActivate = TRUE;

	// 전역변수 설정
	g_pWindow = this;
}

bgWindow::~bgWindow()
{
}
