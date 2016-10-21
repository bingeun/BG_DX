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

	bool	Render(ID3D11DeviceContext* m_pDContext);
};
