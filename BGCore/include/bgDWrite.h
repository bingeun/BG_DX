#pragma once
#include "bgStd.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "WindowsCodecs.lib")

class bgDWrite
{
public:
	HWND		m_hWnd;

public:
	ID2D1RenderTarget*		m_pRenderTarget;
	ID2D1Factory*			m_pD2DFactory;
	IDWriteFactory*			m_pDWriteFactory;
	IDXGISurface1*			m_pSurface;

	IDWriteTextFormat*		m_pTextFormat;
	IDWriteTextLayout*		m_pTextLayout;
	ID2D1SolidColorBrush*	m_pBrushFontColor;

public:
	FLOAT		m_fDPIX;
	FLOAT		m_fDPIY;
	float		m_fDPIScaleX;
	float		m_fDPIScaleY;

	DWRITE_FONT_WEIGHT	m_eFontWeight;
	DWRITE_FONT_STYLE	m_eFontStyle;
	BOOL		m_bFontUnderline;
	wstring		m_wszFontFamily;
	float		m_fFontSize;
	wstring		m_wszText;
	UINT32		m_iTextLength;

	bool		m_bFontBorder; // 검정색 외곽선 유무

public:
	bgDWrite();
	virtual ~bgDWrite();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	bool	PreRender();
	bool	PostRender();

public:
	HRESULT	CreateResources();
	void	DiscardResources();
	HRESULT	CreateDeviceResources(IDXGISurface1* pSurface);
	void	DiscardDeviceResources();

public:
	bool	Set(HWND hWnd, int iWidth, int iHeight, IDXGISurface1* pSurface);
	void	Resize(UINT iWidth, UINT iHeight, IDXGISurface1* pSurface);

	HRESULT	SetText(D2D1_POINT_2F size, wchar_t* text, D2D1::ColorF Color);
	HRESULT	SetFont(wchar_t* fontFamily);
	HRESULT	SetFontSize(float fSize);
	HRESULT	SetBold(bool bBold);
	HRESULT	SetItalic(bool bItalic);
	HRESULT	SetUnderline(bool bUnderline);

public:
	HRESULT	Print(RECT rect, TCHAR* pText, D2D1::ColorF Color = D2D1::ColorF(1, 1, 1, 1));
	HRESULT	Print(D2D1_POINT_2F pos, D2D1::ColorF Color = D2D1::ColorF(1, 1, 1, 1));
};
