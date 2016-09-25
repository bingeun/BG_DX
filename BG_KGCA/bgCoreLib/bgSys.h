#pragma once
#include "bgWindow.h"
#include "bgDevice.h"
#include "bgShape.h"

// bgWindow
extern bgWindow*	g_pWindow;
extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;

// bgTimer
extern int			g_iFPS;
extern float		g_fSPF;
extern float		g_fCurrent;

// bgDevice
extern bgDevice*	g_pDevice;

// bgShape
extern MATRIX_BUFFER*	g_pCameraMatrix;
