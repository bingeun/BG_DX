#pragma once


// bgSaveMapB ��ȭ �����Դϴ�.

class bgSaveMapB : public CDialogEx
{
	DECLARE_DYNAMIC(bgSaveMapB)

public:
	bgSaveMapB(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~bgSaveMapB();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SAVE_B };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
