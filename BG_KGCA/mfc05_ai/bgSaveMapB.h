#pragma once


// bgSaveMapB 대화 상자입니다.

class bgSaveMapB : public CDialogEx
{
	DECLARE_DYNAMIC(bgSaveMapB)

public:
	bgSaveMapB(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~bgSaveMapB();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SAVE_B };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
