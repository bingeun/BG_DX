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

	// 상태 처리 - 랜덤하게 지정된 방향으로 이동
	pObj->m_vPos += pObj->m_vDir * g_fSPF;

	// 플레이어가 시야에 들어옴 => 추격
	D3DXVECTOR3 vDis = *pObj->m_pHeroPos - pObj->m_vPos;
	if (fabs(vDis.x) < 6.0f && fabs(vDis.z) < 6.0f)
	{
		pObj->m_iCurrentStateID = pObj->TransState(FSM_INPUT_MOB::InBoundaryPlayer);
		pObj->m_pCurrentState->Init();
	}
	// 이동 종료 => 대기
	else if (g_fCurrent >= pObj->m_fStateEndTick)
	{
		pObj->m_iCurrentStateID = pObj->TransState(FSM_INPUT_MOB::EndMove);
		pObj->m_pCurrentState->Init();
	}
	return true;
}

bool StateMobMove::Render()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);

	pObj->m_pDContext->DrawIndexed(8, 8 * 0, 0); // 8 * 0 = 노란색 몸체
	pObj->m_pDContext->DrawIndexed(8, 8 * 1, 0); // 8 * 1 = 하얀색 시야
	//pObj->m_pDContext->DrawIndexed(8, 8 * 2, 0); // 8 * 2 = 빨간색 추격 영역

	return true;
}

bool StateMobMove::Release()
{
	return true;
}
