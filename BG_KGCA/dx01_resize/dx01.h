#pragma once
#include "bgCore.h"

class dx01 : public bgCore
{
public:
	bool	m_bWireFrameRender;
	int		m_iPrimitiveType;

	ID3D11RasterizerState*	m_pRasterizerStateNoCull;

public:
	HRESULT		SetRasterizerState(D3D11_FILL_MODE d3dFillMode = D3D11_FILL_SOLID);
	LRESULT		MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	dx01();
	virtual ~dx01();
};
