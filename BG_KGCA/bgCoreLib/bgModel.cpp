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
	m_pDevice = NULL;
	m_pDContext = NULL;
	m_pInputLayout = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pBipedInputLayout = NULL;
	m_pBipedVertexShader = NULL;
	m_pBipedPixelShader = NULL;
	m_pMatrixBuffer = NULL;

	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_fCurrentTick = 0.0f;

	Clear();

	return true;
}

bool bgModel::Frame()
{
	m_fCurrentTick += g_fSPF * m_pObjList[0]->m_Scene.iFrameSpeed * m_pObjList[0]->m_Scene.iTickPerFrame;
	if (m_fCurrentTick >= m_pObjList[0]->m_Scene.iLastFrame * m_pObjList[0]->m_Scene.iTickPerFrame)
	{
		m_fCurrentTick = 0.0f;
	}

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	for (int iObj = 0; iObj < m_pObjList.size(); iObj++)
	{
		for (int iMesh = 0; iMesh < m_pObjList[iObj]->m_MeshList.size(); iMesh++)
		{
			m_pObjList[iObj]->Interpolate(&m_pObjList[iObj]->m_MeshList[iMesh], m_fCurrentTick, &matWorld);
		}
	}

	return true;
}

bool bgModel::Render()
{
	UINT iStride;
	UINT iOffset;

	iStride = sizeof(PNCT_VERTEX);
	iOffset = 0;
	
	m_pDContext->IASetInputLayout(m_pInputLayout);
	m_pDContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	m_pDContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pDContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	m_pDContext->HSSetShader(NULL, NULL, 0);
	m_pDContext->DSSetShader(NULL, NULL, 0);
	m_pDContext->GSSetShader(NULL, NULL, 0);
	m_pDContext->PSSetShader(m_pPixelShader, NULL, 0);

	m_pDContext->RSSetState(m_pRasterizerState);
	m_pDContext->VSSetSamplers(0, 1, &g_pDevice->m_pSSTexture);
	m_pDContext->PSSetSamplers(0, 1, &g_pDevice->m_pSSTexture);
	
	for (int iObj = 0; iObj < m_pObjList.size(); iObj++)
	{
		for (int iMesh = 0; iMesh < m_pObjList[iObj]->m_MeshList.size(); iMesh++)
		{
			SetMatrix(&m_pObjList[iObj]->m_MeshList[iMesh].matWorld, &m_RenderMatrix.matView, &m_RenderMatrix.matProj);

			D3D11_MAPPED_SUBRESOURCE MappedResource;
			m_pDContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
			MATRIX_BUFFER* pCBData = (MATRIX_BUFFER*)MappedResource.pData;
			pCBData->matWorld = m_RenderMatrix.matWorld;
			pCBData->matView = g_pCameraMatrix->matView;
			pCBData->matProj = g_pCameraMatrix->matProj;
			m_pDContext->Unmap(m_pMatrixBuffer, 0);

			bgMesh* pMesh = &m_pObjList[iObj]->m_MeshList[iMesh];
			if (pMesh->IndexList.size() > 0)
			{
				m_pDContext->IASetVertexBuffers(0, 1, &pMesh->pVertexBuffer, &iStride, &iOffset);
				m_pDContext->IASetIndexBuffer(pMesh->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

				I_TextureMgr.GetPtr(m_iTexID)->Apply();

				m_pDContext->DrawIndexed(pMesh->IndexList.size(), 0, 0);
			}
		}
	}

	// ----------------------------------------------------------------------

	iStride = sizeof(PNC_VERTEX);
	iOffset = 0;

	m_pDContext->IASetInputLayout(m_pBipedInputLayout);
	m_pDContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	m_pDContext->VSSetShader(m_pBipedVertexShader, NULL, 0);
	m_pDContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	m_pDContext->HSSetShader(NULL, NULL, 0);
	m_pDContext->DSSetShader(NULL, NULL, 0);
	m_pDContext->GSSetShader(NULL, NULL, 0);
	m_pDContext->PSSetShader(m_pBipedPixelShader, NULL, 0);

	m_pDContext->RSSetState(m_pBipedRasterizerState);
	m_pDContext->VSSetSamplers(0, 1, &g_pDevice->m_pSSTexture);
	m_pDContext->PSSetSamplers(0, 1, &g_pDevice->m_pSSTexture);

	for (int iAnim = 0; iAnim < m_pAnimList.size(); iAnim++)
	{
		for (int iNode = 0; iNode < m_pAnimList[iAnim]->m_NodeList.size(); iNode++)
		{
			if (m_pAnimList[iAnim]->m_NodeList[iNode].vBipedList.size() > 0)
			{
				SetMatrix(&m_pAnimList[iAnim]->m_NodeList[iNode].matWorld, &m_RenderMatrix.matView, &m_RenderMatrix.matProj);

				D3D11_MAPPED_SUBRESOURCE MappedResource;
				m_pDContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
				MATRIX_BUFFER* pCBData = (MATRIX_BUFFER*)MappedResource.pData;
				pCBData->matWorld = m_RenderMatrix.matWorld;
				pCBData->matView = g_pCameraMatrix->matView;
				pCBData->matProj = g_pCameraMatrix->matProj;
				m_pDContext->Unmap(m_pMatrixBuffer, 0);

				bgAnimNode* pNode = &m_pAnimList[iAnim]->m_NodeList[iNode];
				if (pNode->vBipedList.size() > 0)
				{
					m_pDContext->IASetVertexBuffers(0, 1, &pNode->pBipedBuffer, &iStride, &iOffset);
					m_pDContext->Draw(pNode->vBipedList.size(), 0);
				}
			}
		}
	}

	return true;
}

bool bgModel::Release()
{
	SAFE_RELEASE(m_pBipedVertexShader);
	SAFE_RELEASE(m_pBipedPixelShader);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pMatrixBuffer);

	return true;
}

bool bgModel::SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11RasterizerState* pRasterizerState, ID3D11Buffer* pCB)
{
	m_pDevice = pDevice;
	m_pDContext = pDeviceContext;
	m_pRasterizerState = pRasterizerState;
	m_pMatrixBuffer = pCB;

	return true;
}

void bgModel::Clear()
{
}

HRESULT bgModel::CreateBuffer()
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.CPUAccessFlags = 0;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA SRData;
	SRData.SysMemPitch = 0;
	SRData.SysMemSlicePitch = 0;

	// 버텍스버퍼
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	for (int iObj = 0; iObj < m_pObjList.size(); iObj++)
	{
		for (int iMesh = 0; iMesh < m_pObjList[iObj]->m_MeshList.size(); iMesh++)
		{
			if (m_pObjList[iObj]->m_MeshList[iMesh].VertexList.size() > 0)
			{
				BufferDesc.ByteWidth = sizeof(PNCT_VERTEX) * m_pObjList[iObj]->m_MeshList[iMesh].VertexList.size();
				SRData.pSysMem = &m_pObjList[iObj]->m_MeshList[iMesh].VertexList[0];
				HR_RETURN(m_pDevice->CreateBuffer(&BufferDesc, &SRData, &m_pObjList[iObj]->m_MeshList[iMesh].pVertexBuffer));
			}
		}
	}

	// 인덱스버퍼
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	for (int iObj = 0; iObj < m_pObjList.size(); iObj++)
	{
		for (int iMesh = 0; iMesh < m_pObjList[iObj]->m_MeshList.size(); iMesh++)
		{
			if (m_pObjList[iObj]->m_MeshList[iMesh].IndexList.size() > 0)
			{
				BufferDesc.ByteWidth = sizeof(UINT) * m_pObjList[iObj]->m_MeshList[iMesh].IndexList.size();
				SRData.pSysMem = &m_pObjList[iObj]->m_MeshList[iMesh].IndexList[0];
				HR_RETURN(m_pDevice->CreateBuffer(&BufferDesc, &SRData, &m_pObjList[iObj]->m_MeshList[iMesh].pIndexBuffer));
			}
		}
	}

	// 바이패드용 버텍스버퍼
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	for (int iAnim = 0; iAnim < m_pAnimList.size(); iAnim++)
	{
		for (int iNode = 0; iNode < m_pAnimList[iAnim]->m_NodeList.size(); iNode++)
		{
			if (m_pAnimList[iAnim]->m_NodeList[iNode].vBipedList.size() > 0)
			{
				BufferDesc.ByteWidth = sizeof(PNC_VERTEX) * m_pAnimList[iAnim]->m_NodeList[iNode].vBipedList.size();
				SRData.pSysMem = &m_pAnimList[iAnim]->m_NodeList[iNode].vBipedList[0];
				HR_RETURN(m_pDevice->CreateBuffer(&BufferDesc, &SRData, &m_pAnimList[iAnim]->m_NodeList[iNode].pBipedBuffer));
			}
		}
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
	HR_RETURN(m_pDevice->CreateVertexShader((DWORD*)pVSB->GetBufferPointer(), pVSB->GetBufferSize(), NULL, &m_pVertexShader));

	// 픽셀쉐이더 로드
	ID3DBlob* pPSB = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PNCT.hlsl", NULL, NULL, szPS, "ps_5_0", dwShaderFlags, NULL, NULL, &pPSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreatePixelShader((DWORD*)pPSB->GetBufferPointer(), pPSB->GetBufferSize(), NULL, &m_pPixelShader));

	// 레이아웃 생성
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 40,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HR_RETURN(m_pDevice->CreateInputLayout(layout, 4, pVSB->GetBufferPointer(), pVSB->GetBufferSize(), &m_pInputLayout));

	// ----------------------------------------------------------------------

	// 바이패드용 정점쉐이더 로드
	ID3DBlob* pBipedVSB = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PNC.hlsl", NULL, NULL, "VS", "vs_5_0", dwShaderFlags, NULL, NULL, &pBipedVSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreateVertexShader((DWORD*)pBipedVSB->GetBufferPointer(), pBipedVSB->GetBufferSize(), NULL, &m_pBipedVertexShader));

	// 바이패드용 픽셀쉐이더 로드
	ID3DBlob* pBipedPSB = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PNC.hlsl", NULL, NULL, "PS", "ps_5_0", dwShaderFlags, NULL, NULL, &pBipedPSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreatePixelShader((DWORD*)pBipedPSB->GetBufferPointer(), pBipedPSB->GetBufferSize(), NULL, &m_pBipedPixelShader));

	// 바이패드용 레이아웃 생성
	const D3D11_INPUT_ELEMENT_DESC layoutBiped[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HR_RETURN(m_pDevice->CreateInputLayout(layoutBiped, 3, pBipedVSB->GetBufferPointer(), pBipedVSB->GetBufferSize(), &m_pBipedInputLayout));

	return hr;
}

void bgModel::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
{
	if (pWorld)
		m_RenderMatrix.matWorld = *pWorld;
	if (pView)
		m_RenderMatrix.matView = *pView;
	if (pProj)
		m_RenderMatrix.matProj = *pProj;

	D3DXMatrixTranspose(&m_RenderMatrix.matWorld, &m_RenderMatrix.matWorld);
	D3DXMatrixTranspose(&m_RenderMatrix.matView, &m_RenderMatrix.matView);
	D3DXMatrixTranspose(&m_RenderMatrix.matProj, &m_RenderMatrix.matProj);
}
