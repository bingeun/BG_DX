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

	m_pDContext->IASetInputLayout(m_pInputLayout);
	m_pDContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	m_pDContext->VSSetShader(m_pVS, NULL, 0);
	m_pDContext->VSSetConstantBuffers(0, 1, &m_pCB);
	m_pDContext->HSSetShader(NULL, NULL, 0);
	m_pDContext->DSSetShader(NULL, NULL, 0);
	m_pDContext->GSSetShader(NULL, NULL, 0);
	m_pDContext->PSSetShader(m_pPS, NULL, 0);

	m_pDContext->RSSetState(m_pRasterizerState);
	m_pDContext->VSSetSamplers(0, 1, &g_pDevice->m_pSamplerState);
	m_pDContext->PSSetSamplers(0, 1, &g_pDevice->m_pSamplerState);

	if (m_TexIDList[0].SubIDList.size() == 0)
	{
		m_pDContext->IASetVertexBuffers(0, 1, &m_pVBList[0], &iStride, &iOffset);
		m_pDContext->IASetIndexBuffer(m_pIBList[0], DXGI_FORMAT_R32_UINT, 0);
		I_TextureMgr.GetPtr(m_TexIDList[0].iID)->Apply();
		m_pDContext->DrawIndexed(m_IndexList[0].size(), 0, 0);
	}
	else
	{
		for (int i = 0; i < m_IndexList.size(); i++)
		{
			m_pDContext->IASetVertexBuffers(0, 1, &m_pVBList[i], &iStride, &iOffset);
			m_pDContext->IASetIndexBuffer(m_pIBList[i], DXGI_FORMAT_R32_UINT, 0);
			I_TextureMgr.GetPtr(m_TexIDList[0].SubIDList[i].iID)->Apply(); // PSSetShaderResources(0, 1, &m_pTextureSRV);
			m_pDContext->DrawIndexed(m_IndexList[i].size(), 0, 0);
		}
	}

	return true;
}

bool bgModel::Release()
{
	return true;
}

HRESULT bgModel::CreateBuffer()
{
	HRESULT hr = S_OK;

	// 버텍스버퍼
	D3D11_BUFFER_DESC VBDesc;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA VData;
	VData.SysMemPitch = 0;
	VData.SysMemSlicePitch = 0;

	m_pVBList.resize(m_VertexList.size());
	for (int i = 0; i < m_VertexList.size(); i++)
	{
		VBDesc.ByteWidth = sizeof(VertexPNCT) * m_VertexList[i].size();
		VData.pSysMem = &m_VertexList[i][0];
		HR_RETURN(m_pDevice->CreateBuffer(&VBDesc, &VData, &m_pVBList[i]));
	}

	// 인덱스버퍼
	D3D11_BUFFER_DESC IBDesc;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA IData;
	IData.SysMemPitch = 0;
	IData.SysMemSlicePitch = 0;

	m_pIBList.resize(m_IndexList.size());
	for (int i = 0; i < m_IndexList.size(); i++)
	{
		IBDesc.ByteWidth = sizeof(UINT) * m_IndexList[i].size();
		IData.pSysMem = &m_IndexList[i][0];
		HR_RETURN(m_pDevice->CreateBuffer(&IBDesc, &IData, &m_pIBList[i]));
	}

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

void ObjectNode::Interpolate(float fFrameTick)
{

}
