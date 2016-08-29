#include "bgModel.h"

bgModel::bgModel()
{
	Init();
}

bgModel::~bgModel()
{
	Release();
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
	UINT iStride = sizeof(VertexPNCT);
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

	I_TextureMgr.GetPtr(m_TextureIDList[0])->Apply();
	m_pDContext->VSSetSamplers(0, 1, &g_pDevice->m_pSamplerState);
	m_pDContext->PSSetSamplers(0, 1, &g_pDevice->m_pSamplerState);
	//m_pDContext->OMSetBlendState(m_pAlphaBlend, 0, -1);

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

	m_iNumVertex = m_VertexList.size();
	m_iNumIndex = m_IndexList.size();

	// 버텍스버퍼 생성
	D3D11_BUFFER_DESC VBDesc;
	VBDesc.ByteWidth = sizeof(VertexPNCT) * m_iNumVertex;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA VData;
	VData.pSysMem = &m_VertexList.at(0);
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
	IData.pSysMem = &m_IndexList.at(0);
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
	HR_RETURN(D3DX11CompileFromFile(L"PNCT.hlsl", NULL, NULL, szVS, "vs_5_0", dwShaderFlags, NULL, NULL, &pVSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreateVertexShader((DWORD*)pVSB->GetBufferPointer(), pVSB->GetBufferSize(), NULL, &m_pVS));

	// 픽셀쉐이더 로드
	ID3DBlob* pPSB = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PNCT.hlsl", NULL, NULL, szPS, "ps_5_0", dwShaderFlags, NULL, NULL, &pPSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreatePixelShader((DWORD*)pPSB->GetBufferPointer(), pPSB->GetBufferSize(), NULL, &m_pPS));

	// 레이아웃 생성
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 40,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HR_RETURN(m_pDevice->CreateInputLayout(layout, 4, pVSB->GetBufferPointer(), pVSB->GetBufferSize(), &m_pInputLayout));

	return hr;
}
