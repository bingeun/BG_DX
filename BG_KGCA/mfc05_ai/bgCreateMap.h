#pragma once


// bgCreateMap 대화 상자입니다.

class bgCreateMap : public CDialogEx
{
	DECLARE_DYNAMIC(bgCreateMap)

public:
	bgCreateMap(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~bgCreateMap();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_MAP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
