#pragma once


// bgCreateMap ��ȭ �����Դϴ�.

class bgCreateMap : public CDialogEx
{
	DECLARE_DYNAMIC(bgCreateMap)

public:
	bgCreateMap(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~bgCreateMap();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_MAP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
