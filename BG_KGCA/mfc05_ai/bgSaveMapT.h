#pragma once


// bgSaveMapT 대화 상자입니다.

class bgSaveMapT : public CDialogEx
{
	DECLARE_DYNAMIC(bgSaveMapT)

public:
	bgSaveMapT(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~bgSaveMapT();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SAVE_T };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
