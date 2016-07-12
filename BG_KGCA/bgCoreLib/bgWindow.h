#pragma once
#include "bgStd.h"

class bgWindow
{
public:
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;
	DWORD		m_dwStyle;

	int			m_iWindowX, m_iWindowY;
	int			m_iWindowW, m_iWindowH;
	int			m_iClientW, m_iClientH;
	int			m_iScreenW, m_iScreenH;

	RECT		m_rtWindow;
	RECT		m_rtClient;

public:
	bgWindow();
	virtual ~bgWindow();

public:
	bool	AppRun();
	virtual LRESULT CALLBACK	AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool	InitWindow(HINSTANCE hInstance, TCHAR* pszTitle = L"BG Project!", int iWidth = 800, int iHeight = 600, BOOL bFullScreen = FALSE, bool bCenter = true);
	void	SetWindowSize(int iWidth, int iHeight, DWORD dwStyle);
	
public:
	virtual bool	GameRun() { return true; }
	virtual bool	GameInit() { return true; }
	virtual bool	GameFrame() { return true; }
	virtual bool	GameRender() { return true; }
	virtual bool	GameRelease() { return true; }
};
