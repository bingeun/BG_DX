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

	// 상태 처리 - 대기 상태 (처리내용 없음)
	// ... 대기상태 처리내용 추가 ...

	// 플레이어가 시야에 들어옴 => 추격
	D3DXVECTOR3 vDis = *pObj->m_pHeroPos - pObj->m_vPos;
	if (fabs(vDis.x) < 6.0f && fabs(vDis.z) < 6.0f)
	{
		pObj->m_iCurrentStateID = pObj->TransState(FSM_INPUT_MOB::InBoundaryPlayer);
		pObj->m_pCurrentState->Init();
	}
	// 오랫동안 대기 => 이동
	else if (g_fCurrent >= pObj->m_fStateEndTick)
	{
		pObj->m_iCurrentStateID = pObj->TransState(FSM_INPUT_MOB::IdleLongTime);
		pObj->m_pCurrentState->Init();
	}
	return true;
}

bool StateMobIdle::Render()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);

	pObj->m_pDContext->DrawIndexed(8, 8 * 0, 0); // 8 * 0 = 노란색 몸체
	pObj->m_pDContext->DrawIndexed(8, 8 * 1, 0); // 8 * 1 = 하얀색 시야
	//pObj->m_pDContext->DrawIndexed(8, 8 * 2, 0); // 8 * 2 = 빨간색 추격 영역

	return true;
}

bool StateMobIdle::Release()
{
	return true;
}
