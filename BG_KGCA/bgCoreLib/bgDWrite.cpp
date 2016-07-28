#include "bgDWrite.h"

bgDWrite::bgDWrite()
{
	m_hWnd = NULL;
	m_pRenderTarget = NULL;
	m_pD2DFactory = NULL;
	m_pDWriteFactory = NULL;
	m_pSurface = NULL;
	m_pTextFormat = NULL;
	m_pTextLayout = NULL;
	m_pBrushFontColor = NULL;

	m_eFontWeight = DWRITE_FONT_WEIGHT_NORMAL;
	m_eFontStyle = DWRITE_FONT_STYLE_NORMAL;
	m_bFontUnderline = FALSE;
	m_fFontSize = 15.0f;
	m_iTextLength = 0;

	m_bFontBorder = false;
}

bgDWrite::~bgDWrite()
{
	Release();
}

bool bgDWrite::Init()
{
	const wchar_t fontFamily[] = L"Consolas"; //L"맑은 고딕"; Consolas
	m_wszFontFamily = fontFamily;
	return true;
}

bool bgDWrite::Frame()
{
	return true;
}

bool bgDWrite::Render()
{
	PreRender();
	PostRender();
	return true;
}

bool bgDWrite::Release()
{
	m_wszText.clear();
	m_wszFontFamily.clear();
	DiscardResources();
	DiscardDeviceResources();

	return true;
}

bool bgDWrite::PreRender()
{
	if (m_pRenderTarget)
	{
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::IdentityMatrix());
		// m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	}
	return true;
}

bool bgDWrite::PostRender()
{
	if (m_pRenderTarget && FAILED(m_pRenderTarget->EndDraw()))
	{
		return false;
	}
	return true;
}

HRESULT bgDWrite::CreateResources()
{
	HRESULT hr = S_OK;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	m_pD2DFactory->GetDesktopDpi(&m_fDPIX, &m_fDPIY);
	m_fDPIScaleX = m_fDPIX / 96.0f;
	m_fDPIScaleY = m_fDPIY / 96.0f;

	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pDWriteFactory->CreateTextFormat(m_wszFontFamily.c_str(), NULL,
			m_eFontWeight, m_eFontStyle, DWRITE_FONT_STRETCH_NORMAL, m_fFontSize, L"ko-KR", &m_pTextFormat);
	}

	return hr;
}

void bgDWrite::DiscardResources()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextFormat);
	SAFE_RELEASE(m_pTextLayout);
}

HRESULT bgDWrite::CreateDeviceResources(IDXGISurface1* pSurface)
{
	HRESULT hr = S_OK;

	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), m_fDPIX, m_fDPIY);

	HR_RETURN(m_pD2DFactory->CreateDxgiSurfaceRenderTarget(pSurface, (const D2D1_RENDER_TARGET_PROPERTIES *)&props, &m_pRenderTarget));
	HR_RETURN(m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), &m_pBrushFontColor));

	return hr;
}

void bgDWrite::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBrushFontColor);
}

bool bgDWrite::Set(HWND hWnd, int iWidth, int iHeight, IDXGISurface1* pSurface)
{
	HRESULT hr = S_OK;

	if (!Init())
		return false;

	m_hWnd = hWnd;

	hr = CreateResources();
	hr = CreateDeviceResources(pSurface);

	SetText(D2D1::Point2F(iWidth, iHeight), L"TEXT 텍스트 셋 Set()...!", D2D1::ColorF(1, 1, 1, 1));

	return true;
}

void bgDWrite::Resize(UINT iWidth, UINT iHeight, IDXGISurface1* pSurface)
{
	DiscardDeviceResources();
	CreateDeviceResources(pSurface);

	SetText(D2D1::Point2F(iWidth, iHeight), L"TEXT 텍스트 리사이즈 Resize()...!!!", D2D1::ColorF(1, 1, 1, 1));
}

HRESULT bgDWrite::SetText(D2D1_POINT_2F size, wchar_t* text, D2D1::ColorF Color)
{
	HRESULT hr = S_OK;

	m_iTextLength = (UINT32)wcslen(text);
	m_wszText.clear();
	m_wszText = text;

	SAFE_RELEASE(m_pTextLayout);
	if (SUCCEEDED(hr))
		hr = m_pDWriteFactory->CreateTextLayout(m_wszText.c_str(), m_iTextLength, m_pTextFormat, size.x, size.y, &m_pTextLayout);

	IDWriteTypography* pTypography = NULL;
	if (SUCCEEDED(hr))
		hr = m_pDWriteFactory->CreateTypography(&pTypography);

	DWRITE_FONT_FEATURE fontFeature = { DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7, 1 };
	if (SUCCEEDED(hr))
		hr = pTypography->AddFontFeature(fontFeature);

	DWRITE_TEXT_RANGE textRange = { 0, m_iTextLength };
	if (SUCCEEDED(hr))
		hr = m_pTextLayout->SetTypography(pTypography, textRange);

	if (SUCCEEDED(hr))
		hr = m_pTextLayout->SetUnderline(m_bFontUnderline, textRange);

	SAFE_RELEASE(pTypography);

	return hr;
}

HRESULT bgDWrite::SetFont(wchar_t* fontFamily)
{
	HRESULT hr = S_OK;

	DWRITE_TEXT_RANGE textRange = { 0, m_iTextLength };

	hr = m_pTextLayout->SetFontFamilyName(fontFamily, textRange);
	if (SUCCEEDED(hr))
	{
		m_wszFontFamily.clear();
		m_wszFontFamily = fontFamily;
	}

	return hr;
}

HRESULT bgDWrite::SetFontSize(float fSize)
{
	HRESULT hr = S_OK;

	DWRITE_TEXT_RANGE textRange = { 0, m_iTextLength };

	hr = m_pTextLayout->SetFontSize(fSize, textRange);
	if (SUCCEEDED(hr))
	{
		m_fFontSize = fSize;
	}

	return hr;
}

HRESULT bgDWrite::SetBold(bool bBold)
{
	HRESULT hr = S_OK;

	DWRITE_TEXT_RANGE textRange = { 0, m_iTextLength };

	if (bBold)
		m_eFontWeight = DWRITE_FONT_WEIGHT_BOLD;
	else
		m_eFontWeight = DWRITE_FONT_WEIGHT_NORMAL;

	hr = m_pTextLayout->SetFontWeight(m_eFontWeight, textRange);

	return hr;
}

HRESULT bgDWrite::SetItalic(bool bItalic)
{
	HRESULT hr = S_OK;

	DWRITE_TEXT_RANGE textRange = { 0, m_iTextLength };

	if (bItalic)
		m_eFontStyle = DWRITE_FONT_STYLE_ITALIC;
	else
		m_eFontStyle = DWRITE_FONT_STYLE_NORMAL;

	hr = m_pTextLayout->SetFontStyle(m_eFontStyle, textRange);

	return hr;
}

HRESULT bgDWrite::SetUnderline(bool bUnderline)
{
	HRESULT hr = S_OK;

	DWRITE_TEXT_RANGE textRange = { 0, m_iTextLength };
	m_bFontUnderline = bUnderline;
	hr = m_pTextLayout->SetUnderline(m_bFontUnderline, textRange);

	return hr;
}

HRESULT bgDWrite::Print(RECT rect, TCHAR* pText, D2D1::ColorF Color)
{
	HRESULT hr = S_OK;

	if (m_pRenderTarget && m_pBrushFontColor)
	{
		D2D1_RECT_F rectLayout = D2D1::RectF(
			static_cast<FLOAT>(rect.left) / m_fDPIScaleX, static_cast<FLOAT>(rect.top) / m_fDPIScaleY,
			static_cast<FLOAT>(rect.right) / m_fDPIScaleX, static_cast<FLOAT>(rect.bottom) / m_fDPIScaleY);
		m_pBrushFontColor->SetColor(Color);
		m_pRenderTarget->DrawText(pText, wcslen(pText), m_pTextFormat, rectLayout, m_pBrushFontColor);
	}

	return hr;
}

HRESULT bgDWrite::Print(D2D1_POINT_2F pos, D2D1::ColorF Color)
{
	HRESULT hr = S_OK;

	D2D1_POINT_2F origin = D2D1::Point2F(static_cast<FLOAT>(pos.x / m_fDPIScaleX), static_cast<FLOAT>(pos.y / m_fDPIScaleY));
	m_pBrushFontColor->SetColor(Color);
	m_pRenderTarget->DrawTextLayout(origin, m_pTextLayout, m_pBrushFontColor);

	return hr;
}
