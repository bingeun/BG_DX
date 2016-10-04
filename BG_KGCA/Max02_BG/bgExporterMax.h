#pragma once
#include "bgAnimMax.h"

#define I_Exporter bgExporterMax::GetInstance()

class bgExporterMax : public bgAnimMax, public bgSingleton<bgExporterMax>
{
public:

public:
	bgExporterMax();
	virtual ~bgExporterMax();

public:
	void	ExportBG3D();
	void	ExportBGSKN();
	void	ExportBGMTX();
	void	ExportBGACT();
};
