#include "bgArcBall.h"
#include "bgSys.h"

bgArcBall::bgArcBall()
{
}

bgArcBall::~bgArcBall()
{
}

void bgArcBall::OnMouseDown(int iX, int iY)
{
	if ((iX >= 0 && iX < g_pWindow->m_iClientW) && (iY >= 0 && iY < g_pWindow->m_iClientH))
	{
		m_bDrag = true;
		m_qDown = m_qDrag;
		m_vDown = ScreenToVector((float)iX, (float)iY);
	}
}

void bgArcBall::OnMouseMove(int iX, int iY)
{
	if (m_bDrag)
	{
		m_vDrag = ScreenToVector((float)iX, (float)iY);
		m_qDrag = m_qDown*QuatFromBallPoints(m_vDown, m_vDrag);
	}
}

void bgArcBall::OnMouseUp()
{
	m_bDrag = false;
}

D3DXVECTOR3 bgArcBall::ScreenToVector(float fScreenX, float fScreenY)
{
	FLOAT x = ((fScreenX / g_pWindow->m_iClientW) * 2.0f - 1.0f) * -1.0f;
	FLOAT y = (fScreenY / g_pWindow->m_iClientH) * 2.0f - 1.0f;
	FLOAT z = 0.0f;

	FLOAT mag = x*x + y*y;
	if (mag > 1.0f)
	{
		FLOAT scale = 1.0f / sqrtf(mag);
		x *= scale;
		y *= scale;
	}
	else
	{
		z = sqrtf(1.0f - mag);
	}

	return D3DXVECTOR3(x, y, z);
}

D3DXQUATERNION bgArcBall::QuatFromBallPoints(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo)
{
	D3DXVECTOR3 vPart;
	float fDot = D3DXVec3Dot(&vFrom, &vTo);
	D3DXVec3Cross(&vPart, &vFrom, &vTo);

	return D3DXQUATERNION(vPart.x, vPart.y, vPart.z, fDot);
}
