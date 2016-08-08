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

public:
	void	MoveForward(float fValue);
	void	MoveBackward(float fValue);
	void	MoveLeft(float fValue);
	void	MoveRight(float fValue);
	void	MoveUp(float fValue);
	void	MoveDown(float fValue);

	void	RotateLeft(float fValue);
	void	RotateRight(float fValue);
	void	RotateUp(float fValue);
	void	RotateDown(float fValue);
};
