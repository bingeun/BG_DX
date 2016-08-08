#pragma once
#include "bgStd.h"
#include "bgSys.h"

class bgTexture
{
public:
	bgTexture();
	virtual ~bgTexture();

public:
	ID3D11Texture2D*	GetTexture2DFromFile(LPCWSTR filename, D3DX11_IMAGE_LOAD_INFO* pLoadInfo);
};
