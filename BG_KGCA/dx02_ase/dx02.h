#pragma once
#include "bgCore.h"
#include "bgShapeGuideAxis.h"
#include "objMob.h"

#define MAX_MOB 3

class dx02 : public bgCore
{
public:
	bgShapeGuideAxis	m_objWorldAxis;
	objMob				m_objMobList[MAX_MOB];
	objMob				m_objHero;

public:
	dx02();
	virtual ~dx02();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	LRESULT CALLBACK AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
