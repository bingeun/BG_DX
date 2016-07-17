#include "bgShape.h"

bgShape::bgShape()
{
	m_pDevice = NULL;
	m_pDeviceContext = NULL;
	m_uPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_pConstantBuffer = NULL;

	m_iNumVertex = 0;
	m_iNumIndex = 0;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
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

	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uStride, &uOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_uPrimitiveTopology);

	return true;
}

bool bgShape::Release()
{
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
	return true;
}

bool bgShape::Set(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	return true;
}

HRESULT bgShape::CreateCB()
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = sizeof(MatrixBuffer) * 1;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	InitData.pSysMem = &m_ConstantData;

	HR_RETURN(m_pDevice->CreateBuffer(&bd, &InitData, &m_pConstantBuffer));

	return S_OK;
}
