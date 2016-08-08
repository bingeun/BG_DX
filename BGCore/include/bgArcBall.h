#pragma once
#include "bgStd.h"

class bgArcBall
{
public:
	D3DXMATRIXA16	m_matRotation;
	float			m_fRadius;
	D3DXQUATERNION	m_quatDown;
	D3DXQUATERNION	m_quatDrag;

public:
	bgArcBall();
	virtual ~bgArcBall();
};
