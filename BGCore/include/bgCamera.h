#pragma once
#include "bgStd.h"

class bgCamera
{
public:
	D3DXVECTOR3		m_Eye;
	D3DXVECTOR3		m_At;
	D3DXVECTOR3		m_Up;

public:
	bgCamera();
	virtual ~bgCamera();
};

