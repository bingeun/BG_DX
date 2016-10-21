#pragma once
#include "objMob.h"

class StateMobTrace : public bgFSMState
{
public:
	StateMobTrace();
	virtual ~StateMobTrace();

	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	bool	Render(ID3D11DeviceContext* m_pDContext);
};
