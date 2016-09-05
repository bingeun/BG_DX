#pragma once
#include "bgCore.h"
#include "bgParserASE.h"
#include "bgModel.h"
#include "bgTextureMgr.h"
#include "bgShapeGuideAxis.h"
#include "bgShapePlane.h"
#include "bgShapeBox.h"

class dx03 : public bgCore
{
public:
	bgShapeGuideAxis	m_objWorldAxis;
	bgShapePlane		m_objWorldPlane;
	bgShapeBox			m_objWorldBox;

	bgParserASE		m_ParserASE;
	bgModel			m_ModelShip;

public:
	dx03();
	virtual ~dx03();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	LRESULT CALLBACK AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
