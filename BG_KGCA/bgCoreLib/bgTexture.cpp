#include "bgTexture.h"

bgTexture::bgTexture()
{
}

bgTexture::~bgTexture()
{
}

ID3D11Texture2D* bgTexture::GetTexture2DFromFile(LPCWSTR filename, D3DX11_IMAGE_LOAD_INFO* pLoadInfo)
{
	ID3D11Texture2D* texture = NULL;
	ID3D11Resource*  pRes = NULL;

	HRESULT hr = D3DX11CreateTextureFromFile(g_pDevice, filename, pLoadInfo, NULL, &pRes, NULL);
	if (FAILED(hr))
	{
		return NULL;
	}
	pRes->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&texture);
	pRes->Release();
	return texture;
}