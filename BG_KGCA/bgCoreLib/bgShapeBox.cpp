#include "bgShapeBox.h"

bgShapeBox::bgShapeBox()
{
}

bgShapeBox::~bgShapeBox()
{
	Release();
}

bool bgShapeBox::Init()
{
	return true;
}

bool bgShapeBox::Frame()
{
	return true;
}

bool bgShapeBox::Render()
{
	UINT uStride = sizeof(VertexPCTN);
	UINT uOffset = 0;

	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pBufferVertex, &uStride, &uOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pBufferIndex, DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_uPrimitiveTopology);

	return true;
}

bool bgShapeBox::Release()
{
	SAFE_RELEASE(m_pBufferIndex);
	SAFE_RELEASE(m_pBufferVertex);
	return true;
}
