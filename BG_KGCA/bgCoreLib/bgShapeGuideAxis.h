#pragma once
#include "bgShape.h"
#include "bgSys.h"

class bgShapeGuideAxis : public bgShape
{
public:
	bgShapeGuideAxis();
	virtual ~bgShapeGuideAxis();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	HRESULT	CreateBuffer(float fLength = 1.0f);
	HRESULT	LoadShader();
};
