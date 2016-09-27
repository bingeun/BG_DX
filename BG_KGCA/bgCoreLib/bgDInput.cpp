#include "bgDInput.h"

bgDInput::bgDInput()
{
	m_pDInput = NULL;
	m_pDInputKey = NULL;
	m_pDInputMouse = NULL;

	m_dwElements = 0;
	m_dwImmediate = TRUE; // TRUE FALSE

	m_iMouseX = 0;
	m_iMouseY = 0;
	m_iMouseZ = 0;
}

bgDInput::~bgDInput()
{
	Release();
}

bool bgDInput::Init()
{
	m_hWnd = g_hWnd;
	memset(&m_KeyStateBefore, 0, sizeof(BYTE) * SIZE_KEYSTATE);
	memset(&m_MouseStateBefore, 0, sizeof(DIMOUSESTATE));
	m_dwElements = 0;

	return true;
}

bool bgDInput::Frame()
{
	PreProcess();

	KeyProcess();
	MouseProcess();

	return true;
}

bool bgDInput::Render()
{
	return true;
}

bool bgDInput::Release()
{
	if (m_pDInput)
	{
		if (m_pDInputKey)
		{
			m_pDInputKey->Unacquire();
			m_pDInputKey->Release();
			m_pDInputKey = NULL;
		}
		if (m_pDInputMouse)
		{
			m_pDInputMouse->Unacquire();
			m_pDInputMouse->Release();
			m_pDInputMouse = NULL;
		}
		m_pDInput->Release();
		m_pDInput = NULL;
	}
	return true;
}

bool bgDInput::ResetDevice()
{
	Release();
	Init();
	return true;
}

bool bgDInput::Create(bool bKey, bool bMouse)
{
	HRESULT	hr = S_OK;

	if (m_pDInputKey || m_pDInputMouse)
		return true;

	BOOL_RETURN(DirectInput8Create(g_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDInput, NULL));

	// 키보드 장치생성
	if (bKey)
	{
		BOOL_RETURN(m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pDInputKey, NULL));
		BOOL_RETURN(m_pDInputKey->SetDataFormat(&c_dfDIKeyboard));
		if (FAILED(hr = m_pDInputKey->SetCooperativeLevel(m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
		{
			while (m_pDInputKey->Acquire() == DIERR_INPUTLOST)
				; // none
			BOOL_RETURN(m_pDInputKey->SetCooperativeLevel(m_hWnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND));
		}
		if (!m_dwImmediate)
		{
			DIPROPDWORD dipdw;
			dipdw.diph.dwSize = sizeof(DIPROPDWORD);
			dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			dipdw.diph.dwObj = 0;
			dipdw.diph.dwHow = DIPH_DEVICE;
			dipdw.dwData = SIZE_KEYDOD;
			BOOL_RETURN(m_pDInputKey->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph));

		}
		while (m_pDInputKey->Acquire() == DIERR_INPUTLOST)
			; // none
	}

	// 마우스 장치생성
	if (bMouse)
	{
		BOOL_RETURN(m_pDInput->CreateDevice(GUID_SysMouse, &m_pDInputMouse, NULL));
		BOOL_RETURN(m_pDInputMouse->SetDataFormat(&c_dfDIMouse));
		if (FAILED(hr = m_pDInputMouse->SetCooperativeLevel(m_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
		{
			while (m_pDInputMouse->Acquire() == DIERR_INPUTLOST)
				; // none
			BOOL_RETURN(m_pDInputMouse->SetCooperativeLevel(m_hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND));
		}
		while (m_pDInputMouse->Acquire() == DIERR_INPUTLOST)
			; // none

		m_iMouseX = m_MouseState.lX;
		m_iMouseY = m_MouseState.lY;
		m_iMouseZ = m_MouseState.lZ;
	}

	return true;
}

bool bgDInput::KeyProcess()
{
	HRESULT hr;

	if (m_dwImmediate)
	{
		ZeroMemory(m_KeyState, sizeof(BYTE) * SIZE_KEYSTATE);
		if (!m_pDInputKey)
			return false;

		if (FAILED(m_pDInputKey->GetDeviceState(SIZE_KEYSTATE, m_KeyState)))
		{
			hr = m_pDInputKey->Acquire();
			while (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
				hr = m_pDInputKey->Acquire();
			return true;
		}
	}
	else
	{
		if (!m_pDInputKey)
			return false;

		ZeroMemory(m_KeyDOD, sizeof(DIDEVICEOBJECTDATA) * SIZE_KEYDOD);

		m_dwElements = SIZE_KEYDOD;
		hr = m_pDInputKey->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), m_KeyDOD, &m_dwElements, 0);
		if (hr != DI_OK)
		{
			m_dwElements = 0;
			hr = m_pDInputKey->Acquire();
			while (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
				hr = m_pDInputKey->Acquire();
			return true;
		}
	}

	return true;
}

bool bgDInput::MouseProcess()
{
	HRESULT hr;

	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
	if (!m_pDInputMouse)
		return false;

	if (FAILED(m_pDInputMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState)))
	{
		hr = m_pDInputMouse->Acquire();
		while (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			hr = m_pDInputMouse->Acquire();
		return true;
	}

	m_iMouseX += m_MouseState.lX;
	m_iMouseY += m_MouseState.lY;
	m_iMouseZ += m_MouseState.lZ;

	return true;
}

bool bgDInput::PreProcess()
{
	memcpy(&m_KeyStateBefore, &m_KeyState, sizeof(BYTE) * SIZE_KEYSTATE);
	memcpy(&m_MouseStateBefore, &m_MouseState, sizeof(DIMOUSESTATE));

	return true;
}

bool bgDInput::IsKeyUp(DWORD dwKey)
{
	if (m_dwImmediate)
	{
		if (m_KeyStateBefore[dwKey] & 0x80)
		{
			if (KEY_UP(dwKey))
				return true;
		}
	}
	else
	{
		for (DWORD i = 0; i < m_dwElements; i++)
		{
			if ((m_KeyDOD[i].dwOfs == dwKey) && !(m_KeyDOD[i].dwData & 0x80))
				return true;
		}
	}
	return false;
}

bool bgDInput::IsKeyDown(DWORD dwKey)
{
	if (m_dwImmediate)
	{
		if (KEY_DOWN(dwKey))
			return true;
	}
	else
	{
		for (DWORD i = 0; i < m_dwElements; i++)
		{
			if ((m_KeyDOD[i].dwOfs == dwKey) && (m_KeyDOD[i].dwData & 0x80))
				return true;
		}
	}
	return false;
}

void bgDInput::SetAcquire(bool bActive)
{
	if (bActive)
		DeviceAcquire();
	else
		DeviceUnacquire();
}

void bgDInput::DeviceAcquire()
{
	if (m_pDInputKey)
		m_pDInputKey->Acquire();
	if (m_pDInputMouse)
		m_pDInputMouse->Acquire();
}

void bgDInput::DeviceUnacquire()
{
	if (m_pDInputKey)
		m_pDInputKey->Unacquire();
	if (m_pDInputMouse)
		m_pDInputMouse->Unacquire();
}
