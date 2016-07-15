#pragma once
#include "bgShape.h"

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

	HRESULT	Create();

public:
};
