#include "bgDInput.h"

bgDInput::bgDInput()
{
}

bgDInput::~bgDInput()
{
	Release();
}

bool bgDInput::Init()
{
	HRESULT hr = S_OK;

	BOOL_RETURN(Create());

	return true;
}

bool bgDInput::Frame()
{
	KeyProcess();
	return true;
}

bool bgDInput::Render()
{
	PrintLog();
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

void bgDInput::PrintLog()
{
	int iKey, iCountLine = 0;
	for (iKey = 0; iKey < SIZE_KEYSTATE; iKey++)
	{
		if (I_DInput.m_KeyState[iKey] & 0x80)
		{
			TCHAR pBuffer[256] = { 0, };
			_stprintf_s(pBuffer, _T("Key State : 0x%02x : %d"), iKey, I_DInput.m_KeyState[iKey]);
			iCountLine++;
		}
	}
	for (iKey = 0; iKey < 4; iKey++)
	{
		if (I_DInput.m_MouseState.rgbButtons[iKey] & 0x80)
		{
			TCHAR pBuffer[256] = { 0, };
			_stprintf_s(pBuffer, _T("Mouse Button State : %d"), iKey);
			iCountLine++;
		}
	}
}

HRESULT bgDInput::Create()
{
	HRESULT	hr = S_OK;

	HR_RETURN(DirectInput8Create(g_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDInput, NULL));
	HR_RETURN(m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pDInputKey, NULL));
	HR_RETURN(m_pDInput->CreateDevice(GUID_SysMouse, &m_pDInputMouse, NULL));

	HR_RETURN(m_pDInputKey->SetDataFormat(&c_dfDIKeyboard));
	HR_RETURN(m_pDInputMouse->SetDataFormat(&c_dfDIMouse));

	if (FAILED(hr = m_pDInputKey->SetCooperativeLevel(g_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
	{
		while (m_pDInputKey->Acquire() == DIERR_INPUTLOST)
			; // none
		HR_RETURN(m_pDInputKey->SetCooperativeLevel(g_hWnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND));
	}

	if (FAILED(hr = m_pDInputMouse->SetCooperativeLevel(g_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
	{
		while (m_pDInputMouse->Acquire() == DIERR_INPUTLOST)
			; // none
		HR_RETURN(m_pDInputMouse->SetCooperativeLevel(g_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND));
	}

	return hr;
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
			while (m_pDInputKey->Acquire() == DIERR_INPUTLOST)
				m_pDInputKey->Acquire();
			return true;
		}
	}
	else
	{
		ZeroMemory(m_KeyDOD, sizeof(DIDEVICEOBJECTDATA) * SIZE_KEYDOD);
		if (!m_pDInputKey)
			return false;

		m_dwElements = SIZE_KEYSTATE;
		hr = m_pDInputKey->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), m_KeyDOD, &m_dwElements, 0);
		if (hr != DI_OK)
		{
			hr = m_pDInputKey->Acquire();
			while(hr == DIERR_INPUTLOST)
				hr = m_pDInputKey->Acquire();
			return true;
		}
	}
	return true;
}

bool bgDInput::MouseProcess()
{
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
	if (!m_pDInputMouse)
		return false;

	if (FAILED(m_pDInputMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState)))
	{
		while (m_pDInputMouse->Acquire() == DIERR_INPUTLOST)
			m_pDInputMouse->Acquire();
		return true;
	}
	return true;
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
