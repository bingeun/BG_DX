#pragma once


// bgSaveMapT ��ȭ �����Դϴ�.

class bgSaveMapT : public CDialogEx
{
	DECLARE_DYNAMIC(bgSaveMapT)

public:
	bgSaveMapT(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~bgSaveMapT();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SAVE_T };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
