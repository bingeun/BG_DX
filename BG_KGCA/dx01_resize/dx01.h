#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgCore.h"
#include "bgShapeGuideAxis.h"
#include "bgShapePlane.h"
#include "bgShapeBox.h"

class dx01 : public bgCore
{
public:
	bgShapeGuideAxis	m_objGuideAxis;
	bgShapePlane		m_objPlane;
	bgShapeBox			m_objBox;

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
