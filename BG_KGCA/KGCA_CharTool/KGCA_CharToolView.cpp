
// KGCA_CharToolView.cpp : CKGCA_CharToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "KGCA_CharTool.h"
#endif

#include "KGCA_CharToolDoc.h"
#include "KGCA_CharToolView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKGCA_CharToolView

IMPLEMENT_DYNCREATE(CKGCA_CharToolView, CView)

BEGIN_MESSAGE_MAP(CKGCA_CharToolView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CKGCA_CharToolView ����/�Ҹ�

CKGCA_CharToolView::CKGCA_CharToolView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CKGCA_CharToolView::~CKGCA_CharToolView()
{
}

BOOL CKGCA_CharToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CKGCA_CharToolView �׸���

void CKGCA_CharToolView::OnDraw(CDC* /*pDC*/)
{
	CKGCA_CharToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}

void CKGCA_CharToolView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CKGCA_CharToolView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CKGCA_CharToolView ����

#ifdef _DEBUG
void CKGCA_CharToolView::AssertValid() const
{
	CView::AssertValid();
}

void CKGCA_CharToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKGCA_CharToolDoc* CKGCA_CharToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKGCA_CharToolDoc)));
	return (CKGCA_CharToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CKGCA_CharToolView �޽��� ó����
