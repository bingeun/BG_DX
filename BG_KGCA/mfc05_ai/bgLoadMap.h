#pragma once


// bgLoadMap 대화 상자입니다.

class bgLoadMap : public CDialogEx
{
	DECLARE_DYNAMIC(bgLoadMap)

public:
	bgLoadMap(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~bgLoadMap();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOAD_MAP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
