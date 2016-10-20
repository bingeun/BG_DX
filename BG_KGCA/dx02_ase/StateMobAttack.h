#pragma once
#include "objMob.h"

class StateMobAttack : public bgFSMState
{
public:
	StateMobAttack();
	virtual ~StateMobAttack();

	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();
};
