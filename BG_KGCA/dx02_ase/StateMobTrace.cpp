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
	pObj->m_fStateEndTick = g_fCurrent + 16.0f; // �ð����� 16��
	return true;
}

bool StateMobTrace::Frame()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);
	
	// ���� ó�� - �þ߿� ���� �÷��̾� �������� 2�� �����ӵ��� �߰�
	D3DXVECTOR3 vDis = *pObj->m_pHeroPos - pObj->m_vPos;
	D3DXVec3Normalize(&pObj->m_vDir, &vDis);
	pObj->m_vPos += pObj->m_vDir * g_fSPF * 2.0f;

	// �÷��̾ ���ݹ����� ���� => ����
	if (fabs(vDis.x) < 2.0f && fabs(vDis.z) < 2.0f)
	{
		pObj->m_iCurrentStateID = pObj->TransState(FSM_INPUT_MOB::InRangePlayer);
		pObj->m_pCurrentState->Init();
	}
	// �÷��̾ �þ߿��� ��� => ���
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

	pObj->m_pDContext->DrawIndexed(8, 8 * 0, 0); // 8 * 0 = ����� ��ü
	//pObj->m_pDContext->DrawIndexed(8, 8 * 1, 0); // 8 * 1 = �Ͼ�� �þ�
	pObj->m_pDContext->DrawIndexed(8, 8 * 2, 0); // 8 * 2 = ������ �߰� ����

	return true;
}

bool StateMobTrace::Release()
{
	return true;
}
