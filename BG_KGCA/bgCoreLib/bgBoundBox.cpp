#include "bgBoundBox.h"

bgBoundBox::bgBoundBox()
{
}

bgBoundBox::~bgBoundBox()
{
}

bool bgBoundBox::Init()
{
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	return true;
}

bool bgBoundBox::Frame()
{
	return true;
}

bool bgBoundBox::Render(D3DXMATRIX* pMatParent)
{
	UINT iStride = sizeof(VertexPC);
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

	m_pDContext->RSSetState(m_pRasterizerState);
	m_pDContext->PSSetShader(m_pPS, NULL, 0);

	SetMatrix(pMatParent, &m_MatrixBuffer.matView, &m_MatrixBuffer.matProj);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pDContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	MATRIX_BUFFER* pCBData = (MATRIX_BUFFER*)MappedResource.pData;
	pCBData->matWorld = m_MatrixBuffer.matWorld;
	pCBData->matView = g_pCameraMatrix->matView;
	pCBData->matProj = g_pCameraMatrix->matProj;
	m_pDContext->Unmap(m_pCB, 0);

	m_pDContext->DrawIndexed(m_iNumIndex, 0, 0);

	return true;
}

bool bgBoundBox::Release()
{
	return true;
}

HRESULT bgBoundBox::CreateBuffer()
{
	HRESULT hr = S_OK;

	// 버텍스 정보 - 생성시 아래 함수 호출
	//SetBoundBox(&m_vMax, &m_vMin);

	// 인덱스 정보
	UINT iIndices[] =
	{
		0, 1, 1, 3, 3, 2, 2, 0, // 아래 사각형
		4, 5, 5, 7, 7, 6, 6, 4, // 윗 사각형
		0, 4, 1, 5, 3, 7, 2, 6, // 옆 이음선들
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
	IData.pSysMem = iIndices;
	IData.SysMemPitch = 0;
	IData.SysMemSlicePitch = 0;

	HR_RETURN(m_pDevice->CreateBuffer(&IBDesc, &IData, &m_pIB));

	return hr;
}

HRESULT bgBoundBox::LoadShader()
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

void bgBoundBox::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
{
	if (pWorld)
		m_MatrixBuffer.matWorld = *pWorld;
	if (pView)
		m_MatrixBuffer.matView = *pView;
	if (pProj)
		m_MatrixBuffer.matProj = *pProj;

	D3DXMatrixTranspose(&m_MatrixBuffer.matWorld, &m_MatrixBuffer.matWorld);
	D3DXMatrixTranspose(&m_MatrixBuffer.matView, &m_MatrixBuffer.matView);
	D3DXMatrixTranspose(&m_MatrixBuffer.matProj, &m_MatrixBuffer.matProj);
}

void bgBoundBox::SetBoundBox(D3DXVECTOR3* pVecMax, D3DXVECTOR3* pVecMin)
{
	m_vMax = *pVecMax;
	m_vMin = *pVecMin;

	m_VertexList.clear();
	m_VertexList.resize(8);
	m_VertexList[0].pos = D3DXVECTOR3(m_vMin.x, m_vMin.y, m_vMin.z);	// 0 Min	  6---------7
	m_VertexList[1].pos = D3DXVECTOR3(m_vMax.x, m_vMin.y, m_vMin.z);	// 1		 /|        /|
	m_VertexList[2].pos = D3DXVECTOR3(m_vMin.x, m_vMin.y, m_vMax.z);	// 2		4---------5 |
	m_VertexList[3].pos = D3DXVECTOR3(m_vMax.x, m_vMin.y, m_vMax.z);	// 3		| |       | |
	m_VertexList[4].pos = D3DXVECTOR3(m_vMin.x, m_vMax.y, m_vMin.z);	// 4		| |       | |
	m_VertexList[5].pos = D3DXVECTOR3(m_vMax.x, m_vMax.y, m_vMin.z);	// 5		| 2-------|-3
	m_VertexList[6].pos = D3DXVECTOR3(m_vMin.x, m_vMax.y, m_vMax.z);	// 6		|/        |/
	m_VertexList[7].pos = D3DXVECTOR3(m_vMax.x, m_vMax.y, m_vMax.z);	// 7 Max	0---------1 
	m_VertexList[0].col = D3DXVECTOR4(0.0f, 0.0f, 0.625f, 1.0f);		// 파랑 (Min 0)
	m_VertexList[1].col = D3DXVECTOR4(0.375f, 0.375f, 1.0f, 1.0f);		// 
	m_VertexList[2].col = D3DXVECTOR4(0.375f, 0.375f, 1.0f, 1.0f);		// 하늘 (1 ~ 3)
	m_VertexList[3].col = D3DXVECTOR4(0.375f, 0.375f, 1.0f, 1.0f);		// 
	m_VertexList[4].col = D3DXVECTOR4(1.0f, 0.375f, 0.375f, 1.0f);		// 
	m_VertexList[5].col = D3DXVECTOR4(1.0f, 0.375f, 0.375f, 1.0f);		// 주황 (4 ~ 6)
	m_VertexList[6].col = D3DXVECTOR4(1.0f, 0.375f, 0.375f, 1.0f);		// 
	m_VertexList[7].col = D3DXVECTOR4(0.625f, 0.0f, 0.0f, 1.0f);		// 빨강 (Max 7)
	m_iNumVertex = m_VertexList.size();
}

void bgBoundBox::UpdateBoundBox()
{
	m_pDContext->UpdateSubresource(m_pVB, 0, NULL, &m_VertexList[0], 0, 0);
}
