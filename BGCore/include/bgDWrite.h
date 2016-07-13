#pragma once
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <string.h>
#include <commctrl.h>
#include <wchar.h>
#include <d3dx11.h>
#include <string.h>
#include <windows.h>
#include <intsafe.h>
#include <windowsx.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <new>
#include <vector>

using namespace std;

class bgDWrite
{
public:
	float					m_fDPIScaleX;
	float					m_fDPIScaleY;
	FLOAT					m_fdpiX;
	FLOAT					m_fdpiY;
	HWND					m_hWnd;
	ID2D1RenderTarget*		m_pRT;
	ID2D1Factory*			m_pD2DFactory;
	ID2D1SolidColorBrush*	m_pBlackBrush;
	IDWriteFactory*			m_pDWriteFactory;
	IDWriteTextFormat*		m_pTextFormat;
	IDWriteTextLayout*		m_pTextLayout;

public:
	DWRITE_FONT_WEIGHT		m_fontWeight;
	DWRITE_FONT_STYLE		m_fontStyle;
	BOOL					m_fontUnderline;
	wstring					m_wszFontFamily;
	float					m_fontSize;
	wstring					m_wszText;
	UINT32					m_cTextLength;

public:
	bgDWrite();
	virtual ~bgDWrite();

public:
	bool			Set(HWND hWnd, int iWidth, int iHeight, IDXGISurface1*	m_pSurface);
	bool			Init();
	bool			Begin();
	HRESULT			DrawText(RECT rc, TCHAR* pText, D2D1::ColorF Color = D2D1::ColorF(1, 0, 0, 1));
	HRESULT			DrawText(D2D1_POINT_2F origin, D2D1::ColorF Color = D2D1::ColorF(1, 0, 0, 1));
	bool			End();
	bool			Release();
	HRESULT			CreateDeviceIndependentResources();
	void			DiscardDeviceIndependentResources();
	HRESULT			CreateDeviceResources(IDXGISurface1* m_pSurface);
	void			DiscardDeviceResources();

public:
	HRESULT			SetText(D2D1_POINT_2F pos, wchar_t *text, D2D1::ColorF Color);
	HRESULT			SetFont(wchar_t *fontFamily);
	HRESULT			SetFontSize(float size);
	HRESULT			SetBold(bool bold);
	HRESULT			SetItalic(bool italic);
	HRESULT			SetUnderline(bool underline);
	void			OnResize(UINT width, UINT height, IDXGISurface1*pSurface);
};
