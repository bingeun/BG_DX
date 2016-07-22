#pragma once
#include "bgTimer.h"
#include "bgDInput.h"
#include "bgWindow.h"
#include "bgDevice.h"

class bgCore : public bgWindow, public bgDevice
{
public:
	bgTimer		m_Timer;
	bgDInput	m_DInput;

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

	virtual bool	PrintDebug();
	virtual bool	PrintDebug(TCHAR* pszString, int iX, int iY);
};
