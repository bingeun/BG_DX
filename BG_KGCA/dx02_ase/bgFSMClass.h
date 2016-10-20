#pragma once
#include "bgFSMState.h"

class bgFSMClass
{
public:
	map<int, bgFSMState*>	m_StateMap;
	bgFSMState*				m_pCurrentState;
	int						m_iCurrentStateID;

public:
	bgFSMClass();
	virtual ~bgFSMClass();

	int			GetCurrentStateID() { return m_iCurrentStateID; }
	void		SetCurrentStateID(int iStateID) { m_iCurrentStateID = iStateID; }
	
	bgFSMState*	GetState(int iStateID);
	void		AddState(bgFSMState* pState);
	void		DelState(int iStateID);
	int			TransState(int iInputEvent);
};
