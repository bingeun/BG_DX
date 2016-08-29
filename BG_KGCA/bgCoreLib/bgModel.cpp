#include "bgModel.h"

bgModel::bgModel()
{
}

bgModel::~bgModel()
{
}

bool bgModel::Init()
{
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	return true;
}

bool bgModel::Frame()
{
	return true;
}

bool bgModel::Render()
{
	UINT iStride = sizeof(VertexPCTN);
	UINT iOffset = 0;
	m_pDContext->IASetVertexBuffers(0, 1, &m_pVB, &iStride, &iOffset);
	m_pDContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
	m_pDContext->IASetInputLayout(m_pInputLayout);
	m_pDContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	m_pDContext->VSSetShader(m_pVS, NULL, 0);
	m_pDContext->VSSetConstantBuffers(0, 1, &m_pCB);
	m_pDContext->HSSetShader(NULL, NULL, 0);
	m_pDContext->DSSetShader(NULL, NULL, 0);
	m_pDContext->GSSetShader(NULL, NULL, 0);
	//m_pDContext->GSSetConstantBuffers(0, 1, &m_pCB);
	m_pDContext->RSSetState(m_pRasterizerState);
	m_pDContext->PSSetShader(m_pPS, NULL, 0);
	//m_pDContext->PSSetConstantBuffers(0, 1, &m_pCB);

	//m_pDContext->OMSetRenderTargets(1, &, &);
	//m_pDContext->OMSetBlendState(1, &, &);
	//m_pDContext->OMSetDepthStencilState(&, 0);

	m_pDContext->DrawIndexed(m_iNumIndex, 0, 0);
	return true;
}

bool bgModel::Release()
{
	return true;
}

HRESULT bgModel::CreateBuffer()
{
	HRESULT hr = S_OK;

	float fSize = 1.0f;
	// 버텍스 정보
	VertexPCTN Vertices[] =
	{
		{ { -fSize, +fSize, -fSize },{ 1.0f, 0.0f, 1.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f } }, // 0
		{ { +fSize, +fSize, -fSize },{ 1.0f, 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 0.0f } }, // 1
		{ { +fSize, -fSize, -fSize },{ 1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 0.0f } }, // 2
		{ { -fSize, -fSize, -fSize },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f, 0.0f } }, // 3
		{ { -fSize, +fSize, +fSize },{ 0.0f, 0.0f, 1.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 0.0f } }, // 4
		{ { +fSize, +fSize, +fSize },{ 0.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f } }, // 5
		{ { +fSize, -fSize, +fSize },{ 0.0f, 1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f, 0.0f } }, // 6
		{ { -fSize, -fSize, +fSize },{ 0.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 0.0f } }, // 7
	};
	m_iNumVertex = COUNTOF(Vertices);

	// 인덱스 정보
	UINT iIndices[] =
	{
		0, 1, 2, 0, 2, 3, // 앞면      4 ======= 5
		5, 4, 7, 5, 7, 6, // 뒷면    //|      //||
		4, 0, 3, 4, 3, 7, // 좌측   0 ======= 1 ||
		1, 5, 6, 1, 6, 2, // 우측   || 7 ----||- 6
		4, 5, 1, 4, 1, 0, // 상단   ||/      ||//
		3, 2, 6, 3, 6, 7, // 하단   3 ======= 2
	};
	m_iNumIndex = COUNTOF(iIndices);

	// 버텍스버퍼 생성
	D3D11_BUFFER_DESC VBDesc;
	VBDesc.ByteWidth = sizeof(VertexPCTN) * m_iNumVertex;
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

HRESULT bgModel::LoadShader(CHAR* szVS, CHAR* szPS)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( _DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	// 정점쉐이더 로드
	ID3DBlob* pVSB = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PCTN.hlsl", NULL, NULL, szVS, "vs_5_0", dwShaderFlags, NULL, NULL, &pVSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreateVertexShader((DWORD*)pVSB->GetBufferPointer(), pVSB->GetBufferSize(), NULL, &m_pVS));

	// 픽셀쉐이더 로드
	ID3DBlob* pPSB = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PCTN.hlsl", NULL, NULL, szPS, "ps_5_0", dwShaderFlags, NULL, NULL, &pPSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreatePixelShader((DWORD*)pPSB->GetBufferPointer(), pPSB->GetBufferSize(), NULL, &m_pPS));

	// 레이아웃 생성
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 28,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HR_RETURN(m_pDevice->CreateInputLayout(layout, 4, pVSB->GetBufferPointer(), pVSB->GetBufferSize(), &m_pInputLayout));

	return hr;
}
