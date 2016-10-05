#pragma once
#include "bgStd.h"
#include "bgSys.h"
#include <dinput.h>

#define I_DInput bgDInput::GetInstance()

#define KEY_DOWN(key)			((m_KeyState[(key)] & 0x80) ? true : false)
#define KEY_UP(key)				((m_KeyState[(key)] & 0x80) ? false : true)
#define MOUSE_DOWN(key)			((m_MouseState.rgbButtons[(key)]) ? true : false)
#define MOUSE_UP(key)			((m_MouseState.rgbButtons[(key)]) ? false : true)
#define KEY_DOWN_EVENT(key)		(((m_KeyState[(key)] & 0x80) && !(m_KeyStateBefore[(key)] & 0x80)) ? true : false)
#define MOUSE_DOWN_EVENT(key)	(((m_MouseState.rgbButtons[(key)] & 0x80) && !(m_MouseStateBefore.rgbButtons[(key)] & 0x80)) ? true : false)

#define SIZE_KEYSTATE	(256)
#define SIZE_KEYDOD		(16)

class bgDInput : public bgSingleton<bgDInput>
{
private:
	friend class bgSingleton<bgDInput>;

public:
	HWND					m_hWnd;
	LPDIRECTINPUT8			m_pDInput;
	LPDIRECTINPUTDEVICE8	m_pDInputKey;
	LPDIRECTINPUTDEVICE8	m_pDInputMouse;
	DIDEVICEOBJECTDATA		m_KeyDOD[SIZE_KEYDOD];

	DIMOUSESTATE	m_MouseState;
	DIMOUSESTATE	m_MouseStateBefore;
	BYTE			m_KeyState[SIZE_KEYSTATE];
	BYTE			m_KeyStateBefore[SIZE_KEYSTATE];
	DWORD			m_dwImmediate;
	DWORD			m_dwElements;

public:
	LONG	m_iMouseX;
	LONG	m_iMouseY;
	LONG	m_iMouseZ;

public:
	bgDInput();
	virtual ~bgDInput();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	bool	ResetDevice();
	bool	ResetResource() { return true; }
	
public:
	bool	Create(bool bKey = true, bool bMouse = true);
	bool	KeyProcess();
	bool	MouseProcess();
	bool	PreProcess();

	bool	IsKeyUp(DWORD dwKey);
	bool	IsKeyDown(DWORD dwKey);
	bool	IsKeyDownEvent(DWORD dwKey);
	bool	IsMouseUp(DWORD dwKey) { return ((m_MouseState.rgbButtons[(dwKey)]) ? false : true); }
	bool	IsMouseDown(DWORD dwKey) { return ((m_MouseState.rgbButtons[(dwKey)]) ? true : false); }
	bool	IsMouseDownEvent(DWORD dwKey) { return (((m_MouseState.rgbButtons[(dwKey)] & 0x80) && !(m_MouseStateBefore.rgbButtons[(dwKey)] & 0x80)) ? true : false); }
	void	SetAcquire(bool bActive);
	void	DeviceAcquire();
	void	DeviceUnacquire();
};
