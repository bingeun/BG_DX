#pragma once
#include "bgStd.h"

LRESULT	CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class bgWindow
{
public:
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;
	DWORD		m_dwStyle;		// 윈도우 스타일
	DWORD		m_dwStyleEx;	// 윈도우 스타일(확장)
	RECT		m_rtWindow;		// 윈도우 상하좌우 위치값
	int			m_iScreenW, m_iScreenH; // 화면 크기
	int			m_iClientW, m_iClientH; // 클라이언트 크기
	int			m_iWindowW, m_iWindowH; // 윈도우 크기
	int			m_iWindowX, m_iWindowY; // 윈도우 위치
	bool		m_bSizeLarge;	// 윈도우 크기 (1024x768 <=> 800x600)
	bool		m_bFullScreen;	// 윈도우 <=> 전체화면 모드변환
	bool		m_bActive;		// 윈도우 활성상태

public:
	bgWindow();
	virtual ~bgWindow();

public:
	bool	AppRun();
	virtual LRESULT CALLBACK AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool	InitWindow(HINSTANCE hInstance, TCHAR* pszTitle = L"BG Project!", int iWidth = 800, int iHeight = 600, BOOL bFullScreen = FALSE, bool bCenter = true);
	bool	SetWindowSize(HWND hWnd, int iWidth, int iHeight);
	
public:
	virtual bool	GameRun() { return true; }
	virtual bool	GameInit() { return true; }
	virtual bool	GameFrame() { return true; }
	virtual bool	GameRender() { return true; }
	virtual bool	GameRelease() { return true; }
};
