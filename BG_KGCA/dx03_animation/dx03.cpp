#include "dx03.h"

TCHAR* szASEFileName[] =
{
	_T("../../data/model/box.ase"),					// 0	�ڽ�
	_T("../../data/model/ship.ase"),				// 1	���ߺξ� ��
	_T("../../data/model/MultiCameras.ase"),		// 2	��Ƽī�޶�
	_T("../../data/model/scaleanimationmodel.ASE"),	// 3	�ڽ� ������ �ִϸ��̼�
	_T("../../data/model/Turret_Deploy.ASE"),		// 4	�ͷ� �ִϸ��̼�
};
int g_iASEFileIndex = 3;	// <<=== ASE �ε��� �ֱ�
int g_iASEFileIndex2 = 2;	// <<=== ASE �ε��� �ֱ�

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInsteance, LPWSTR lpCmdLine, int nCmdShow)
{
	dx03 core;
	if (core.InitWindow(hInstance, L"BG Project!", 800, 600))
	{
		core.AppRun();
	}
	return 1;
}

dx03::dx03()
{
}

dx03::~dx03()
{
}

bool dx03::Init()
{
	m_objWorldAxis.Init();
	m_objWorldAxis.SetDevice(m_pDevice, m_pDContext, m_pRSWireNone, m_pMatrixBuffer);
	m_objWorldAxis.CreateBuffer(1000.0f);
	m_objWorldAxis.LoadShader();


	// m_pRSWireFront �� �� m_pRSWireNone �� �յ� m_pRSSolidFront �� �� m_pRSSolidNone �� �յ� 
	m_ParserASE.Init(&m_Model);
	m_ParserASE.Open(szASEFileName[g_iASEFileIndex]);
	m_Model.SetDevice(m_pDevice, m_pDContext, m_pRSSolidFront, m_pMatrixBuffer);
	m_Model.CreateBuffer();
	m_Model.LoadShader("VS", "PS_Tex");

	m_ParserASE.Init(&m_Model2);
	m_ParserASE.Open(szASEFileName[g_iASEFileIndex2]);
	m_Model2.SetDevice(m_pDevice, m_pDContext, m_pRSSolidFront, m_pMatrixBuffer);
	m_Model2.CreateBuffer();
	m_Model2.LoadShader("VS", "PS_Tex");

	return true;
}

bool dx03::Frame()
{
	if (I_DInput.IsKeyDown(DIK_HOME))
	{
		m_CameraViewport[0].MoveForward(m_Timer.m_fSPF * m_fSpeedCamera * 250.0f);
		m_CameraViewport[0].m_Eye.z += m_Timer.m_fSPF * m_fSpeedCamera * 250.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_END))
	{
		m_CameraViewport[0].MoveBackward(m_Timer.m_fSPF * m_fSpeedCamera * 250.0f);
		m_CameraViewport[0].m_Eye.z -= m_Timer.m_fSPF * m_fSpeedCamera * 250.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_DELETE))
	{
		m_CameraViewport[0].MoveLeft(m_Timer.m_fSPF * m_fSpeedCamera * 250.0f);
		m_CameraViewport[0].m_Eye.x -= m_Timer.m_fSPF * m_fSpeedCamera * 250.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_PGDN))
	{
		m_CameraViewport[0].MoveRight(m_Timer.m_fSPF * m_fSpeedCamera * 250.0f);
		m_CameraViewport[0].m_Eye.x += m_Timer.m_fSPF * m_fSpeedCamera * 250.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_PGUP))
	{
		m_CameraViewport[0].RotateLeft(m_Timer.m_fSPF * m_fSpeedCamera * 250.0f);
		m_CameraViewport[0].m_Eye.y -= m_Timer.m_fSPF * m_fSpeedCamera * 250.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_INSERT))
	{
		m_CameraViewport[0].RotateRight(m_Timer.m_fSPF * m_fSpeedCamera * 250.0f);
		m_CameraViewport[0].m_Eye.y += m_Timer.m_fSPF * m_fSpeedCamera * 250.0f; // �ӽ�
	}
	// ī�޶� �̵�
	if (I_DInput.IsKeyDown(DIK_W))
	{
		m_CameraViewport[0].MoveForward(m_Timer.m_fSPF * m_fSpeedCamera * 50.0f);
		m_CameraViewport[0].m_Eye.z += m_Timer.m_fSPF * m_fSpeedCamera * 50.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_S))
	{
		m_CameraViewport[0].MoveBackward(m_Timer.m_fSPF * m_fSpeedCamera * 50.0f);
		m_CameraViewport[0].m_Eye.z -= m_Timer.m_fSPF * m_fSpeedCamera * 50.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_A))
	{
		m_CameraViewport[0].MoveLeft(m_Timer.m_fSPF * m_fSpeedCamera * 50.0f);
		m_CameraViewport[0].m_Eye.x -= m_Timer.m_fSPF * m_fSpeedCamera * 50.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_D))
	{
		m_CameraViewport[0].MoveRight(m_Timer.m_fSPF * m_fSpeedCamera * 50.0f);
		m_CameraViewport[0].m_Eye.x += m_Timer.m_fSPF * m_fSpeedCamera * 50.0f; // �ӽ�
	}
	// ī�޶� ȸ��
	if (I_DInput.IsKeyDown(DIK_Q))
	{
		m_CameraViewport[0].RotateLeft(m_Timer.m_fSPF * m_fSpeedCamera * 50.0f);
		m_CameraViewport[0].m_Eye.y -= m_Timer.m_fSPF * m_fSpeedCamera * 50.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_E))
	{
		m_CameraViewport[0].RotateRight(m_Timer.m_fSPF * m_fSpeedCamera * 50.0f);
		m_CameraViewport[0].m_Eye.y += m_Timer.m_fSPF * m_fSpeedCamera * 50.0f; // �ӽ�
	}
	if (I_DInput.IsKeyDown(DIK_R))
	{
		m_CameraViewport[0].RotateUp(m_Timer.m_fSPF * m_fSpeedCamera);
	}
	if (I_DInput.IsKeyDown(DIK_F))
	{
		m_CameraViewport[0].RotateDown(m_Timer.m_fSPF * m_fSpeedCamera);
	}

	m_objWorldAxis.Frame();
	m_Model.Frame();
	m_Model2.Frame();

	return true;
}

bool dx03::Render()
{
	m_objWorldAxis.Render();
	m_Model.Render();
	m_Model2.Render();

	return true;
}

bool dx03::Release()
{
	return true;
}

LRESULT CALLBACK dx03::AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case '1': // ������ ũ�� ���� (1024x768 <=> 800x600)
		{
			if (m_pSwapChain && !m_bFullScreen)
			{
				m_bSizeLarge = !m_bSizeLarge;
				if (m_bSizeLarge)
					g_pWindow->SetWindowSize(m_hWnd, 1024, 768);
				else
					g_pWindow->SetWindowSize(m_hWnd, 800, 600);

				DXGI_MODE_DESC desc;
				ZeroMemory(&desc, sizeof(desc));
				desc.Width = (m_bSizeLarge) ? 1024 : 800;
				desc.Height = (m_bSizeLarge) ? 768 : 600;
				desc.RefreshRate.Numerator = 0;
				desc.RefreshRate.Denominator = 0;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				m_pSwapChain->ResizeTarget(&desc);
			}
		}
		break;

		case '2': // Ű���� ���� ��¿��� ����
		{
			if (m_DWrite.m_bFontBorder)
				m_bPrintKeyState = !m_bPrintKeyState;
			m_DWrite.m_bFontBorder = !m_DWrite.m_bFontBorder;
		}
		break;

		case '3': // ����Ʈ ��� ���� (0=���ϸ��, 1=2x2���, 2=4+1���)
		{
			m_iModeViewport = (m_iModeViewport + 1) % 3;
			switch (m_iModeViewport)
			{
			case 0: // ����ȭ�� ���
			{
			}
			break;

			case 1: // 2x2ȭ�� ���
			{
			}
			break;

			case 2: // 4+1ȭ�� ���
			{
			}
			break;
			}
		}
		break;

		case '4': // ���̾������� <=> �ָ��� ��庯��
		{

		}
		break;

		case '0': // ������ <=> ��üȭ�� ��庯��
		{
			if (m_pSwapChain)
			{
				m_bFullScreen = !m_bFullScreen;
				if (m_bFullScreen)
					m_pSwapChain->SetFullscreenState(TRUE, NULL);
				else
					m_pSwapChain->SetFullscreenState(FALSE, NULL);
			}
		}
		break;

		case VK_ESCAPE:
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
		}
	}
	break;

	default:
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	break;
	}
}
