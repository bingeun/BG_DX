#include "bgCamera.h"

bgCamera::bgCamera()
{
	m_Eye = { 0.0f, 0.0f, -10.0f, 0.0f };
	m_At = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_Up = { 0.0f, 1.0f, 0.0f, 0.0f };
}

bgCamera::~bgCamera()
{
}
