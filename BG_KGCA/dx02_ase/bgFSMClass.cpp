#include "bgFSMClass.h"

bgFSMClass::bgFSMClass()
{
	m_StateMap.clear();
	m_pCurrentState = NULL;
	m_iCurrentStateID = 0;
}

bgFSMClass::~bgFSMClass()
{
	m_pCurrentState = NULL;
	m_iCurrentStateID = 0;

	map<int, bgFSMState*>::iterator iter;

	for (iter = m_StateMap.begin(); iter != m_StateMap.end(); ) // 요소 제거시 ++iter 하지 않게 처리
	{
		SAFE_DEL(iter->second);
		m_StateMap.erase(iter++);
	}
}

bgFSMState* bgFSMClass::GetState(int iStateID)
{
	map<int, bgFSMState*>::iterator iter;
	
	iter = m_StateMap.find(iStateID);
	if (iter != m_StateMap.end())
		return iter->second;
	
	return NULL;
}

void bgFSMClass::AddState(bgFSMState* pState)
{
	m_StateMap.insert(pair<int, bgFSMState*>(pState->GetStateID(), pState));
}

void bgFSMClass::DelState(int iStateID)
{
	map<int, bgFSMState*>::iterator iter;

	iter = m_StateMap.find(iStateID);
	if (iter != m_StateMap.end())
	{
		delete iter->second;
		m_StateMap.erase(iter);
	}
}

int bgFSMClass::TransState(int iInputEvent)
{
	map<int, int>::iterator iter = m_pCurrentState->m_TransMap.begin();

	iter = m_pCurrentState->m_TransMap.find(iInputEvent);
	if (iter != m_pCurrentState->m_TransMap.end())
	{
		m_iCurrentStateID = iter->second;
		m_pCurrentState = m_StateMap[m_iCurrentStateID];
	}

	return m_iCurrentStateID;
}
