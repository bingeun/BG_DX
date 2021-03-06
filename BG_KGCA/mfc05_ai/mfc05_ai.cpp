
// mfc05_ai.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "mfc05_ai.h"
#include "MainFrm.h"

#include "mfc05_aiDoc.h"
#include "mfc05_aiView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Cmfc05_aiApp

BEGIN_MESSAGE_MAP(Cmfc05_aiApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &Cmfc05_aiApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// Cmfc05_aiApp 생성

Cmfc05_aiApp::Cmfc05_aiApp()
{
	m_bHiColorIcons = TRUE;

	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 응용 프로그램을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
	//     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
	//     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("mfc05_ai.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 Cmfc05_aiApp 개체입니다.

Cmfc05_aiApp theApp;


// Cmfc05_aiApp 초기화

BOOL Cmfc05_aiApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면  AfxInitRichEdit2()가 있어야 합니다.	
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다.  문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(Cmfc05_aiDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(Cmfc05_aiView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// ===========================================================================================================================

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	Cmfc05_aiView* pView = (Cmfc05_aiView*)pFrame->GetActiveView();

	g_hInstance = bgWindow::m_hInstance = AfxGetInstanceHandle();
	g_hWnd = bgWindow::m_hWnd = pView->m_hWnd;
	I_DInput.m_hWnd = pFrame->m_hWnd;

	CRect rcClient;
	pView->GetClientRect(rcClient);
	bgWindow::m_iClientW = rcClient.Width();
	bgWindow::m_iClientH = rcClient.Height();

	bgCore::GameInit();
	ResizeClient(bgWindow::m_iClientW, bgWindow::m_iClientH);

	// ===========================================================================================================================

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

int Cmfc05_aiApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	AfxOleTerm(FALSE);

	// ===========================================================================================================================

	GameRelease();

	return CWinAppEx::ExitInstance();
}

// Cmfc05_aiApp 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

														// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void Cmfc05_aiApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// Cmfc05_aiApp 사용자 지정 로드/저장 메서드

void Cmfc05_aiApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void Cmfc05_aiApp::LoadCustomState()
{
}

void Cmfc05_aiApp::SaveCustomState()
{
}

// Cmfc05_aiApp 메시지 처리기





BOOL Cmfc05_aiApp::OnIdle(LONG lCount)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	__super::OnIdle(lCount);

	// ==========================================================================================================

	GameFrame();
	GameRender();

	// ==========================================================================================================

	return TRUE;
}







bool Cmfc05_aiApp::Init()
{
	m_GuideAxis.Init();
	m_GuideAxis.SetDevice(m_pDevice, m_pDContext, m_pRSWireNone, m_Camera.m_pMatrixBuffer);
	m_GuideAxis.CreateBuffer(1000.0f);
	m_GuideAxis.LoadShader();

	m_Model.Init();
	m_Model.SetDevice(m_pDevice, m_pDContext, m_pRSSolidFront, m_Camera.m_pMatrixBuffer);
	m_Model.m_pObjList.resize(3);

	m_Model.m_pObjList[0] = new bgObject;
	m_ParserSKN.Set(m_Model.m_pObjList[0]);
	m_ParserSKN.Open(_T("../../data/model/mob_body.BGSKN"));

	m_Model.m_pObjList[1] = new bgObject;
	m_ParserSKN.Set(m_Model.m_pObjList[1]);
	m_ParserSKN.Open(_T("../../data/model/mob_left.BGSKN"));

	m_Model.m_pObjList[2] = new bgObject;
	m_ParserSKN.Set(m_Model.m_pObjList[2]);
	m_ParserSKN.Open(_T("../../data/model/mob_right.BGSKN"));

	m_Animation.m_NodeList.resize(1);
	m_Model.m_pAnimList.resize(1);
	m_Model.m_pAnimList[0] = &m_Animation;
	m_ParserMTX.Set(&m_Animation);
	m_ParserMTX.Open(_T("../../data/model/mob.BGMTX"));

	m_Model.CreateBuffer();
	m_Model.LoadShader("VS", "PS_Tex");

	m_Model.m_iTexID = I_TextureMgr.Add(_T("../../data/model/mob04.bmp"));

	m_MoveSpeed = 2.0f;

	return true;
}

bool Cmfc05_aiApp::Frame()
{
	// 프로그램 종료
	if (I_DInput.IsKeyDown(DIK_ESCAPE))
	{
		m_pMainWnd->SendMessage(WM_CLOSE, 0, 0);
	}
	// 카메라 이동
	if (I_DInput.IsKeyDown(DIK_HOME))
	{
		m_Camera.MoveForward();
	}
	if (I_DInput.IsKeyDown(DIK_END))
	{
		m_Camera.MoveBackward();
	}
	if (I_DInput.IsKeyDown(DIK_DELETE))
	{
		m_Camera.MoveLeft();
	}
	if (I_DInput.IsKeyDown(DIK_PGDN))
	{
		m_Camera.MoveRight();
	}
	if (I_DInput.IsKeyDown(DIK_PGUP))
	{
		m_Camera.MoveDown();
	}
	if (I_DInput.IsKeyDown(DIK_INSERT))
	{
		m_Camera.MoveUp();
	}
	// 카메라 이동
	if (I_DInput.IsKeyDown(DIK_W))
	{
		m_Camera.MoveForward(m_MoveSpeed);
	}
	if (I_DInput.IsKeyDown(DIK_S))
	{
		m_Camera.MoveBackward(m_MoveSpeed);
	}
	if (I_DInput.IsKeyDown(DIK_A))
	{
		m_Camera.MoveLeft(m_MoveSpeed);
	}
	if (I_DInput.IsKeyDown(DIK_D))
	{
		m_Camera.MoveRight(m_MoveSpeed);
	}
	// 카메라 회전
	if (I_DInput.IsKeyDown(DIK_Q))
	{
		m_Camera.RotateLeft(m_MoveSpeed);
	}
	if (I_DInput.IsKeyDown(DIK_E))
	{
		m_Camera.RotateRight(m_MoveSpeed);
	}
	if (I_DInput.IsKeyDown(DIK_R))
	{
		m_Camera.RotateUp(m_MoveSpeed);
	}
	if (I_DInput.IsKeyDown(DIK_F))
	{
		m_Camera.RotateDown(m_MoveSpeed);
	}
	if (I_DInput.IsKeyDown(DIK_Z))
	{
		m_Camera.MoveLeft(m_MoveSpeed * 2);
		m_Camera.RotateRight(m_MoveSpeed * 2);
	}
	if (I_DInput.IsKeyDown(DIK_X))
	{
		m_Camera.MoveRight(m_MoveSpeed * 2);
		m_Camera.RotateLeft(m_MoveSpeed * 2);
	}

	if (I_DInput.IsKeyDownEvent(DIK_C))
	{
	}
	if (I_DInput.IsKeyDownEvent(DIK_V))
	{
	}

	m_GuideAxis.Frame();
	m_Model.Frame();

	return true;
}

bool Cmfc05_aiApp::Render()
{
	m_GuideAxis.Render();
	m_Model.Render();

	return true;
}

bool Cmfc05_aiApp::Release()
{
	return true;
}
