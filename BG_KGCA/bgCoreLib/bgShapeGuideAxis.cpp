#include "bgShapeGuideAxis.h"

bgShapeGuideAxis::bgShapeGuideAxis()
{
}

bgShapeGuideAxis::~bgShapeGuideAxis()
{
}

bool bgShapeGuideAxis::Init()
{
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	return true;
}

bool bgShapeGuideAxis::Frame()
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pDContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	MATRIX_BUFFER* pCBData = (MATRIX_BUFFER*)MappedResource.pData;
	pCBData->matWorld = g_MatrixBuffer.matWorld;
	pCBData->matView = g_MatrixBuffer.matView;
	pCBData->matProj = g_MatrixBuffer.matProj;
	m_pDContext->Unmap(m_pCB, 0);
	return true;
}

bool bgShapeGuideAxis::Render()
{
	m_pDContext->VSSetConstantBuffers(0, 1, &m_pCB);
	//m_pDContext->UpdateSubresource(m_pCB, 0, NULL, &g_MatrixBuffer, 0, 0);
	m_pDContext->VSSetShader(m_pVS, NULL, 0);
	m_pDContext->HSSetShader(NULL, NULL, 0);
	m_pDContext->DSSetShader(NULL, NULL, 0);
	m_pDContext->GSSetShader(NULL, NULL, 0);
	m_pDContext->PSSetShader(m_pPS, NULL, 0);
	m_pDContext->IASetInputLayout(m_pInputLayout);

	UINT iStride = sizeof(VertexPC);
	UINT iOffset = 0;
	m_pDContext->IASetVertexBuffers(0, 1, &m_pVB, &iStride, &iOffset);
	m_pDContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R16_UINT, 0);
	m_pDContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
	m_pDContext->RSSetState(m_pRasterizerState);
	m_pDContext->DrawIndexed(m_iNumIndex, 0, 0);
	return true;
}

bool bgShapeGuideAxis::Release()
{
	return true;
}

HRESULT bgShapeGuideAxis::CreateBuffer()
{
	HRESULT hr = S_OK;

	// 버텍스 정보
	VertexPC Vertices[] =
	{
		{ { 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } }, // 원점 빨강
		{ { 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } }, // x축 빨강
		{ { 0.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 원점 초록
		{ { 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // y축 초록
		{ { 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }, // 원점 파랑
		{ { 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }, // z축 파랑
	};
	m_iNumVertex = COUNTOF(Vertices);

	// 인덱스 정보
	UINT iIndices[] =
	{
		0, 1, // x축 빨강
		2, 3, // y축 초록
		4, 5, // z축 파랑
	};
	m_iNumIndex = COUNTOF(iIndices);

	// 버텍스버퍼 생성
	D3D11_BUFFER_DESC VBDesc;
	VBDesc.ByteWidth = sizeof(VertexPC) * m_iNumVertex;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA VData;
	VData.pSysMem = Vertices;
	VData.SysMemPitch = 0;
	VData.SysMemSlicePitch = 0;

	HR_RETURN(m_pDevice->CreateBuffer(&VBDesc, &VData, &m_pVB));

	// 인덱스버퍼 생성
	D3D11_BUFFER_DESC IBDesc;
	IBDesc.ByteWidth = sizeof(UINT) * m_iNumIndex;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA IData;
	IData.pSysMem = iIndices;
	IData.SysMemPitch = 0;
	IData.SysMemSlicePitch = 0;

	HR_RETURN(m_pDevice->CreateBuffer(&IBDesc, &IData, &m_pIB));

	return hr;
}

HRESULT bgShapeGuideAxis::LoadShader()
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( _DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	// 정점쉐이더 로드
	ID3DBlob* pVSB = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PC.hlsl", NULL, NULL, "VS", "vs_5_0", dwShaderFlags, NULL, NULL, &pVSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreateVertexShader((DWORD*)pVSB->GetBufferPointer(), pVSB->GetBufferSize(), NULL, &m_pVS));

	// 픽셀쉐이더 로드
	ID3DBlob* pPSB = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PC.hlsl", NULL, NULL, "PS", "ps_5_0", dwShaderFlags, NULL, NULL, &pPSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreatePixelShader((DWORD*)pPSB->GetBufferPointer(), pPSB->GetBufferSize(), NULL, &m_pPS));

	// 레이아웃 생성
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HR_RETURN(m_pDevice->CreateInputLayout(layout, 2, pVSB->GetBufferPointer(), pVSB->GetBufferSize(), &m_pInputLayout));

	return hr;
}
