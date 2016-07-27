#pragma once
#include "bgWindow.h"
#include "bgDevice.h"
#include "bgDInput.h"
#include "bgTimer.h"

#ifdef _DEBUG
#include "bgDWrite.h"
#endif //_DEBUG

class bgCore : public bgWindow, public bgDevice
{
public:
	bgDInput		m_DInput;
	bgTimer			m_Timer;
	bgDWrite		m_DWrite;
	IDXGISurface1*	m_pBackScreen;

public:
	bgCore();
	virtual ~bgCore();

public:
	bool	GameRun();
	bool	GameInit();
	bool	GameFrame();
	bool	GameRender();
	bool	GameRelease();

public:
	virtual bool	Init() { return true; }
	virtual bool	Frame() { return true; }
	virtual bool	Render() { return true; }
	virtual bool	Release() { return true; }

	virtual bool	PreInit();
	virtual bool	PostInit();
	virtual bool	PreFrame();
	virtual bool	PostFrame();
	virtual bool	PreRender();
	virtual bool	PostRender();

	virtual bool	PrintInfo(TCHAR* pszString);
	virtual bool	PrintLog(TCHAR* pszString, int iX, int iY);
};
