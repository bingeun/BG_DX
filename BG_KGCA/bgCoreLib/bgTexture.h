#pragma once
#include "bgStd.h"
#include "bgSys.h"

class bgTexture
{
public:
	ID3D11Texture2D*		m_pTexture;
	D3D11_TEXTURE2D_DESC	m_TextureDesc;

public:
	bgTexture();
	virtual ~bgTexture();

public:
	HRESULT		Load(LPCWSTR filename, D3DX11_IMAGE_LOAD_INFO* pLoadInfo);
};
