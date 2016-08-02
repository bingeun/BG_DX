#pragma once
#include "bgShape.h"
#include "bgSys.h"

class bgShapePlane : public bgShape
{
public:
	bgShapePlane();
	virtual ~bgShapePlane();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	HRESULT	CreateBuffer(float fSize = 1.0f);
	HRESULT	LoadShader();
};
