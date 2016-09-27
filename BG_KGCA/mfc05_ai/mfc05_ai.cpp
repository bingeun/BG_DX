
// mfc05_ai.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
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



TCHAR* szASEFileName[] =
{
	_T("../../data/model/box.ase"),					// 0	�ڽ�
	_T("../../data/model/ship.ase"),				// 1	���ߺξ� ��
	_T("../../data/model/MultiCameras.ase"),		// 2	��Ƽī�޶�
	_T("../../data/model/scaleanimationmodel.ASE"),	// 3	�ڽ� ������ �ִϸ��̼�
	_T("../../data/model/BoxAnimation.ASE"),		// 4	�ڽ� �ִϸ��̼� 2
	_T("../../data/model/Turret_Deploy.ASE"),		// 5	�ͷ� �ִϸ��̼�
	_T("../../data/model/mob.ASE"),					// 6	�� - �ΰ��� ����
};
int g_iASEFileIndex = 3;	// <<=== ASE �ε��� �ֱ�
int g_iASEFileIndex2 = 5;	// <<=== ASE �ε��� �ֱ�


// Cmfc05_aiApp

BEGIN_MESSAGE_MAP(Cmfc05_aiApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &Cmfc05_aiApp::OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// ǥ�� �μ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// Cmfc05_aiApp ����

Cmfc05_aiApp::Cmfc05_aiApp()
{
	m_bHiColorIcons = TRUE;

	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���� ���α׷��� ���� ��� ��Ÿ�� ������ ����Ͽ� ������ ���(/clr):
	//     1) �� �߰� ������ �ٽ� ���� ������ ������ ����� �۵��ϴ� �� �ʿ��մϴ�.
	//     2) ������Ʈ���� �����Ϸ��� System.Windows.Forms�� ���� ������ �߰��ؾ� �մϴ�.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("mfc05_ai.AppID.NoVersion"));

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

// ������ Cmfc05_aiApp ��ü�Դϴ�.

Cmfc05_aiApp theApp;


// Cmfc05_aiApp �ʱ�ȭ

BOOL Cmfc05_aiApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
	// AfxInitRichEdit2();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ���� ���α׷��� ���� ���ø��� ����մϴ�.  ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(Cmfc05_aiDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ������ â�Դϴ�.
		RUNTIME_CLASS(Cmfc05_aiView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
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

	// â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

int Cmfc05_aiApp::ExitInstance()
{
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
	AfxOleTerm(FALSE);

	// ===========================================================================================================================

	GameRelease();

	return CWinAppEx::ExitInstance();
}

// Cmfc05_aiApp �޽��� ó����


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void Cmfc05_aiApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// Cmfc05_aiApp ����� ���� �ε�/���� �޼���

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

// Cmfc05_aiApp �޽��� ó����





BOOL Cmfc05_aiApp::OnIdle(LONG lCount)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	__super::OnIdle(lCount);

	// ==========================================================================================================

	GameFrame();
	GameRender();

	// ==========================================================================================================

	return TRUE;
}







bool Cmfc05_aiApp::Init()
{
	m_objWorldAxis.Init();
	m_objWorldAxis.SetDevice(m_pDevice, m_pDContext, m_pRSWireNone, m_Camera.m_pMatrixBuffer);
	m_objWorldAxis.CreateBuffer(1000.0f);
	m_objWorldAxis.LoadShader();


	// m_pRSWireFront �� �� m_pRSWireNone �� �յ� m_pRSSolidFront �� �� m_pRSSolidNone �� �յ� 
	m_ParserASE.Init(&m_Model);
	m_ParserASE.Open(szASEFileName[g_iASEFileIndex]);
	m_Model.SetDevice(m_pDevice, m_pDContext, m_pRSSolidFront, m_Camera.m_pMatrixBuffer);
	m_Model.CreateBuffer();
	m_Model.LoadShader("VS", "PS_Tex");

	m_ParserASE.Init(&m_Model2);
	m_ParserASE.Open(szASEFileName[g_iASEFileIndex2]);
	m_Model2.SetDevice(m_pDevice, m_pDContext, m_pRSSolidFront, m_Camera.m_pMatrixBuffer);
	m_Model2.CreateBuffer();
	m_Model2.LoadShader("VS", "PS_Tex");

	return true;
}

bool Cmfc05_aiApp::Frame()
{
	if (I_DInput.IsKeyDown(DIK_HOME))
	{
		m_Camera.MoveForward();
		//m_Camera.m_vEye.z += m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 200.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_END))
	{
		m_Camera.MoveBackward();
		//m_Camera.m_vEye.z -= m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 200.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_DELETE))
	{
		m_Camera.MoveLeft();
		//m_Camera.m_vEye.x -= m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 200.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_PGDN))
	{
		m_Camera.MoveRight();
		//m_Camera.m_vEye.x += m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 200.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_PGUP))
	{
		m_Camera.MoveDown();
		//m_Camera.m_vEye.y -= m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 200.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_INSERT))
	{
		m_Camera.MoveUp();
		//m_Camera.m_vEye.y += m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 200.0f; // �ӽ�
	}
	// ī�޶� �̵�
	if (I_DInput.IsKeyDown(DIK_W))
	{
		m_Camera.MoveForward();
		//m_Camera.m_vEye.z += m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 20.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_S))
	{
		m_Camera.MoveBackward();
		//m_Camera.m_vEye.z -= m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 20.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_A))
	{
		m_Camera.MoveLeft();
		m_Camera.m_vEye.x -= m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 20.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_D))
	{
		m_Camera.MoveRight();
		m_Camera.m_vEye.x += m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 20.0f; // �ӽ�
	}
	// ī�޶� ȸ��
	if (I_DInput.IsKeyDown(DIK_Q))
	{
		m_Camera.RotateLeft();
		//m_Camera.m_vEye.y -= m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 20.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_E))
	{
		m_Camera.RotateRight();
		//m_Camera.m_vEye.y += m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 20.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_R))
	{
		m_Camera.RotateUp();
	}
	if (I_DInput.IsKeyDown(DIK_F))
	{
		m_Camera.RotateDown();
	}

	m_objWorldAxis.Frame();
	m_Model.Frame();
	m_Model2.Frame();

	return true;
}

bool Cmfc05_aiApp::Render()
{
	m_objWorldAxis.Render();
	m_Model.Render();
	m_Model2.Render();

	return true;
}

bool Cmfc05_aiApp::Release()
{
	return true;
}
