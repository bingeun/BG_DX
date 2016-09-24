#pragma once
#include "bgStd.h"

#define I_Writer bgMaxWriter::GetInstance()

class bgMaxWriter : public bgSingleton<bgMaxWriter>
{
public:
	bgMaxWriter();
	virtual ~bgMaxWriter();
};
