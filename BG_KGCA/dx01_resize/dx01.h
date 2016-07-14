#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgCore.h"

class dx01 : public bgCore
{
public:
	dx01();
	virtual ~dx01();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	LRESULT CALLBACK AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
