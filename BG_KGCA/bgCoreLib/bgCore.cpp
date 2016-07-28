#include "bgCore.h"

bgCore::bgCore()
{
	m_bPrintKeyState = false;
}

bgCore::~bgCore()
{
}

bool bgCore::GameRun()
{
	GameFrame();
	GameRender();
	return true;
}

bool bgCore::GameInit()
{
	if (!PreInit())
		return false;
	if (!Init())
		return false;
	if (!PostInit())
		return false;
	return true;
}

bool bgCore::GameFrame()
{
	PreFrame();
	Frame();
	PostFrame();
	return true;
}

bool bgCore::GameRender()
{
	PreRender();
	Render();
	PostRender();
	return true;
}

bool bgCore::GameRelease()
{
	Release();
	m_Timer.Release();
	m_DWrite.Release();
	I_DInput.Release();
	return true;
}

bool bgCore::PreInit()
{
	InitDevice(m_hWnd);
	I_DInput.Create();
	I_DInput.Init();

	m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (LPVOID*)&m_pBackScreen);
	m_DWrite.Set(m_hWnd, m_iClientW, m_iClientH, m_pBackScreen);
	m_Timer.Init();
	return true;
}

bool bgCore::PostInit()
{
	return true;
}

bool bgCore::PreFrame()
{
	m_Timer.Frame();
	I_DInput.Frame();
	m_DWrite.Frame();
	return true;
}

bool bgCore::PostFrame()
{
	return true;
}

bool bgCore::PreRender()
{
	float ClearColor[4] = { 0.0f, 0.25f, 0.5f, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	return true;
}

bool bgCore::PostRender()
{
	if (m_bVsync)
	{
		m_Timer.Render();
		I_DInput.Render();
		m_DWrite.Render();
		m_pSwapChain->Present(1, 0);
	}
	else
	{
		m_Timer.Render();
		I_DInput.Render();
#ifdef _DEBUG
		TCHAR pBuffer[DEBUG_BUFFER_SIZE] = { 0, };
		_stprintf_s(pBuffer, _T("int=%d, float=%f"), 1024, 3.141592f);
		PrintInfo(pBuffer); // FPS 및 로그 출력
#endif //_DEBUG
		m_pSwapChain->Present(0, 0);
	}
	return true;
}

bool bgCore::PrintInfo(TCHAR* pszString)
{
	// FPS 출력
	TCHAR pBuffer[DEBUG_BUFFER_SIZE] = { 0, };
	if (m_bPrintKeyState)
	{
		_stprintf_s(pBuffer, _T("FPS[%d] SPF[%.3fms] Sec[%.2f] Pos[%d,%d,%d]\n%s%s%s %s%s%s %s%s%s\n%s%s%s %s%s%s %s%s%s\n%s"),
			g_iFPS, g_fSPF*1000.0f, g_fCurrent, I_DInput.m_iMouseX, I_DInput.m_iMouseY, I_DInput.m_iMouseZ,
			(I_DInput.IsKeyDown(DIK_Q)) ? _T("[Q]") : _T(" - "),
			(I_DInput.IsKeyDown(DIK_W)) ? _T("[W]") : _T(" - "),
			(I_DInput.IsKeyDown(DIK_E)) ? _T("[E]") : _T(" - "),		// QWE
			_T("   "), //(I_DInput.IsKeyDown(DIK_O)) ? _T("[O]") : _T(" - "),
			(I_DInput.IsKeyDown(DIK_UP)) ? _T("[U]") : _T(" - "),
			_T("   "), //(I_DInput.IsKeyDown(DIK_P)) ? _T("[P]") : _T(" - "),		//  U
			(I_DInput.IsKeyDown(DIK_INSERT)) ? _T("Ins") : _T(" - "),
			(I_DInput.IsKeyDown(DIK_HOME)) ? _T("Hom") : _T(" - "),
			(I_DInput.IsKeyDown(DIK_PGUP)) ? _T("PgU") : _T(" - "),		// InsHomPgUp
			(I_DInput.IsKeyDown(DIK_A)) ? _T("[A]") : _T(" - "),
			(I_DInput.IsKeyDown(DIK_S)) ? _T("[S]") : _T(" - "),
			(I_DInput.IsKeyDown(DIK_D)) ? _T("[D]") : _T(" - "),		// ASD
			(I_DInput.IsKeyDown(DIK_LEFT)) ? _T("[L]") : _T(" - "),
			(I_DInput.IsKeyDown(DIK_DOWN)) ? _T("[D]") : _T(" - "),
			(I_DInput.IsKeyDown(DIK_RIGHT)) ? _T("[R]") : _T(" - "),	// LDR
			(I_DInput.IsKeyDown(DIK_DELETE)) ? _T("Del") : _T(" - "),
			(I_DInput.IsKeyDown(DIK_END)) ? _T("End") : _T(" - "),
			(I_DInput.IsKeyDown(DIK_PGDN)) ? _T("PgD") : _T(" - "),		// DelEndPgDn
			pszString);
	}
	else
	{
		_stprintf_s(pBuffer, _T("FPS[%d] SPF[%.3fms] Sec[%.2f] Pos[%d,%d,%d]\n%s"),
			g_iFPS, g_fSPF*1000.0f, g_fCurrent, I_DInput.m_iMouseX, I_DInput.m_iMouseY, I_DInput.m_iMouseZ,
			pszString);
	}
	PrintLog(pBuffer, 5, 5);

	return true;
}

bool bgCore::PrintLog(TCHAR* pszString, int iX, int iY)
{
	if (m_DWrite.m_pTextFormat)
	{
		RECT rect = { iX, iY, m_iClientW - iX, m_iClientH - iY };
		m_DWrite.PreRender();
		{
			FLOAT fFontSize = m_DWrite.m_fFontSize;
			m_DWrite.m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING); // LEFT
			m_DWrite.m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR); // TOP
			m_DWrite.m_pTextFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, fFontSize * 1.25f, fFontSize * 0.8f);

			// 검정색 외곽선
			if(m_DWrite.m_bFontBorder)
			{
				m_DWrite.Print(rect, pszString, D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
				rect.left += 2;
				rect.top += 2;
				m_DWrite.Print(rect, pszString, D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
				rect.left -= 1;
				rect.top -= 1;
			}

			m_DWrite.Print(rect, pszString, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
		}
		m_DWrite.PostRender();
	}

	return true;
}
