#include "bgFSMState.h"

bgFSMState::bgFSMState()
{
}

bgFSMState::~bgFSMState()
{
}

void bgFSMState::AddTrans(int iInputEvent, int iOutputID)
{
	m_TransMap.insert(pair<int, int>(iInputEvent, iOutputID));
}

void bgFSMState::DelTrans(int iOutputID)
{
	map<int, int>::iterator iter;
	
	// 인자ID값을 출력하는 모든 요소 제거
	for (iter = m_TransMap.begin(); iter != m_TransMap.end(); ) // 요소 제거시 ++iter 하지 않게 처리
	{
		if (iter->second == iOutputID)
			m_TransMap.erase(iter++);
		else
			++iter;
	}
}

int bgFSMState::TransEvent(int iInputEvent)
{
	map<int, int>::iterator iter;

	iter = m_TransMap.find(iInputEvent);
	if (iter == m_TransMap.end())
		return m_iStateID;

	m_iStateID = iter->second;

	return iter->second;
}
