#include "bgTexture.h"

bgTexture::bgTexture()
{
	Init();
}

bgTexture::~bgTexture()
{
	//SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pTextureSRV);
}

bool bgTexture::Init()
{
	//m_pTexture = NULL;
	return true;
}

HRESULT bgTexture::Load(LPCWSTR filename, UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;
/*
	ID3D11Resource* pRes = NULL;
	D3DX11_IMAGE_LOAD_INFO loadInfo;
	loadInfo.Width = (iWidth) ? iWidth : g_pDevice->m_descBackBuffer.Width;
	loadInfo.Height = (iHeight) ? iHeight : g_pDevice->m_descBackBuffer.Height;
	loadInfo.Depth = D3DX11_DEFAULT;
	loadInfo.FirstMipLevel = 0;
	loadInfo.MipLevels = 1;
	loadInfo.Usage = D3D11_USAGE_DEFAULT;
	loadInfo.BindFlags = 0;
	loadInfo.CpuAccessFlags = 0;
	loadInfo.MiscFlags = 0;
	loadInfo.Format = g_pDevice->m_descBackBuffer.Format;
	loadInfo.Filter = D3DX11_FILTER_LINEAR;
	loadInfo.MipFilter = D3DX11_FILTER_NONE;
	loadInfo.pSrcInfo = NULL;

	HR_RETURN(D3DX11CreateTextureFromFile(g_pDevice->m_pDevice, filename, &loadInfo, NULL, &pRes, NULL));
	pRes->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&m_pTexture);
	pRes->Release();

	if (m_pTexture == NULL)
	{
		return false;
	}
	m_pTexture->GetDesc(&m_TextureDesc);
*/	
	/* ///////// 전체문자열로 구분하기 위하여 요소별 분리하지 않음
	// 파일경로를 요소별로 분리 후, [파일명.확장자] 형태로 멤버변수(m_szName)에 저장
	TCHAR szDrive[MAX_PATH] = { 0, };
	TCHAR szDir[MAX_PATH] = { 0, };
	TCHAR szName[MAX_PATH] = { 0, };
	TCHAR szExt[MAX_PATH] = { 0, };
	_tsplitpath_s(filename, szDrive, szDir, szName, szExt);
	ZeroMemory(m_szName, sizeof(TCHAR) * MAX_PATH);
	_stprintf_s(m_szName, _T("%s%s"), szName, szExt);
	*/
	//_stprintf_s(m_szName, _T("%s"), filename);

	HR_RETURN(D3DX11CreateShaderResourceViewFromFile(g_pDevice->m_pDevice, filename, NULL, NULL, &m_pTextureSRV, NULL));

	return hr;
}

bool bgTexture::Apply()
{
	if (m_pTextureSRV == NULL)
		return false;

	g_pDevice->m_pDContext->PSSetShaderResources(0, 1, &m_pTextureSRV);

	return true;
}
