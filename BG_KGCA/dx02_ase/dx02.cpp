#include "dx02.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	dx02 core;
	if (core.InitWindow(hInstance, L"BG Project!", 800, 600))
	{
		core.AppRun();
	}
	return 1;
}

dx02::dx02()
{
}

dx02::~dx02()
{
}

bool dx02::Init()
{
	m_objWorldAxis.Init();
	m_objWorldAxis.SetDevice(m_pDevice, m_pDContext, m_pRSWireNone, m_Camera.m_pMatrixBuffer);
	m_objWorldAxis.CreateBuffer(1000.0f);
	m_objWorldAxis.LoadShader();

	m_objMob.Init();
	m_objMob.SetDevice(m_pDevice, m_pDContext, m_pRSWireNone, m_Camera.m_pMatrixBuffer);
	m_objMob.CreateBuffer();
	m_objMob.LoadShader();

	m_objHero.Init();
	m_objHero.SetDevice(m_pDevice, m_pDContext, m_pRSWireNone, m_Camera.m_pMatrixBuffer);
	m_objHero.CreateBuffer();
	m_objHero.LoadShader();

	m_objHero.m_vPos.x = -15.0f;
	m_objHero.m_vPos.y = 0.0f;
	m_objHero.m_vPos.z = -15.0f;

	m_objMob.m_pHeroPos = &m_objHero.m_vPos;

	return true;
}

bool dx02::Frame()
{
	if (I_DInput.IsKeyDown(DIK_W))
	{
		m_Camera.MoveForward();
	}
	if (I_DInput.IsKeyDown(DIK_S))
	{
		m_Camera.MoveBackward();
	}
	if (I_DInput.IsKeyDown(DIK_A))
	{
		m_Camera.MoveLeft();
	}
	if (I_DInput.IsKeyDown(DIK_D))
	{
		m_Camera.MoveRight();
	}

	if (I_DInput.IsKeyDown(DIK_Q))
	{
		m_Camera.RotateLeft(4.0f);
	}
	if (I_DInput.IsKeyDown(DIK_E))
	{
		m_Camera.RotateRight(4.0f);
	}
	if (I_DInput.IsKeyDown(DIK_R))
	{
		m_Camera.RotateUp(4.0f);
	}
	if (I_DInput.IsKeyDown(DIK_F))
	{
		m_Camera.RotateDown(4.0f);
	}

	m_objWorldAxis.Frame();
	m_objMob.Frame();
	
	//m_objHero.Frame();
	{
		if (I_DInput.IsKeyDown(DIK_LEFT))
		{
			m_objHero.m_vPos.x -= g_fSPF * 5.0f;
		}
		if (I_DInput.IsKeyDown(DIK_RIGHT))
		{
			m_objHero.m_vPos.x += g_fSPF * 5.0f;
		}
		if (I_DInput.IsKeyDown(DIK_UP))
		{
			m_objHero.m_vPos.z += g_fSPF * 5.0f;
		}
		if (I_DInput.IsKeyDown(DIK_DOWN))
		{
			m_objHero.m_vPos.z -= g_fSPF * 5.0f;
		}

		D3DXVECTOR3 vMin = D3DXVECTOR3(m_objHero.m_vPos.x - 1.0f, m_objHero.m_vPos.y - 1.0f, m_objHero.m_vPos.z - 1.0f);
		D3DXVECTOR3 vMax = D3DXVECTOR3(m_objHero.m_vPos.x + 1.0f, m_objHero.m_vPos.y + 1.0f, m_objHero.m_vPos.z + 1.0f);
		
		D3DXMatrixIdentity(&m_objHero.m_MatrixBuffer.matWorld);
		D3DXMatrixTranslation(&m_objHero.m_MatrixBuffer.matWorld, m_objHero.m_vPos.x, m_objHero.m_vPos.y, m_objHero.m_vPos.z);
		m_objHero.SetMatrix(&m_objHero.m_MatrixBuffer.matWorld, &m_objHero.m_MatrixBuffer.matView, &m_objHero.m_MatrixBuffer.matProj);
	}

	return true;
}

bool dx02::Render()
{
	m_objWorldAxis.Render();
	m_objMob.Render();
	m_objHero.Render();

	return true;
}

bool dx02::Release()
{
	m_objWorldAxis.Release();
	m_objMob.Release();
	m_objHero.Release();

	return true;
}

LRESULT CALLBACK dx02::AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

		case '4': // 와이어프레임 <=> 솔리드 모드변경
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
