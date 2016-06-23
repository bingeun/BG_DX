#include "bgCore.h"



bool bgCore::DrawDebug()
{
#ifdef _DEBUG

#endif //_DEBUG
	return true;
}

bool bgCore::DrawDebug(TCHAR * pString, int iX, int iY)
{
#ifdef _DEBUG

#endif //_DEBUG
	return true;
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
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
	return true;
}

bool bgCore::PostRender()
{
	m_Timer.Render();
	m_Input.Render();
	m_pSwapChain->Present(0, 0);
	return true;
}

void bgCore::MsgEvent(MSG msg)
{
	//m_Input.MsgEvent(msg);
}

bgCore::bgCore()
{
}

bgCore::~bgCore()
{
}
