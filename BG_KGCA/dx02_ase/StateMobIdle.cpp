#include "StateMobIdle.h"

StateMobIdle::StateMobIdle()
{
	m_iStateID = FSM_STATE_MOB::Idle;
}

StateMobIdle::~StateMobIdle()
{
}

bool StateMobIdle::Init()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);
	pObj->m_fStateStartTick = g_fCurrent;
	pObj->m_fStateEndTick = g_fCurrent + (float)(2 + rand() % 4);
	return true;
}

bool StateMobIdle::Frame()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);

	// 오랫동안 대기 => 이동
	if (g_fCurrent >= pObj->m_fStateEndTick)
	{
		pObj->m_iCurrentStateID = pObj->TransState(FSM_INPUT_MOB::IdleLongTime);
		pObj->m_pCurrentState->Init();
	}
	return true;
}

bool StateMobIdle::Render()
{
	return true;
}

bool StateMobIdle::Release()
{
	return true;
}
