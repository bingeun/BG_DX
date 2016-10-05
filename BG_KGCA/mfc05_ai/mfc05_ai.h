
// mfc05_ai.h : mfc05_ai ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.

#include "bgCore.h"
#include "bgParserBG3D.h"
#include "bgModelBG3D.h"
#include "bgTextureMgr.h"
#include "bgShapeGuideAxis.h"
#include "bgShapePlane.h"
#include "bgShapeBox.h"
#include "bgBoundBox.h"

// Cmfc05_aiApp:
// �� Ŭ������ ������ ���ؼ��� mfc05_ai.cpp�� �����Ͻʽÿ�.
//

class Cmfc05_aiApp : public CWinAppEx, public bgCore
{
public:
	bgShapeGuideAxis	m_objWorldAxis;

	bgParserBG3D	m_ParserBG3D;
	bgModelBG3D		m_Model;
	bgModelBG3D		m_Model2;
	bgBoundBox		m_BoundBox;

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();



public:
	Cmfc05_aiApp();


	// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// �����Դϴ�.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern Cmfc05_aiApp theApp;
