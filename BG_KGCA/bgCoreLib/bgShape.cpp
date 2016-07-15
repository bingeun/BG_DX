#include "bgShape.h"

bgShape::bgShape()
{
	m_pDevice = NULL;
	m_pDeviceContext = NULL;
	m_uPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

	m_pBufferVertex = NULL;
	m_pBufferIndex = NULL;

	m_iNumVertex = 0;
	m_iNumIndex = 0;
}

bgShape::~bgShape()
{
}

bool bgShape::Init()
{
	return true;
}

bool bgShape::Frame()
{
	return true;
}

bool bgShape::Render()
{
	UINT uStride = sizeof(VertexPC);
	UINT uOffset = 0;

	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pBufferVertex, &uStride, &uOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pBufferIndex, DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_uPrimitiveTopology);

	return true;
}

bool bgShape::Release()
{
	SAFE_RELEASE(m_pBufferIndex);
	SAFE_RELEASE(m_pBufferVertex);
	return true;
}

bool bgShape::Set(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	return true;
}
