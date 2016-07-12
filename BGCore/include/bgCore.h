#pragma once
#include "bgTimer.h"
#include "bgInput.h"
#include "bgDWrite.h"
#include "bgWindow.h"

class bgCore : public bgWindow
{
public:
	bgTimer		m_Timer;
	bgInput		m_Input;

public:
	bool	GameRun();
	bool	GameInit();
	bool	GameFrame();
	bool	GameRender();
	bool	GameRelease();
	void	MsgEvent(MSG msg);

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

	virtual bool	DrawDebug();
	virtual bool	DrawDebug(TCHAR* pString, int iX, int iY);
	
public:
	bgCore();
	virtual ~bgCore();
};
