#pragma once
#include "bgStd.h"

class bgBase
{
public:
	virtual bool Init() = 0;
	virtual bool Frame() = 0;
	virtual bool Render() = 0;
	virtual bool Release() = 0;

public:
	bgBase();
	virtual ~bgBase();
};
