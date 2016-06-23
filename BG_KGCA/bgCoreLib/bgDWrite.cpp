#include "bgDWrite.h"
#include <math.h>



template <class T> inline void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

bool bgDWrite::Set(HWND hWnd, int iWidth, int iHeight, IDXGISurface1*	pSurface)
{
	HRESULT hr;
	if (!Init())
	{
		return false;
	}
	m_hWnd = hWnd;

	hr = CreateDeviceIndependentResources();
	hr = CreateDeviceResources(pSurface);
	SetText(D2D1::Point2F((FLOAT)iWidth, (FLOAT)iHeight), L"BG!", D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
	return true;
}

bool bgDWrite::Release()
{
	m_wszText.clear();
	m_wszFontFamily.clear();
	DiscardDeviceIndependentResources();
	DiscardDeviceResources();
	return true;
}

bool bgDWrite::Init()
{
	const wchar_t defaultText[] = L"Times New Roman";
	m_wszFontFamily = defaultText;
	return true;
}

HRESULT bgDWrite::CreateDeviceIndependentResources()
{
	HRESULT hr;
	// Create Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	m_pD2DFactory->GetDesktopDpi(&m_fdpiX, &m_fdpiY);
	m_fDPIScaleX = m_fdpiX / 96.0f;
	m_fDPIScaleY = m_fdpiY / 96.0f;

	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pDWriteFactory->CreateTextFormat(m_wszFontFamily.c_str(), NULL, m_fontWeight, m_fontStyle,
			DWRITE_FONT_STRETCH_NORMAL, 20, L"en-us", &m_pTextFormat);
	}
	return hr;
}

HRESULT bgDWrite::SetFont(wchar_t *fontFamily)
{
	HRESULT hr;

	DWRITE_TEXT_RANGE textRange = { 0, m_cTextLength };

	hr = m_pTextLayout->SetFontFamilyName(fontFamily, textRange);

	if (SUCCEEDED(hr))
	{
		m_wszFontFamily.clear();
		m_wszFontFamily = fontFamily;
	}
	return hr;
}

HRESULT bgDWrite::SetFontSize(float size)
{
	HRESULT hr;
	DWRITE_TEXT_RANGE textRange = { 0, m_cTextLength };

	hr = m_pTextLayout->SetFontSize(size, textRange);

	if (SUCCEEDED(hr))
	{
		m_fontSize = size;
	}
	return hr;
}

HRESULT bgDWrite::SetText(D2D1_POINT_2F size, wchar_t *text, D2D1::ColorF Color)
{
	HRESULT hr = S_OK;
	m_cTextLength = (UINT32)wcslen(text);
	m_wszText.clear();
	m_wszText = text;

	SafeRelease(&m_pTextLayout);
	if (SUCCEEDED(hr))
	{
		hr = m_pDWriteFactory->CreateTextLayout(m_wszText.c_str(), m_cTextLength, m_pTextFormat, size.x, size.y, &m_pTextLayout);
	}

	IDWriteTypography* pTypography = NULL;
	if (SUCCEEDED(hr))
	{
		hr = m_pDWriteFactory->CreateTypography(&pTypography);
	}

	DWRITE_FONT_FEATURE fontFeature = { DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7,1 };
	if (SUCCEEDED(hr))
	{
		hr = pTypography->AddFontFeature(fontFeature);
	}

	DWRITE_TEXT_RANGE textRange = { 0, m_cTextLength };

	if (SUCCEEDED(hr))
	{
		hr = m_pTextLayout->SetTypography(pTypography, textRange);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pTextLayout->SetUnderline(m_fontUnderline, textRange);
	}
	SafeRelease(&pTypography);

	return hr;
}

HRESULT bgDWrite::SetBold(bool bold)
{
	HRESULT hr;
	DWRITE_TEXT_RANGE textRange = { 0, m_cTextLength };

	if (bold)
	{
		m_fontWeight = DWRITE_FONT_WEIGHT_BOLD;
	}
	else
	{
		m_fontWeight = DWRITE_FONT_WEIGHT_NORMAL;
	}

	hr = m_pTextLayout->SetFontWeight(m_fontWeight, textRange);
	return hr;
}

HRESULT bgDWrite::SetItalic(bool italic)
{
	HRESULT hr;
	DWRITE_TEXT_RANGE textRange = { 0, m_cTextLength };

	if (italic)
	{
		m_fontStyle = DWRITE_FONT_STYLE_ITALIC;
	}
	else
	{
		m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
	}

	hr = m_pTextLayout->SetFontStyle(m_fontStyle, textRange);
	return hr;
}

HRESULT bgDWrite::SetUnderline(bool underline)
{
	HRESULT hr;
	DWRITE_TEXT_RANGE textRange = { 0, m_cTextLength };

	m_fontUnderline = underline;

	hr = m_pTextLayout->SetUnderline(m_fontUnderline, textRange);
	return hr;
}

void bgDWrite::DiscardDeviceIndependentResources()
{
	SafeRelease(&m_pD2DFactory);
	SafeRelease(&m_pDWriteFactory);
	SafeRelease(&m_pTextFormat);
	SafeRelease(&m_pTextFormat);
	SafeRelease(&m_pTextLayout);
}

HRESULT bgDWrite::DrawText(RECT rc, TCHAR* pText, D2D1::ColorF Color)
{
	if (m_pRT&&m_pBlackBrush)
	{
		D2D1_RECT_F layoutRect = D2D1::RectF(
			static_cast<FLOAT>(rc.left) / m_fDPIScaleX,
			static_cast<FLOAT>(rc.top) / m_fDPIScaleY,
			static_cast<FLOAT>(rc.right) / m_fDPIScaleX,
			static_cast<FLOAT>(rc.bottom) / m_fDPIScaleY);
		m_pBlackBrush->SetColor(Color);
		m_pRT->DrawText(pText, wcslen(pText), m_pTextFormat, layoutRect, m_pBlackBrush);
	}
	return S_OK;
}

HRESULT bgDWrite::DrawText(D2D1_POINT_2F pos, D2D1::ColorF Color)
{
	D2D1_POINT_2F origin = D2D1::Point2F(
		static_cast<FLOAT>(pos.x / m_fDPIScaleX),
		static_cast<FLOAT>(pos.y / m_fDPIScaleY));
	m_pBlackBrush->SetColor(Color);
	m_pRT->DrawTextLayout(origin, m_pTextLayout, m_pBlackBrush);
	return S_OK;
}

bool bgDWrite::Begin()
{
	if (m_pRT)
	{
		m_pRT->BeginDraw();
		m_pRT->SetTransform(D2D1::IdentityMatrix());
	}
	return true;
}

bool bgDWrite::End()
{
	if (m_pRT && FAILED(m_pRT->EndDraw()))
	{
		return false;
	}
	return true;
}

HRESULT bgDWrite::CreateDeviceResources(IDXGISurface1* pSurface)
{
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), static_cast<float>(m_fdpiX), static_cast<float>(m_fdpiY));

	HRESULT hr = S_OK;
	if (FAILED(hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(pSurface, (const D2D1_RENDER_TARGET_PROPERTIES *)&props, &m_pRT)))
		return hr;

	if (FAILED(hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pBlackBrush)))
		return hr;

	return S_OK;
}

void bgDWrite::DiscardDeviceResources()
{
	SafeRelease(&m_pRT);
	SafeRelease(&m_pBlackBrush);
}

void bgDWrite::OnResize(UINT width, UINT height, IDXGISurface1*	pSurface)
{
	DiscardDeviceResources();
	CreateDeviceResources(pSurface);
	SetText(D2D1::Point2F((FLOAT)width, (FLOAT)height), L"BG!", D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
}

bgDWrite::bgDWrite()
{
	m_pBlackBrush = NULL;
	m_pDWriteFactory = NULL;
	m_pTextFormat = NULL;
	m_pD2DFactory = NULL;
	m_pTextLayout = NULL;
	m_pRT = NULL;
	m_hWnd = NULL;
	m_cTextLength = 0;
	m_fontWeight = DWRITE_FONT_WEIGHT_NORMAL;
	m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
	m_fontUnderline = FALSE;
	m_fontSize = 72.0f;
}

bgDWrite::~bgDWrite()
{
}
