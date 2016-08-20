#include "bgTexture.h"

bgTexture::bgTexture()
{
	m_pTexture = NULL;
}

bgTexture::~bgTexture()
{
	SAFE_RELEASE(m_pTexture);
}

HRESULT bgTexture::Load(LPCWSTR filename, D3DX11_IMAGE_LOAD_INFO* pLoadInfo)
{
	HRESULT hr = S_OK;

	ID3D11Resource* pRes = NULL;
	HR_RETURN(D3DX11CreateTextureFromFile(g_pDevice->m_pDevice, filename, pLoadInfo, NULL, &pRes, NULL));

	pRes->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&m_pTexture);
	pRes->Release();

	return hr;
}
