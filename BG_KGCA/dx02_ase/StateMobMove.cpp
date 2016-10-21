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

	// ���� ó�� - �����ϰ� ������ �������� �̵�
	pObj->m_vPos += pObj->m_vDir * g_fSPF;

	// �÷��̾ �þ߿� ���� => �߰�
	D3DXVECTOR3 vDis = *pObj->m_pHeroPos - pObj->m_vPos;
	if (fabs(vDis.x) < 6.0f && fabs(vDis.z) < 6.0f)
	{
		pObj->m_iCurrentStateID = pObj->TransState(FSM_INPUT_MOB::InBoundaryPlayer);
		pObj->m_pCurrentState->Init();
	}
	// �̵� ���� => ���
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

	pObj->m_pDContext->DrawIndexed(8, 8 * 0, 0); // 8 * 0 = ����� ��ü
	pObj->m_pDContext->DrawIndexed(8, 8 * 1, 0); // 8 * 1 = �Ͼ�� �þ�
	//pObj->m_pDContext->DrawIndexed(8, 8 * 2, 0); // 8 * 2 = ������ �߰� ����

	return true;
}

bool StateMobMove::Release()
{
	return true;
}
