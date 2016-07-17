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

	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uStride, &uOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_uPrimitiveTopology);

	return true;
}

bool bgShapeBox::Release()
{
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
	return true;
}

HRESULT bgShapeBox::Create()
{
	HRESULT hr = S_OK;



	return hr;
}

HRESULT bgShapeBox::Load()
{
	HRESULT hr = S_OK;



	return hr;
}
