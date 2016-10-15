
// mfc05_ai.h : mfc05_ai ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.

#include "bgCore.h"
#include "bgParserBGSKN.h"
#include "bgParserBGMTX.h"
#include "bgModel.h"
#include "bgAnimation.h"
#include "bgTextureMgr.h"
#include "bgShapeGuideAxis.h"

// Cmfc05_aiApp:
// �� Ŭ������ ������ ���ؼ��� mfc05_ai.cpp�� �����Ͻʽÿ�.
//

class Cmfc05_aiApp : public CWinAppEx, public bgCore
{
public:
	bgShapeGuideAxis	m_GuideAxis;
	bgParserBGSKN	m_ParserSKN;
	bgParserBGMTX	m_ParserMTX;
	bgModel			m_Model;
	bgAnimation		m_Animation;

	float			m_MoveSpeed;

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
