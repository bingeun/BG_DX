#include "bgWindow.h"

// ������ ���� ��������
bgWindow*	g_pWindow;
HWND		g_hWnd;
HINSTANCE	g_hInstance;



// ���� ���ν��� (�����Լ�)
LRESULT WINAPI MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	assert(g_pWindow);
	return g_pWindow->MsgProc(hWnd, msg, wParam, lParam);
}

// �޼��� ���ν��� (����Լ�)
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
		case '0': // Ǯ��ũ�� or ������
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

	// ������ Ŭ���� ��ü ���
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

	// Ŭ���̾�Ʈ ������ ����Ͽ� ������ ����
	m_iWindowW = m_iClientW + 2 * GetSystemMetrics(SM_CXFRAME);
	m_iWindowH = m_iClientH + (2 * GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION));
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"BG", titleName, m_dwStyle, 0, 0, m_iWindowW, m_iWindowH, NULL, NULL, hInstance, NULL);

	// ������� Ŭ���̾�Ʈ �� ������ ũ�⸦ ����
	GetWindowRect(m_hWnd, &m_rtWindow);
	GetClientRect(m_hWnd, &m_rtClient);

	// ȭ�鿡 ������ ���̱�
	if (m_hWnd != NULL)
	{
		if(bCenter)
			MoveCenter();
		ShowWindow(m_hWnd, SW_SHOW);
	}

	// �������� ����
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

	// �������� ����
	g_pWindow = this;
}

bgWindow::~bgWindow()
{
}
