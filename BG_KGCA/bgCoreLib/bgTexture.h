#pragma once
#include "bgStd.h"
#include "bgSys.h"

class bgTexture
{
public:
	TCHAR					m_szName[MAX_PATH];
	//ID3D11Texture2D*		m_pTexture;
	//D3D11_TEXTURE2D_DESC	m_TextureDesc;
	ID3D11ShaderResourceView*	m_pTextureSRV;

public:
	bgTexture();
	virtual ~bgTexture();

public:
	bool		Init();
	HRESULT		Load(LPCWSTR filename, UINT iWidth = 0, UINT iHeight = 0);
	bool		Apply();
};
