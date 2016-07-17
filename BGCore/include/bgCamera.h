#pragma once
#include "bgStd.h"

class bgCamera
{
public:
	D3DXVECTOR4		m_Eye;
	D3DXVECTOR4		m_At;
	D3DXVECTOR4		m_Up;

public:
	bgCamera();
	virtual ~bgCamera();
};

