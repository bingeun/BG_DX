#include "bgCore.h"



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
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
	return true;
}

bool bgCore::PostRender()
{
	m_Timer.Render();
	m_Input.Render();
	DrawDebug();
	m_pSwapChain->Present(0, 0);
	return true;
}

void bgCore::MsgEvent(MSG msg)
{
	//m_Input.MsgEvent(msg);
}

bool bgCore::DrawDebug()
{
#ifdef _DEBUG
	// FPS Ãâ·Â
	TCHAR pBuffer[DEBUG_BUFFER_SIZE];
	memset(pBuffer, 0, sizeof(TCHAR) * DEBUG_BUFFER_SIZE);
	_stprintf_s(pBuffer, _T("FPS[%d] SPF[%.3fms] Now[%.2f]\nFontSize[%d]"),
		g_iFPS, g_fSPF*1000.0f, g_fCurrent, (int)m_Font.m_fontSize);

	m_Font.Begin(); // --------------------
	RECT rect = { 2, 2, m_iClientW, m_iClientH };
	m_Font.m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_Font.m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	m_Font.DrawText(rect, pBuffer, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.75f));
	m_Font.End(); // ----------------------
#endif //_DEBUG
	return true;
}

bool bgCore::DrawDebug(TCHAR * pString, int iX, int iY)
{
#ifdef _DEBUG

#endif //_DEBUG
	return true;
}

HRESULT bgCore::CreateDxResource()
{
	IDXGISurface1* pBackBuffer = NULL;
	HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (LPVOID*)&pBackBuffer);
	m_Font.Set(m_hWnd, m_iWindowW, m_iWindowH, pBackBuffer);
	if (pBackBuffer)
		pBackBuffer->Release();

	CreateResource();
	return S_OK;
}

HRESULT bgCore::DeleteDxResource()
{
	if (!m_Font.Release())
		return false;
	DeleteResource();
	return S_OK;
}

HRESULT bgCore::CreateResource()
{
	return S_OK;
}

HRESULT bgCore::DeleteResource()
{
	return S_OK;
}

bgCore::bgCore()
{
}

bgCore::~bgCore()
{
}
