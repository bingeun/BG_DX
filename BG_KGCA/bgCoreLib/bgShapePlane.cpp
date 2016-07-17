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

	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uStride, &uOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_uPrimitiveTopology);

	return true;
}

bool bgShapePlane::Release()
{
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
	return true;
}

HRESULT bgShapePlane::Create()
{
	HRESULT hr = S_OK;



	return hr;
}

HRESULT bgShapePlane::Load()
{
	HRESULT hr = S_OK;



	return hr;
}
