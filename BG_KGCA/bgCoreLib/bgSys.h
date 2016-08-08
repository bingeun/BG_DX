#pragma once
#include "bgWindow.h"
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
extern ID3D11Device*	g_pDevice;

// bgShape
extern MATRIX_BUFFER	g_MatrixBuffer;
