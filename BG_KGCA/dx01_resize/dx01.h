#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgCore.h"
#include "bgTextureMgr.h"
#include "bgShapeGuideAxis.h"
#include "bgShapePlane.h"
#include "bgShapeBox.h"

class dx01 : public bgCore
{
public:
	bgShapeGuideAxis	m_objWorldAxis;
	bgShapePlane		m_objWorldPlane;
	bgShapeBox			m_objWorldBox;
	
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
