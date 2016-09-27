
// mfc05_aiView.cpp : Cmfc05_aiView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Cmfc05_aiView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// Cmfc05_aiView 생성/소멸

Cmfc05_aiView::Cmfc05_aiView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

Cmfc05_aiView::~Cmfc05_aiView()
{
}

BOOL Cmfc05_aiView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// Cmfc05_aiView 그리기

void Cmfc05_aiView::OnDraw(CDC* /*pDC*/)
{
	Cmfc05_aiDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// Cmfc05_aiView 인쇄


void Cmfc05_aiView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL Cmfc05_aiView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void Cmfc05_aiView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void Cmfc05_aiView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
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


// Cmfc05_aiView 진단

#ifdef _DEBUG
void Cmfc05_aiView::AssertValid() const
{
	CView::AssertValid();
}

void Cmfc05_aiView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cmfc05_aiDoc* Cmfc05_aiView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cmfc05_aiDoc)));
	return (Cmfc05_aiDoc*)m_pDocument;
}
#endif //_DEBUG


// Cmfc05_aiView 메시지 처리기


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
	case '2': // 키눌림 상태 출력여부 변경
	{
		if (pApp->m_DWrite.m_bFontBorder)
			pApp->m_bPrintKeyState = !pApp->m_bPrintKeyState;
		pApp->m_DWrite.m_bFontBorder = !pApp->m_DWrite.m_bFontBorder;
	}
	break;

	case '3': // 와이어프레임 <=> 솔리드 모드변경
	{

	}
	break;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
