#include "objBoard.h"

objBoard::objBoard()
{
}

objBoard::~objBoard()
{
}

bool objBoard::Init()
{
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	return true;
}

bool objBoard::Frame()
{
	return true;
}

bool objBoard::Render()
{
	UINT iStride = sizeof(VertexPCT);
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

bool objBoard::Release()
{
	return true;
}

HRESULT objBoard::CreateBuffer()
{
	HRESULT hr = S_OK;

	// 버텍스 정보
	m_iNumVertex = COUNTOF(m_CandyVertex);

	// 인덱스 정보
	UINT iIndices[BOARD_H * BOARD_W * 6] = { 0, };
	m_iNumIndex = COUNTOF(iIndices);

	// 버텍스데이터는 업데이트, 인덱스데이터는 고정
	for (int i = 0; i < BOARD_W*BOARD_H; i++)
	{
		for (int j = 0; j < 6; j++)
			iIndices[i * 6 + j] = i * 6 + j;

		m_CandyVertex[i * 6 + 0].pos = D3DXVECTOR3(GET_PIXEL_X(BOARD_X + TILE_W*(i % BOARD_W)), GET_PIXEL_Y(BOARD_Y + TILE_H*(i / BOARD_H)), 0.5f);
		m_CandyVertex[i * 6 + 0].col = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		m_CandyVertex[i * 6 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);

		m_CandyVertex[i * 6 + 1].pos = D3DXVECTOR3(GET_PIXEL_X(BOARD_X + TILE_W*((i % BOARD_W) + 1)), GET_PIXEL_Y(BOARD_Y + TILE_H*(i / BOARD_H)), 0.5f);
		m_CandyVertex[i * 6 + 1].col = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		m_CandyVertex[i * 6 + 1].tex = D3DXVECTOR2(0.0f, 0.0f);

		m_CandyVertex[i * 6 + 2].pos = D3DXVECTOR3(GET_PIXEL_X(BOARD_X + TILE_W*((i % BOARD_W) + 1)), GET_PIXEL_Y(BOARD_Y + TILE_H*((i / BOARD_H) + 1)), 0.5f);
		m_CandyVertex[i * 6 + 2].col = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		m_CandyVertex[i * 6 + 2].tex = D3DXVECTOR2(0.0f, 0.0f);

		m_CandyVertex[i * 6 + 3].pos = m_CandyVertex[i * 6 + 0].pos;
		m_CandyVertex[i * 6 + 3].col = m_CandyVertex[i * 6 + 0].col;
		m_CandyVertex[i * 6 + 3].tex = m_CandyVertex[i * 6 + 0].tex;

		m_CandyVertex[i * 6 + 4].pos = m_CandyVertex[i * 6 + 2].pos;
		m_CandyVertex[i * 6 + 4].col = m_CandyVertex[i * 6 + 2].col;
		m_CandyVertex[i * 6 + 4].tex = m_CandyVertex[i * 6 + 2].tex;

		m_CandyVertex[i * 6 + 5].pos = D3DXVECTOR3(GET_PIXEL_X(BOARD_X + TILE_W*(i % BOARD_W)), GET_PIXEL_Y(BOARD_Y + TILE_H*((i / BOARD_H) + 1)), 0.5f);
		m_CandyVertex[i * 6 + 5].col = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		m_CandyVertex[i * 6 + 5].tex = D3DXVECTOR2(0.0f, 0.0f);
	}

	// 버텍스버퍼 생성
	D3D11_BUFFER_DESC VBDesc;
	VBDesc.ByteWidth = sizeof(VertexPCT) * m_iNumVertex;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA VData;
	VData.pSysMem = m_CandyVertex;
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

HRESULT objBoard::LoadShader()
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( _DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	// 정점쉐이더 로드
	ID3DBlob* pVSB = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PCT.hlsl", NULL, NULL, "VS", "vs_5_0", dwShaderFlags, NULL, NULL, &pVSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreateVertexShader((DWORD*)pVSB->GetBufferPointer(), pVSB->GetBufferSize(), NULL, &m_pVS));

	// 픽셀쉐이더 로드
	ID3DBlob* pPSB = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PCT.hlsl", NULL, NULL, "PS", "ps_5_0", dwShaderFlags, NULL, NULL, &pPSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreatePixelShader((DWORD*)pPSB->GetBufferPointer(), pPSB->GetBufferSize(), NULL, &m_pPS));

	// 레이아웃 생성
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 28,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HR_RETURN(m_pDevice->CreateInputLayout(layout, 3, pVSB->GetBufferPointer(), pVSB->GetBufferSize(), &m_pInputLayout));

	return hr;
}
