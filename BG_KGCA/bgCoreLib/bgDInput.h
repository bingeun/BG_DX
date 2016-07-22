#pragma once
#include "bgStd.h"
#include "bgSys.h"
#include <dinput.h>

#define I_DInput bgDInput::GetInstance()

#define KEY_DOWN(key)	((m_KeyState[(key)] & 0x80) ? true : false)
#define KEY_UP(key)		((m_KeyState[(key)] & 0x80) ? false : true)

#define SIZE_KEYSTATE	(256)
#define SIZE_KEYDOD		(16)

class bgDInput : public bgSingleton<bgDInput>
{
private:
	friend class bgSingleton<bgDInput>;

public:
	LPDIRECTINPUT8			m_pDInput;
	LPDIRECTINPUTDEVICE8	m_pDInputKey;
	LPDIRECTINPUTDEVICE8	m_pDInputMouse;

	DIMOUSESTATE		m_MouseState;
	BYTE				m_KeyState[SIZE_KEYSTATE];
	BYTE				m_KeyStateBefore[SIZE_KEYSTATE];
	DIDEVICEOBJECTDATA	m_KeyDOD[SIZE_KEYDOD];
	DWORD				m_dwImmediate;
	DWORD				m_dwElements;

public:
	bgDInput();
	virtual ~bgDInput();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	void	PrintLog();

public:
	HRESULT	Create();
	bool	KeyProcess();
	bool	MouseProcess();
	bool	IsKeyDown(DWORD dwKey);
	void	SetAcquire(bool bActive);
	void	DeviceAcquire();
	void	DeviceUnacquire();
};
