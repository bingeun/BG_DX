
// KGCA_CharToolView.h : CKGCA_CharToolView Ŭ������ �������̽�
//

#pragma once


class CKGCA_CharToolView : public CView
{
protected: // serialization������ ��������ϴ�.
	CKGCA_CharToolView();
	DECLARE_DYNCREATE(CKGCA_CharToolView)

// Ư���Դϴ�.
public:
	CKGCA_CharToolDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// �����Դϴ�.
public:
	virtual ~CKGCA_CharToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // KGCA_CharToolView.cpp�� ����� ����
inline CKGCA_CharToolDoc* CKGCA_CharToolView::GetDocument() const
   { return reinterpret_cast<CKGCA_CharToolDoc*>(m_pDocument); }
#endif

