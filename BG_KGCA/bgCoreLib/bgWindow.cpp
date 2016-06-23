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
	SetWindowSize(m_iClientW, m_iClientH, m_dwStyle);
	SetPosCenter();
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"BG", titleName, m_dwStyle, m_iWindowX, m_iWindowY, m_iWindowW, m_iWindowH, NULL, NULL, hInstance, NULL);

	// ������� Ŭ���̾�Ʈ �� ������ ũ�⸦ ����
	GetWindowRect(m_hWnd, &m_rtWindow);
	GetClientRect(m_hWnd, &m_rtClient);

	// ȭ�鿡 ������ ���̱�
	if (m_hWnd != NULL)
	{
		//if(bCenter)
		//	MoveCenter();
		ShowWindow(m_hWnd, SW_SHOW);
	}

	// �������� ����
	g_hWnd = m_hWnd;
	g_hInstance = m_hInstance;
	return true;
}

void bgWindow::SetWindowSize(int iWidth, int iHeight, DWORD dwStyle)
{
	// Ŭ���̾�Ʈ ũ��� �ʱⰪ ����
	m_iWindowW = m_iClientW;
	m_iWindowH = m_iClientH;

	// �׵θ� ũ�⸸ŭ ���ϱ�
	if (m_dwStyle & WS_BORDER)
	{
		m_iWindowW += GetSystemMetrics(SM_CXBORDER) * 2;
		m_iWindowH += GetSystemMetrics(SM_CYBORDER) * 2;

		m_iWindowW += GetSystemMetrics(SM_CXFRAME) * 2;
		m_iWindowH += GetSystemMetrics(SM_CYFRAME) * 2;

		m_iWindowW += GetSystemMetrics(SM_CXDLGFRAME) * 2;
		m_iWindowH += GetSystemMetrics(SM_CYDLGFRAME) * 2;
	}

	// Ÿ��Ʋ�� ũ�⸸ŭ ���ϱ�
	if (m_dwStyle & (WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU))
		m_iWindowH += GetSystemMetrics(SM_CYCAPTION);
}

void bgWindow::SetPosCenter()
{
	// ��üȭ�� ũ�� ���ϱ�
	m_iWindowX = m_iScreenW = GetSystemMetrics(SM_CXSCREEN);
	m_iWindowY = m_iScreenH = GetSystemMetrics(SM_CYSCREEN);

	// Ŭ���̾�Ʈ ������ŭ ����
	m_iWindowX -= m_iClientW;
	m_iWindowY -= m_iClientH;

	// �׵θ� ũ�⸸ŭ ����
	if (m_dwStyle & WS_BORDER)
	{
		m_iWindowX -= GetSystemMetrics(SM_CXBORDER);
		m_iWindowY -= GetSystemMetrics(SM_CYBORDER);

		m_iWindowX -= GetSystemMetrics(SM_CXFRAME);
		m_iWindowY -= GetSystemMetrics(SM_CYFRAME);

		m_iWindowX -= GetSystemMetrics(SM_CXDLGFRAME);
		m_iWindowY -= GetSystemMetrics(SM_CYDLGFRAME);
	}
	
	// Ÿ��Ʋ�� ũ�⸸ŭ ����
	if (m_dwStyle & (WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU))
		m_iWindowY -= GetSystemMetrics(SM_CYCAPTION);
	
	// �������� 2�� ������
	m_iWindowX /= 2;
	m_iWindowY /= 2;

	// �̹� ������ �������� ��ǥ�� ũ�⸦ ����
	//MoveWindow(m_hWnd, m_iWindowX, m_iWindowY, m_iWindowW, m_iWindowH, true);
}

void bgWindow::MoveCenter()
{
	SetWindowSize(m_iClientW, m_iClientH, m_dwStyle);
	SetPosCenter();
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
	m_dwStyle = WS_BORDER | WS_MINIMIZEBOX | WS_SYSMENU;
	m_bActivate = TRUE;

	m_iScreenW = GetSystemMetrics(SM_CXSCREEN);
	m_iScreenH = GetSystemMetrics(SM_CYSCREEN);

	// �������� ����
	g_pWindow = this;
}

bgWindow::~bgWindow()
{
}
