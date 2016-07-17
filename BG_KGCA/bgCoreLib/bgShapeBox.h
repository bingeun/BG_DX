#pragma once
#include "bgShape.h"

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

	HRESULT	Create();
	HRESULT	Load();
};
