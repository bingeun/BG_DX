#include "StateMobTrace.h"

StateMobTrace::StateMobTrace()
{
	m_iStateID = FSM_STATE_MOB::Trace;
}

StateMobTrace::~StateMobTrace()
{
}

bool StateMobTrace::Init()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);
	pObj->m_fStateStartTick = g_fCurrent;
	pObj->m_fStateEndTick = g_fCurrent + 999999.0f; // 시간제한없음 (시야에서 사라질때까지)
	return true;
}

bool StateMobTrace::Frame()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);
	
	// 시야에 들어온 플레이어 방향으로 지속적으로 이동
	D3DXVECTOR3 vDir = *pObj->m_pHeroPos - pObj->m_vPos;
	D3DXVec3Normalize(&pObj->m_vDir, &vDir);

	pObj->m_vPos += pObj->m_vDir * g_fSPF * 2.0f;

	return true;
}

bool StateMobTrace::Render()
{
	return true;
}

bool StateMobTrace::Release()
{
	return true;
}
