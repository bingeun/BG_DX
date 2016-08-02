#pragma once
#include "bgShape.h"
#include "bgSys.h"

class bgShapeBox : public bgShape
{
public:
	bgShapeBox();
	virtual ~bgShapeBox();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	HRESULT	CreateBuffer(float fSize = 1.0f);
	HRESULT	LoadShader();
};
