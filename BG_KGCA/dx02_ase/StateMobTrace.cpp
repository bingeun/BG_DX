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
	pObj->m_fStateEndTick = g_fCurrent + 16.0f; // 시간제한 16초
	return true;
}

bool StateMobTrace::Frame()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);
	
	// 상태 처리 - 시야에 들어온 플레이어 방향으로 2배 빠른속도로 추격
	D3DXVECTOR3 vDis = *pObj->m_pHeroPos - pObj->m_vPos;
	D3DXVec3Normalize(&pObj->m_vDir, &vDis);
	pObj->m_vPos += pObj->m_vDir * g_fSPF * 2.0f;

	// 플레이어가 공격범위에 들어옴 => 공격
	if (fabs(vDis.x) < 2.0f && fabs(vDis.z) < 2.0f)
	{
		pObj->m_iCurrentStateID = pObj->TransState(FSM_INPUT_MOB::InRangePlayer);
		pObj->m_pCurrentState->Init();
	}
	// 플레이어가 시야에서 벗어남 => 대기
	else if (fabs(vDis.x) > 10.0f || fabs(vDis.z) > 10.0f)
	{
		pObj->m_iCurrentStateID = pObj->TransState(FSM_INPUT_MOB::OutBoundaryPlayer);
		pObj->m_pCurrentState->Init();
	}

	return true;
}

bool StateMobTrace::Render()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);

	pObj->m_pDContext->DrawIndexed(8, 8 * 0, 0); // 8 * 0 = 노란색 몸체
	//pObj->m_pDContext->DrawIndexed(8, 8 * 1, 0); // 8 * 1 = 하얀색 시야
	pObj->m_pDContext->DrawIndexed(8, 8 * 2, 0); // 8 * 2 = 빨간색 추격 영역

	return true;
}

bool StateMobTrace::Release()
{
	return true;
}
