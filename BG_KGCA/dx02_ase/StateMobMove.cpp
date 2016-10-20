#include "StateMobMove.h"

StateMobMove::StateMobMove()
{
	m_iStateID = FSM_STATE_MOB::Move;
}

StateMobMove::~StateMobMove()
{
}

bool StateMobMove::Init()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);
	pObj->m_fStateStartTick = g_fCurrent;
	pObj->m_fStateEndTick = g_fCurrent + (float)(3 + rand() % 5);
	pObj->m_vDir = D3DXVECTOR3((float)((rand() % 10 - 5) / 5.0f), 0.0f, (float)((rand() % 10 - 5) / 5.0f));
	return true;
}

bool StateMobMove::Frame()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);

	// 기본 이동 동작
//	pObj->m_vPos += pObj->m_vDir * g_fSPF;
	D3DXVECTOR3 vDir = *pObj->m_pHeroPos - pObj->m_vPos;
	D3DXVec3Normalize(&pObj->m_vDir, &vDir);

	pObj->m_vPos += pObj->m_vDir * g_fSPF * 2.0f;

	// 이동 종료 => 대기
	if (g_fCurrent >= pObj->m_fStateEndTick)
	{
		pObj->m_iCurrentStateID = pObj->TransState(FSM_INPUT_MOB::EndMove);
		pObj->m_pCurrentState->Init();
	}
	return true;
}

bool StateMobMove::Render()
{
	return true;
}

bool StateMobMove::Release()
{
	return true;
}
