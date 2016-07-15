#include "bgShapePlane.h"

bgShapePlane::bgShapePlane()
{
}

bgShapePlane::~bgShapePlane()
{
	Release();
}

bool bgShapePlane::Init()
{
	return true;
}

bool bgShapePlane::Frame()
{
	return true;
}

bool bgShapePlane::Render()
{
	UINT uStride = sizeof(VertexPCT);
	UINT uOffset = 0;

	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pBufferVertex, &uStride, &uOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pBufferIndex, DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_uPrimitiveTopology);

	return true;
}

bool bgShapePlane::Release()
{
	SAFE_RELEASE(m_pBufferIndex);
	SAFE_RELEASE(m_pBufferVertex);
	return true;
}
