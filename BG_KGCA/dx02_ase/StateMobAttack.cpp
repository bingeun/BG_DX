#include "StateMobAttack.h"

StateMobAttack::StateMobAttack()
{
	m_iStateID = FSM_STATE_MOB::Attack;
}

StateMobAttack::~StateMobAttack()
{
}

bool StateMobAttack::Init()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);
	pObj->m_fStateStartTick = g_fCurrent;
	pObj->m_fStateEndTick = g_fCurrent + 9999.0f; // 공격상태 지속시간 => 무제한!
	return true;
}

bool StateMobAttack::Frame()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);

	// 상태 처리 - 공격 상태
	// ... 공격상태 처리내용 추가 ...

	// 플레이어가 시야에서 벗어남 => 추격
	D3DXVECTOR3 vDis = *pObj->m_pHeroPos - pObj->m_vPos;
	if (fabs(vDis.x) > 2.0f || fabs(vDis.z) > 2.0f)
	{
		pObj->m_iCurrentStateID = pObj->TransState(FSM_INPUT_MOB::OutRangePlayer);
		pObj->m_pCurrentState->Init();
	}
	return true;
}

bool StateMobAttack::Render()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);

	pObj->m_pDContext->DrawIndexed(8, 8 * 0, 0); // 8 * 0 = 노란색 몸체
	//pObj->m_pDContext->DrawIndexed(8, 8 * 1, 0); // 8 * 1 = 하얀색 시야
	//pObj->m_pDContext->DrawIndexed(8, 8 * 2, 0); // 8 * 2 = 빨간색 추격 영역

	return true;
}

bool StateMobAttack::Release()
{
	return true;
}
