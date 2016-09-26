#pragma once
#include "bgStd.h"

class bgArcBall
{
public:
	D3DXMATRIXA16	m_matRotation;
	float			m_fRadius;
	bool			m_bDrag;
	D3DXQUATERNION	m_qDown;
	D3DXQUATERNION	m_qDrag;
	D3DXVECTOR3		m_vDown;
	D3DXVECTOR3		m_vDrag;

public:
	bgArcBall();
	virtual ~bgArcBall();

public:
	void	OnMouseDown(int iX, int iY);
	void	OnMouseMove(int iX, int iY);
	void	OnMouseUp();

	D3DXMATRIX*		GetRotationMatrix() { return D3DXMatrixRotationQuaternion(&m_matRotation, &m_qDrag); }
	D3DXQUATERNION	GetQuaternionNow() { return m_qDrag; }
	D3DXVECTOR3		ScreenToVector(float fScreenX, float fScreenY);
	static D3DXQUATERNION WINAPI QuatFromBallPoints(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo);
};
