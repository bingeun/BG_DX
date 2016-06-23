#include "bgInput.h"

// 입력 관련 전역변수
//BGINPUT_MAP g_InputData;



bool bgInput::ResetDevice()
{
	Release();
	Init();
	return true;
}

bool bgInput::Render()
{
	return true;
}

bool bgInput::Init()
{
	memset(&m_KeyBeforeState, 0, sizeof(BYTE) * MAX_KEYSTATE);
	memset(&m_MouseBeforeState, 0, sizeof(DIMOUSESTATE));
	m_dwElements = 0;
	return true;
}

bool bgInput::Frame()
{
	if (!KeyProcess() || !MouseProcess())
		return false;
/*
	memset(&g_InputData, 0, sizeof(BGINPUT_MAP));

	if (m_MouseState.rgbButtons[0])
		g_InputData.bLeftClick = true;
	if (m_MouseState.rgbButtons[1])
		g_InputData.bRightClick = true;
	if (m_MouseState.rgbButtons[2])
		g_InputData.bMiddleClick = true;

	g_InputData.iMouseValue[0] = m_MouseState.lX;
	g_InputData.iMouseValue[1] = m_MouseState.lY;
	g_InputData.iMouseValue[2] = m_MouseState.lZ;

	g_InputData.bAKey = IsKeyDown(DIK_A);
	g_InputData.bDKey = IsKeyDown(DIK_D);
	g_InputData.bWKey = IsKeyDown(DIK_W);
	g_InputData.bSKey = IsKeyDown(DIK_S);
	g_InputData.bQKey = IsKeyDown(DIK_Q);
	g_InputData.bEKey = IsKeyDown(DIK_E);

	g_InputData.bLeftKey = IsKeyDown(DIK_LEFT);
	g_InputData.bRightKey = IsKeyDown(DIK_RIGHT);
	g_InputData.bUpKey = IsKeyDown(DIK_UP);
	g_InputData.bDownKey = IsKeyDown(DIK_DOWN);
	g_InputData.bExit = IsKeyDown(DIK_ESCAPE);
	g_InputData.bSpace = IsKeyDown(DIK_SPACE);
	g_InputData.bExit = IsKeyDown(DIK_ESCAPE);

	g_InputData.bChangeFillMode = OneKeyDown(DIK_F5);
	g_InputData.bChangePrimitive = OneKeyDown(DIK_F6);
	g_InputData.bChangeCullMode = OneKeyDown(DIK_F7);
	g_InputData.bChangeCameraType = OneKeyDown(DIK_F8);
*/
	return true;
}

bool bgInput::OneKeyDown(DWORD dwKey)
{
	if (I_Input.m_KeyState[dwKey] & 0x80 && !(I_Input.m_KeyBeforeState[dwKey] & 0x80))
	{
		return true;
	}
	return false;
}

bool bgInput::IsKeyDown(DWORD dwKey)
{
	if (m_dwImmediate)
	{
		if (KEYDOWN(dwKey))
			return true;
	}
	else
	{
		for (DWORD i = 0; i < m_dwElements; i++)
		{
			if (m_Didod[i].dwOfs == dwKey && (m_Didod[i].dwData & 0x80))
			{
				return true;
			}
		}
	}
	return false;
}

bool bgInput::IsKeyUP(DWORD dwKey)
{
	if (m_dwImmediate)
	{
		if (m_KeyBeforeState[dwKey] & 0x80)
		{
			if (KEYUP(dwKey))
				return true;
		}
	}
	else
	{
		for (DWORD i = 0; i < m_dwElements; i++)
		{
			if (m_Didod[i].dwOfs == dwKey && !(m_Didod[i].dwData & 0x80))
			{
				return true;
			}
		}
	}


	return false;
}

bool bgInput::PostProcess()
{
	memcpy(&m_KeyBeforeState, &m_KeyState, sizeof(BYTE) * MAX_KEYSTATE);
	memcpy(&m_MouseBeforeState, &m_MouseState, sizeof(DIMOUSESTATE));
	return true;
}

bool bgInput::InitDirectInput(HINSTANCE hInst, HWND hWnd, bool keyboard, bool mouse)
{
	HRESULT hr;
	if (m_hWnd == NULL)
	{
		m_hWnd = hWnd;
	}
	if (m_pDidevKey || m_pDidevMouse)
		return true;
	if (FAILED(hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDi, NULL)))
	{
		if (DIERR_BETADIRECTINPUTVERSION == hr)
		{
			int kkk = 0;
		}
		else if (DIERR_INVALIDPARAM == hr)
		{
			int kkk = 0;
		}
		else if (DIERR_OLDDIRECTINPUTVERSION == hr)
		{
			int kkk = 0;
		}
		else if (DIERR_OUTOFMEMORY == hr)
		{
			int kkk = 0;
		}

		return false;
	}

	if (keyboard)
	{
		if (FAILED(m_pDi->CreateDevice(GUID_SysKeyboard, &m_pDidevKey, NULL)))
			return false;
		if (FAILED(m_pDidevKey->SetDataFormat(&c_dfDIKeyboard)))
			return false;
		if (FAILED(hr = m_pDidevKey->SetCooperativeLevel(m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY)))
		{
			while (m_pDidevKey->Acquire() == DIERR_INPUTLOST);
			if (FAILED(m_pDidevKey->SetCooperativeLevel(m_hWnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND | DISCL_NOWINKEY)))
				return false;
		}
		if (!m_dwImmediate)
		{
			DIPROPDWORD dipdw;

			dipdw.diph.dwSize = sizeof(DIPROPDWORD);
			dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			dipdw.diph.dwObj = 0;
			dipdw.diph.dwHow = DIPH_DEVICE;
			dipdw.dwData = DIDOD_BUFFER_SIZE;

			if (FAILED(hr = m_pDidevKey->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
				return false;
		}

		while (m_pDidevKey->Acquire() == DIERR_INPUTLOST);
	}
	if (mouse)
	{
		if (FAILED(m_pDi->CreateDevice(GUID_SysMouse, &m_pDidevMouse, NULL)))
			return false;
		if (FAILED(m_pDidevMouse->SetDataFormat(&c_dfDIMouse)))
			return false;
		if (FAILED(m_pDidevMouse->SetCooperativeLevel(m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
		{
			while (m_pDidevMouse->Acquire() == DIERR_INPUTLOST);
			if (FAILED(m_pDidevMouse->SetCooperativeLevel(m_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
				return false;

		}
		while (m_pDidevMouse->Acquire() == DIERR_INPUTLOST);
	}

	return true;
}

bool bgInput::Release()
{
	if (m_pDi)
	{
		if (m_pDidevKey)
		{
			m_pDidevKey->Unacquire();
			m_pDidevKey->Release();
			m_pDidevKey = NULL;
		}
		if (m_pDidevMouse)
		{
			m_pDidevMouse->Unacquire();
			m_pDidevMouse->Release();
			m_pDidevMouse = NULL;
		}

		m_pDi->Release();
		m_pDi = NULL;
	}
	return true;
}

bool bgInput::KeyProcess()
{
	HRESULT hr;
	if (m_dwImmediate)
	{
		ZeroMemory(m_KeyState, sizeof(BYTE) * MAX_KEYSTATE);
		if (!m_pDidevKey)
			return false;
		if (FAILED(m_pDidevKey->GetDeviceState(MAX_KEYSTATE, m_KeyState)))
		{
			while (m_pDidevKey->Acquire() == DIERR_INPUTLOST)
				m_pDidevKey->Acquire();
			return true;
		}
	}
	else
	{
		if (NULL == m_pDidevKey)
			return false;
		memset(&m_Didod, 0, sizeof(DIDEVICEOBJECTDATA) * DIDOD_BUFFER_SIZE);
		m_dwElements = DIDOD_BUFFER_SIZE;
		hr = m_pDidevKey->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), m_Didod, &m_dwElements, 0);
		if (hr != DI_OK)
		{
			hr = m_pDidevKey->Acquire();
			while (hr == DIERR_INPUTLOST)
				hr = m_pDidevKey->Acquire();
			return true;
		}
	}
	return true;
}

bool bgInput::MouseProcess()
{
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
	if (!m_pDidevMouse)
		return false;

	if (FAILED(m_pDidevMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState)))
	{
		while (m_pDidevMouse->Acquire() == DIERR_INPUTLOST)
			m_pDidevMouse->Acquire();

		return true;
	}

	return true;
}

void bgInput::DeviceAcquire()
{
	if (m_pDidevKey)
		m_pDidevKey->Acquire();
	if (m_pDidevMouse)
		m_pDidevMouse->Acquire();
}

void bgInput::DeviceUnacquire()
{
	if (m_pDidevKey)
		m_pDidevKey->Unacquire();
	if (m_pDidevMouse)
		m_pDidevMouse->Unacquire();
}

bgInput::bgInput()
{
	m_pDi = NULL;
	m_pDidevKey = NULL;
	m_pDidevMouse = NULL;
	m_hWnd = NULL;
	m_dwElements = 0;
	m_dwImmediate = TRUE;
}

bgInput::~bgInput()
{
	Release();
}
