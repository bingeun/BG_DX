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
	
	// ����ID���� ����ϴ� ��� ��� ����
	for (iter = m_TransMap.begin(); iter != m_TransMap.end(); ) // ��� ���Ž� ++iter ���� �ʰ� ó��
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
