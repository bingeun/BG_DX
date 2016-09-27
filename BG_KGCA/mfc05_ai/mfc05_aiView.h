
// mfc05_aiView.h : Cmfc05_aiView Ŭ������ �������̽�
//

#pragma once


class Cmfc05_aiView : public CView
{
protected: // serialization������ ��������ϴ�.
	Cmfc05_aiView();
	DECLARE_DYNCREATE(Cmfc05_aiView)

// Ư���Դϴ�.
public:
	Cmfc05_aiDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~Cmfc05_aiView();
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
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // mfc05_aiView.cpp�� ����� ����
inline Cmfc05_aiDoc* Cmfc05_aiView::GetDocument() const
   { return reinterpret_cast<Cmfc05_aiDoc*>(m_pDocument); }
#endif

