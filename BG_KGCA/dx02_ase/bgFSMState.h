#pragma once
#include "bgStd.h"

class bgFSMState
{
public:
	void*			m_pObject;

public:
	map<int, int>	m_TransMap;
	int				m_iStateID;

public:
	bgFSMState();
	virtual ~bgFSMState();

	virtual bool	Init() = 0;
	virtual bool	Frame() = 0;
	virtual bool	Render() = 0;
	virtual bool	Release() = 0;

	int		GetStateID() { return m_iStateID; }
	void	AddTrans(int iInputEvent, int iOutputID);
	void	DelTrans(int iOutputID);
	int		TransEvent(int iInputEvent);
};
