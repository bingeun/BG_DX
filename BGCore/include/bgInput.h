#pragma once
#include "bgStd.h"
#include "bgSys.h"
#include <dinput.h>

#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")

#define KEYDOWN(key)	((m_KeyState[key] & 0x80) ? true : false)
#define KEYUP(key)		((m_KeyState[key] & 0x80) ? false : true)

#define MAX_KEYSTATE		(256)
#define DIDOD_BUFFER_SIZE	(16)

#define I_Input bgInput::GetInstance()

/*
struct BGINPUT_MAP
{
	bool bUpKey;
	bool bDownKey;
	bool bLeftKey;
	bool bRightKey;

	bool bWKey;
	bool bSKey;
	bool bAKey;
	bool bDKey;
	bool bQKey;
	bool bEKey;

	bool bLeftClick;
	bool bRightClick;
	bool bMiddleClick;

	bool bExit;
	bool bSpace; // 카메라의 이동가속도

	int  iMouseValue[3];

	bool bFullScreen;
	bool bChangeFillMode;
	bool bChangePrimitive;
	bool bChangeCullMode;
	bool bChangeCameraType;
};
*/

class bgInput : public bgSingleton<bgInput>
{
private:
	friend class bgSingleton<bgInput>;

public:
	LPDIRECTINPUT8			m_pDi;
	LPDIRECTINPUTDEVICE8	m_pDidevKey;
	LPDIRECTINPUTDEVICE8	m_pDidevMouse;
	DIDEVICEOBJECTDATA		m_Didod[DIDOD_BUFFER_SIZE];

	BYTE			m_KeyState[MAX_KEYSTATE];
	BYTE			m_KeyBeforeState[MAX_KEYSTATE];
	DIMOUSESTATE	m_MouseState;
	DIMOUSESTATE	m_MouseBeforeState;
	DWORD			m_dwElements;
	HWND			m_hWnd;
	DWORD			m_dwImmediate;

public:
	bool		InitDirectInput(HINSTANCE, HWND, bool, bool);
	bool		KeyProcess();
	bool		MouseProcess();
	void		DeviceAcquire();
	void		DeviceUnacquire();
	bool		OneKeyDown(DWORD dwKey);
	bool		IsKeyDown(DWORD dwKey);
	bool		IsKeyUP(DWORD dwKey);
	bool		PostProcess();

	bool		ResetDevice();
	bool		ResetResource() { return true; }

public:
	bool		Init();
	bool		Frame();
	bool		Render();
	bool		Release();

public:
	bgInput();
	virtual ~bgInput();
};
