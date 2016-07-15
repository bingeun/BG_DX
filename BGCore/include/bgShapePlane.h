#pragma once
#include "bgShape.h"

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
};
