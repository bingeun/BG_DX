#pragma once
#include "objMob.h"

class StateMobMove : public bgFSMState
{
public:
	StateMobMove();
	virtual ~StateMobMove();

	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();
};
