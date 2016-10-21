#pragma once
#include "objMob.h"

class StateMobIdle : public bgFSMState
{
public:
	StateMobIdle();
	virtual ~StateMobIdle();

	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	bool	Render(ID3D11DeviceContext* m_pDContext);
};
