#pragma once
#include "bgCore.h"
#include "bgShapeGuideAxis.h"
#include "bgShapeBox.h"
#include "bgQuadTree.h"

class dx04 : public bgCore
{
public:
	bgShapeGuideAxis	m_objWorldAxis;
	vector<bgShapeBox>	m_objBox;

public:
	dx04();
	virtual ~dx04();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	LRESULT CALLBACK AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
