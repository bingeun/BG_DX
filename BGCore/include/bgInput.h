#pragma once
#include "bgStd.h"
#include "bgSys.h"
#include <dinput.h>

#define I_Input bgInput::GetInstance()

class bgInput : public bgSingleton<bgInput>
{
private:
	friend class bgSingleton<bgInput>;

public:

public:
	bgInput();
	virtual ~bgInput();

public:
	bool		Init();
	bool		Frame();
	bool		Render();
	bool		Release();
};
