#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgWindow.h"

class dx01 : public bgWindow
{
public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	dx01();
	virtual ~dx01();
};
