#pragma once
#include "bgStd.h"
#include "bgDevice.h"

class bgWindow : public bgDevice
{
public:
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;
	RECT		m_rtWindow;
	RECT		m_rtClient;
	DWORD		m_dwStyle;
	BOOL		m_bActivate;

	int			m_iScreenW;
	int			m_iScreenH;

	int			m_iClientW;
	int			m_iClientH;

	int			m_iWindowX;
	int			m_iWindowY;
	int			m_iWindowW;
	int			m_iWindowH;

public:
	bool	InitWindow(HINSTANCE hInstance, TCHAR* titleName = L"BG Project", int iWidth = 800, int iHeight = 600, BOOL bFullScreen = FALSE, bool bCenter = true);
	void	SetWindowSize(int iWidth, int iHeight, DWORD dwStyle);
	void	SetPosCenter();
	void	MoveCenter();
	
public:
	bool	Run();
	bool	Release();

	virtual bool	GameRun() { return true; }
	virtual bool	GameInit() { return true; }
	virtual bool	GameFrame() { return true; }
	virtual bool	GameRender() { return true; }
	virtual bool	GameRelease() { return true; }
	virtual void	MsgEvent(MSG msg) { return; }
	virtual LRESULT	MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	bgWindow();
	virtual ~bgWindow();
};
