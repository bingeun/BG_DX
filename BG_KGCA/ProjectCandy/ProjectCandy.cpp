#include "ProjectCandy.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	ProjectCandy core;
	if (core.InitWindow(hInstance, L"BG Project!", CLIENT_W, CLIENT_H))
	{
		core.AppRun();
	}
	return 1;
}

ProjectCandy::ProjectCandy()
{
}

ProjectCandy::~ProjectCandy()
{
	Release();
}

bool ProjectCandy::Init()
{
	m_objBoard.Init();
	m_objBoard.SetDevice(m_pDevice, m_pDContext, m_pRSSolidFront, m_pMatrixBuffer);
	m_objBoard.CreateBuffer();
	m_objBoard.LoadShader();
	InitPlay();
	return true;
}

bool ProjectCandy::Frame()
{
	m_objBoard.Frame();
	return true;
}

bool ProjectCandy::Render()
{
	m_objBoard.Render();
	return true;
}

bool ProjectCandy::Release()
{
	m_objBoard.Release();
	return true;
}

LRESULT CALLBACK ProjectCandy::AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

		case '3': // 뷰포트 모드 변경 (0=단일모드, 1=2x2모드, 2=4+1모드)
		{
			m_iModeViewport = (m_iModeViewport + 1) % 3;
			switch (m_iModeViewport)
			{
			case 0: // 단일화면 모드
			{
			}
			break;

			case 1: // 2x2화면 모드
			{
			}
			break;

			case 2: // 4+1화면 모드
			{
			}
			break;
			}
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

void ProjectCandy::InitPlay()
{
	m_v2BoardPos = D3DXVECTOR2((float)BOARD_X, (float)BOARD_Y);
	m_v2TileStride = D3DXVECTOR2((float)TILE_W, (float)TILE_H);
	m_v2TileStrideUV = D3DXVECTOR2(1.0f / 8.0f, 1.0f / 8.0f);

	for (int y = 0; y < BOARD_H; y++)
	{
		for (int x = 0; x < BOARD_W; x++)
		{
			m_Board[x][y].Candy.iType = 0;
			m_Board[x][y].Candy.iValue = (x + y) % 6;
			m_Board[x][y].Tile.iType = 0;
			m_Board[x][y].Tile.iValue = 0;
		}
	}
}

void ProjectCandy::PutTile(float fX, float fY, int iNum)
{

}

void ProjectCandy::PutBoard()
{
}
