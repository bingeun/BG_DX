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
	pObj->m_fStateEndTick = g_fCurrent + 9999.0f; // ���ݻ��� ���ӽð� => ������!
	return true;
}

bool StateMobAttack::Frame()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);

	// ���� ó�� - ���� ����
	// ... ���ݻ��� ó������ �߰� ...

	// �÷��̾ �þ߿��� ��� => �߰�
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

	pObj->m_pDContext->DrawIndexed(8, 8 * 0, 0); // 8 * 0 = ����� ��ü
	//pObj->m_pDContext->DrawIndexed(8, 8 * 1, 0); // 8 * 1 = �Ͼ�� �þ�
	//pObj->m_pDContext->DrawIndexed(8, 8 * 2, 0); // 8 * 2 = ������ �߰� ����

	return true;
}

bool StateMobAttack::Release()
{
	return true;
}
