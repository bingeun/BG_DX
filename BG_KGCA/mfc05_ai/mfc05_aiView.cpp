
// mfc05_aiView.cpp : Cmfc05_aiView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "mfc05_ai.h"
#endif

#include "mfc05_aiDoc.h"
#include "mfc05_aiView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cmfc05_aiView

IMPLEMENT_DYNCREATE(Cmfc05_aiView, CView)

BEGIN_MESSAGE_MAP(Cmfc05_aiView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Cmfc05_aiView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// Cmfc05_aiView ����/�Ҹ�

Cmfc05_aiView::Cmfc05_aiView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

Cmfc05_aiView::~Cmfc05_aiView()
{
}

BOOL Cmfc05_aiView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// Cmfc05_aiView �׸���

void Cmfc05_aiView::OnDraw(CDC* /*pDC*/)
{
	Cmfc05_aiDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// Cmfc05_aiView �μ�


void Cmfc05_aiView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL Cmfc05_aiView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void Cmfc05_aiView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void Cmfc05_aiView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}

void Cmfc05_aiView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Cmfc05_aiView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// Cmfc05_aiView ����

#ifdef _DEBUG
void Cmfc05_aiView::AssertValid() const
{
	CView::AssertValid();
}

void Cmfc05_aiView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cmfc05_aiDoc* Cmfc05_aiView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cmfc05_aiDoc)));
	return (Cmfc05_aiDoc*)m_pDocument;
}
#endif //_DEBUG


// Cmfc05_aiView �޽��� ó����


void Cmfc05_aiView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// ==============================================================================================================

	Cmfc05_aiApp* pApp = (Cmfc05_aiApp*)AfxGetApp();
	if (cx != 0 && cy != 0)
	{
		pApp->ResizeClient(cx, cy);
	}
}


void Cmfc05_aiView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// ==============================================================================================================
	
	Cmfc05_aiApp* pApp = (Cmfc05_aiApp*)AfxGetApp();
	switch (nChar)
	{
	case '2': // Ű���� ���� ��¿��� ����
	{
		if (pApp->m_DWrite.m_bFontBorder)
			pApp->m_bPrintKeyState = !pApp->m_bPrintKeyState;
		pApp->m_DWrite.m_bFontBorder = !pApp->m_DWrite.m_bFontBorder;
	}
	break;

	case '3': // ���̾������� <=> �ָ��� ��庯��
	{

	}
	break;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
