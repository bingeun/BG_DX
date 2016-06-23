#pragma once
#include "bgWindow.h"

// bgWindow - 윈도우 관련 전역변수
extern bgWindow*	g_pWindow;
extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;

// bgTimer - 타이머 관련 전역변수
extern int			g_iFPS;
extern float		g_fSPF;
extern float		g_fCurrent;

// bgInput - 입력 관련 전역변수
//extern BGINPUT_MAP	g_InputData;
