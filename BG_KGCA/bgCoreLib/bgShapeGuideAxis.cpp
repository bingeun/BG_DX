#include "bgShapeGuideAxis.h"

bgShapeGuideAxis::bgShapeGuideAxis()
{
}

bgShapeGuideAxis::~bgShapeGuideAxis()
{
	Release();
}

bool bgShapeGuideAxis::Init()
{
	return true;
}

bool bgShapeGuideAxis::Frame()
{
	return true;
}

bool bgShapeGuideAxis::Render()
{
	UINT uStride = sizeof(VertexPC);
	UINT uOffset = 0;

	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pBufferVertex, &uStride, &uOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pBufferIndex, DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_uPrimitiveTopology);

	return true;
}

bool bgShapeGuideAxis::Release()
{
	SAFE_RELEASE(m_pBufferIndex);
	SAFE_RELEASE(m_pBufferVertex);
	return true;
}

HRESULT bgShapeGuideAxis::Create()
{
	HRESULT hr = S_OK;

	D3DXVECTOR3 vVertices[] =
	{
		{ 0.0f, 0.0f, 0.0f }, // 원점
		{ 1.0f, 0.0f, 0.0f }, // x축
		{ 0.0f, 1.0f, 0.0f }, // y축
		{ 0.0f, 0.0f, 1.0f }, // z축
	};
	UINT uIndices[] =
	{
		0, 1, // x축
		0, 2, // y축
		0, 3, // z축
	};

	m_iNumVertex = COUNTOF(vVertices);
	m_iNumIndex = COUNTOF(uIndices);

	D3D11_BUFFER_DESC BufferDescVertex;
	BufferDescVertex.ByteWidth = sizeof(VertexPC) * m_iNumVertex;
	BufferDescVertex.Usage = D3D11_USAGE_DEFAULT;
	BufferDescVertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDescVertex.CPUAccessFlags = 0;
	BufferDescVertex.MiscFlags = 0;
	BufferDescVertex.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA DataVertex;
	DataVertex.pSysMem = vVertices;
	DataVertex.SysMemPitch = 0;
	DataVertex.SysMemSlicePitch = 0;
	HR_RETURN(m_pDevice->CreateBuffer(&BufferDescVertex, &DataVertex, &m_pBufferVertex));

	D3D11_BUFFER_DESC BufferDescIndex;
	BufferDescIndex.ByteWidth = sizeof(ULONG) * m_iNumIndex;
	BufferDescIndex.Usage = D3D11_USAGE_DEFAULT;
	BufferDescIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferDescIndex.CPUAccessFlags = 0;
	BufferDescIndex.MiscFlags = 0;
	BufferDescIndex.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA DataIndex;
	DataIndex.pSysMem = uIndices;
	DataIndex.SysMemPitch = 0;
	DataIndex.SysMemSlicePitch = 0;
	HR_RETURN(m_pDevice->CreateBuffer(&BufferDescIndex, &DataIndex, &m_pBufferIndex));

	return hr;
}
