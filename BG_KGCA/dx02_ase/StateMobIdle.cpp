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

	// ���� ó�� - ��� ���� (ó������ ����)
	// ... ������ ó������ �߰� ...

	// �÷��̾ �þ߿� ���� => �߰�
	D3DXVECTOR3 vDis = *pObj->m_pHeroPos - pObj->m_vPos;
	if (fabs(vDis.x) < 6.0f && fabs(vDis.z) < 6.0f)
	{
		pObj->m_iCurrentStateID = pObj->TransState(FSM_INPUT_MOB::InBoundaryPlayer);
		pObj->m_pCurrentState->Init();
	}
	// �������� ��� => �̵�
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

	pObj->m_pDContext->DrawIndexed(8, 8 * 0, 0); // 8 * 0 = ����� ��ü
	pObj->m_pDContext->DrawIndexed(8, 8 * 1, 0); // 8 * 1 = �Ͼ�� �þ�
	//pObj->m_pDContext->DrawIndexed(8, 8 * 2, 0); // 8 * 2 = ������ �߰� ����

	return true;
}

bool StateMobIdle::Release()
{
	return true;
}
