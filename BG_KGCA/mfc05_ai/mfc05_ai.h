
// mfc05_ai.h : mfc05_ai 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.

#include "bgCore.h"
#include "bgParserBGSKN.h"
#include "bgParserBGMTX.h"
#include "bgModel.h"
#include "bgAnimation.h"
#include "bgTextureMgr.h"
#include "bgShapeGuideAxis.h"

// Cmfc05_aiApp:
// 이 클래스의 구현에 대해서는 mfc05_ai.cpp을 참조하십시오.
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


	// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// 구현입니다.
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
