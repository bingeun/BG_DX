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
	m_uPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; //D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	return true;
}

bool bgShapeBox::Frame()
{
	return true;
}

bool bgShapeBox::Render()
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXVECTOR3 vEye(10.0f, 10.0f, -20.0f);
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&m_matView, &vEye, &vAt, &vUp);
	D3DXMatrixPerspectiveFovLH(&m_matProj, (FLOAT)D3DX_PI * 0.25f, g_pWindow->m_iClientW / (FLOAT)g_pWindow->m_iClientH, 0.1f, 1000.0f);

	D3DXMatrixTranspose(&m_ConstantData.matWorld, &m_matWorld);
	D3DXMatrixTranspose(&m_ConstantData.matView, &m_matView);
	D3DXMatrixTranspose(&m_ConstantData.matProj, &m_matProj);

	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, NULL, &m_ConstantData, 0, 0);


	m_pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);
	m_pDeviceContext->GSSetShader(NULL, NULL, 0);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	UINT uStride = sizeof(VertexPCTN);
	UINT uOffset = 0;

	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uStride, &uOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_uPrimitiveTopology);
	m_pDeviceContext->DrawIndexed(36, 0, 0);

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

	VertexPCTN vVertices[] =
	{
		//{ { -1.0f, +1.0f, -1.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, // 0
		//{ { +1.0f, +1.0f, -1.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, // 1
		//{ { +1.0f, -1.0f, -1.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, // 2
		//{ { -1.0f, -1.0f, -1.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, // 3
		//{ { -1.0f, +1.0f, +1.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, // 4
		//{ { +1.0f, +1.0f, +1.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, // 5
		//{ { +1.0f, -1.0f, +1.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, // 6
		//{ { -1.0f, -1.0f, +1.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, // 7
		{ { -1.0f, +1.0f, +1.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, // 0
		{ { +1.0f, +1.0f, +1.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, // 1
		{ { +1.0f, -1.0f, +1.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }, // 2
		{ { -1.0f, -1.0f, +1.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }, // 3
		{ { -1.0f, +1.0f, +3.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, // 4
		{ { +1.0f, +1.0f, +3.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, // 5
		{ { +1.0f, -1.0f, +3.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }, // 6
		{ { -1.0f, -1.0f, +3.0f }, { +0.0f, +0.0f, +0.0f, +1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }, // 7
	};
	UINT uIndices[] =
	{
		0, 1, 2, 0, 2, 3, // 앞면       4========5
		5, 4, 7, 5, 7, 6, // 뒷면     //|     // ||
		4, 0, 3, 4, 3, 7, // 좌측   0========1   ||
		1, 5, 6, 1, 6, 2, // 우측  ||   7---||---6
		4, 5, 1, 4, 1, 0, // 상단  || /     || //
		3, 2, 6, 3, 6, 7, // 하단   3========2
	};

	m_iNumVertex = COUNTOF(vVertices);
	m_iNumIndex = COUNTOF(uIndices);

	D3D11_BUFFER_DESC BufferDescVertex;
	BufferDescVertex.ByteWidth = sizeof(VertexPCTN) * m_iNumVertex;
	BufferDescVertex.Usage = D3D11_USAGE_DEFAULT;
	BufferDescVertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDescVertex.CPUAccessFlags = 0;
	BufferDescVertex.MiscFlags = 0;
	BufferDescVertex.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA DataVertex;
	DataVertex.pSysMem = vVertices;
	DataVertex.SysMemPitch = 0;
	DataVertex.SysMemSlicePitch = 0;
	HR_RETURN(m_pDevice->CreateBuffer(&BufferDescVertex, &DataVertex, &m_pVertexBuffer));

	D3D11_BUFFER_DESC BufferDescIndex;
	BufferDescIndex.ByteWidth = sizeof(UINT) * m_iNumIndex;
	BufferDescIndex.Usage = D3D11_USAGE_DEFAULT;
	BufferDescIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferDescIndex.CPUAccessFlags = 0;
	BufferDescIndex.MiscFlags = 0;
	BufferDescIndex.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA DataIndex;
	DataIndex.pSysMem = uIndices;
	DataIndex.SysMemPitch = 0;
	DataIndex.SysMemSlicePitch = 0;
	HR_RETURN(m_pDevice->CreateBuffer(&BufferDescIndex, &DataIndex, &m_pIndexBuffer));

	hr = CreateCB();

	return hr;
}

HRESULT bgShapeBox::Load()
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( _DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pVSBuf = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PCTN_VS.hlsl", NULL, NULL, "VS", "vs_5_0", dwShaderFlags, NULL, NULL, &pVSBuf, NULL, NULL));
	HR_RETURN(m_pDevice->CreateVertexShader((DWORD*)pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), NULL, &m_pVertexShader));

	ID3DBlob* pPSBuf = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PCTN_PS.hlsl", NULL, NULL, "PS", "ps_5_0", dwShaderFlags, NULL, NULL, &pPSBuf, NULL, NULL));
	HR_RETURN(m_pDevice->CreatePixelShader((DWORD*)pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPixelShader));
	/*
	ID3DBlob* pGSBuf = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PCTN_GS.hlsl", NULL, NULL, "GS", "gs_5_0", dwShaderFlags, NULL, NULL, &pGSBuf, NULL, NULL));
	HR_RETURN(m_pDevice->CreateGeometryShader((DWORD*)pGSBuf->GetBufferPointer(), pGSBuf->GetBufferSize(), NULL, &m_pGeometryShader));
	*/
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 28,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HR_RETURN(m_pDevice->CreateInputLayout(layout, 4, pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), &m_pInputLayout));

	SAFE_RELEASE(pVSBuf);
	SAFE_RELEASE(pPSBuf);
	//SAFE_RELEASE(pGSBuf);
	SAFE_RELEASE(m_pRasterizerState);

	D3D11_RASTERIZER_DESC RSDesc;
	memset(&RSDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
	RSDesc.FillMode = D3D11_FILL_SOLID;
	RSDesc.CullMode = D3D11_CULL_NONE;
	RSDesc.DepthClipEnable = TRUE;

	HR_RETURN(m_pDevice->CreateRasterizerState(&RSDesc, &m_pRasterizerState));

	return hr;
}
