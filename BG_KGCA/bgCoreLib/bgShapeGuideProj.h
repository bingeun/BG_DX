#pragma once
#include "bgShape.h"

class bgShapeGuideProj : public bgShape
{
public:
	bgShapeGuideProj();
	virtual ~bgShapeGuideProj();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	HRESULT	Create();
	HRESULT	Load();
};
