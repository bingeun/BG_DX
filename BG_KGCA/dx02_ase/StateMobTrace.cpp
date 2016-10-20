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
	pObj->m_fStateEndTick = g_fCurrent + 999999.0f; // �ð����Ѿ��� (�þ߿��� �����������)
	return true;
}

bool StateMobTrace::Frame()
{
	objMob* pObj = static_cast<objMob*>(m_pObject);
	
	// �þ߿� ���� �÷��̾� �������� ���������� �̵�
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
