#include "dx04.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInsteance, LPWSTR lpCmdLine, int nCmdShow)
{
	dx04 core;
	if (core.InitWindow(hInstance, L"BG Project!", 800, 600))
	{
		core.AppRun();
	}
	return 1;
}

dx04::dx04()
{
}

dx04::~dx04()
{
}

bool dx04::Init()
{
	m_objWorldAxis.Init();
	m_objWorldAxis.SetDevice(m_pDevice, m_pDContext, m_pRSWireNone, m_Camera.m_pMatrixBuffer);
	m_objWorldAxis.CreateBuffer(1000.0f);
	m_objWorldAxis.LoadShader();

	return true;
}

bool dx04::Frame()
{
	if (I_DInput.IsKeyDown(DIK_HOME))
	{
		m_Camera.MoveForward(m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 200.0f);
		m_Camera.m_vEye.z += m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 200.0f; // 임시
	}
	if (I_DInput.IsKeyDown(DIK_END))
	{
		m_Camera.MoveBackward(m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 200.0f);
		m_Camera.m_vEye.z -= m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 200.0f; // 임시
	}
	if (I_DInput.IsKeyDown(DIK_DELETE))
	{
		m_Camera.MoveLeft(m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 200.0f);
		m_Camera.m_vEye.x -= m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 200.0f; // 임시
	}
	if (I_DInput.IsKeyDown(DIK_PGDN))
	{
		m_Camera.MoveRight(m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 200.0f);
		m_Camera.m_vEye.x += m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 200.0f; // 임시
	}
	if (I_DInput.IsKeyDown(DIK_PGUP))
	{
		m_Camera.RotateLeft(m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 200.0f);
		m_Camera.m_vEye.y -= m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 200.0f; // 임시
	}
	if (I_DInput.IsKeyDown(DIK_INSERT))
	{
		m_Camera.RotateRight(m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 200.0f);
		m_Camera.m_vEye.y += m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 200.0f; // 임시
	}
	// 카메라 이동
	if (I_DInput.IsKeyDown(DIK_W))
	{
		m_Camera.MoveForward(m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 20.0f);
		m_Camera.m_vEye.z += m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 20.0f; // 임시
	}
	if (I_DInput.IsKeyDown(DIK_S))
	{
		m_Camera.MoveBackward(m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 20.0f);
		m_Camera.m_vEye.z -= m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 20.0f; // 임시
	}
	if (I_DInput.IsKeyDown(DIK_A))
	{
		m_Camera.MoveLeft(m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 20.0f);
		m_Camera.m_vEye.x -= m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 20.0f; // 임시
	}
	if (I_DInput.IsKeyDown(DIK_D))
	{
		m_Camera.MoveRight(m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 20.0f);
		m_Camera.m_vEye.x += m_Timer.m_fSPF * m_Camera.m_fMoveSpeed * 20.0f; // 임시
	}
	// 카메라 회전
	if (I_DInput.IsKeyDown(DIK_Q))
	{
		m_Camera.RotateLeft(m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 20.0f);
		m_Camera.m_vEye.y -= m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 20.0f; // 임시
	}
	if (I_DInput.IsKeyDown(DIK_E))
	{
		m_Camera.RotateRight(m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 20.0f);
		m_Camera.m_vEye.y += m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 20.0f; // 임시
	}
	if (I_DInput.IsKeyDown(DIK_R))
	{
		m_Camera.RotateUp(m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 20.0f);
	}
	if (I_DInput.IsKeyDown(DIK_F))
	{
		m_Camera.RotateDown(m_Timer.m_fSPF * m_Camera.m_fRotateSpeed * 20.0f);
	}

	m_objWorldAxis.Frame();

	return true;
}

bool dx04::Render()
{
	m_objWorldAxis.Render();

	return true;
}

bool dx04::Release()
{
	return true;
}

LRESULT CALLBACK dx04::AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case '1': // 윈도우 크기 변경 (1024x768 <=> 800x600)
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

		case '2': // 키눌림 상태 출력여부 변경
		{
			if (m_DWrite.m_bFontBorder)
				m_bPrintKeyState = !m_bPrintKeyState;
			m_DWrite.m_bFontBorder = !m_DWrite.m_bFontBorder;
		}
		break;

		case '3': // 와이어프레임 <=> 솔리드 모드변경
		{

		}
		break;

		case '0': // 윈도우 <=> 전체화면 모드변경
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
