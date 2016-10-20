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
	return true;
}

bool StateMobAttack::Frame()
{
	return true;
}

bool StateMobAttack::Render()
{
	return true;
}

bool StateMobAttack::Release()
{
	return true;
}
