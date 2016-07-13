#include "bgCore.h"

bgCore::bgCore()
{
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
	m_Input.Release();
	return true;
}

bool bgCore::PreInit()
{
	InitDevice(m_hWnd);
	m_Timer.Init();
	m_Input.Init();
	return true;
}

bool bgCore::PostInit()
{
	return true;
}

bool bgCore::PreFrame()
{
	m_Timer.Frame();
	m_Input.Frame();
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
		m_Input.Render();
		PrintDebug();
		m_pSwapChain->Present(1, 0);
	}
	else
	{
		m_Timer.Render();
		m_Input.Render();
		PrintDebug();
		m_pSwapChain->Present(0, 0);
	}
	return true;
}

bool bgCore::PrintDebug()
{
#ifdef _DEBUG
	// FPS Ãâ·Â
	TCHAR pBuffer[DEBUG_BUFFER_SIZE];
	memset(pBuffer, 0, sizeof(TCHAR) * DEBUG_BUFFER_SIZE);
	_stprintf_s(pBuffer, _T("FPS[%d] SPF[%.3fms] Sec[%.2f]\nLog[???]"),
				g_iFPS, g_fSPF*1000.0f, g_fCurrent);

	// printf()?

#endif //_DEBUG
	return true;
}

bool bgCore::PrintDebug(TCHAR * pszString, int iX, int iY)
{
#ifdef _DEBUG

#endif //_DEBUG
	return true;
}
