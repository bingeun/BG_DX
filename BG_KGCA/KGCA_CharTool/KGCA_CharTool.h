
// KGCA_CharTool.h : KGCA_CharTool ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CKGCA_CharToolApp:
// �� Ŭ������ ������ ���ؼ��� KGCA_CharTool.cpp�� �����Ͻʽÿ�.
//

class CKGCA_CharToolApp : public CWinAppEx
{
public:
	CKGCA_CharToolApp();


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
};

extern CKGCA_CharToolApp theApp;
