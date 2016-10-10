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
	UINT iStride = sizeof(PNCT_VERTEX);
	UINT iOffset = 0;
	
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

				//I_TextureMgr.GetPtr(m_TexIDList[iMaterialRef].iID)->Apply();

				m_pDContext->DrawIndexed(pMesh->IndexList.size(), 0, 0);
			}
		}
	}

	return true;
}

bool bgModel::Release()
{
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

	for (int iObj = 0; iObj < m_pObjList.size(); iObj++)
	{
		for (int iMesh = 0; iMesh < m_pObjList[iObj]->m_MeshList.size(); iMesh++)
		{
			if (m_pObjList[iObj]->m_MeshList[iMesh].VertexList.size() > 0)
			{
				VBDesc.ByteWidth = sizeof(PNCT_VERTEX) * m_pObjList[iObj]->m_MeshList[iMesh].VertexList.size();
				VData.pSysMem = &m_pObjList[iObj]->m_MeshList[iMesh].VertexList[0];
				HR_RETURN(m_pDevice->CreateBuffer(&VBDesc, &VData, &m_pObjList[iObj]->m_MeshList[iMesh].pVertexBuffer));
			}
		}
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

	for (int iObj = 0; iObj < m_pObjList.size(); iObj++)
	{
		for (int iMesh = 0; iMesh < m_pObjList[iObj]->m_MeshList.size(); iMesh++)
		{
			if (m_pObjList[iObj]->m_MeshList[iMesh].IndexList.size() > 0)
			{
				IBDesc.ByteWidth = sizeof(UINT) * m_pObjList[iObj]->m_MeshList[iMesh].IndexList.size();
				IData.pSysMem = &m_pObjList[iObj]->m_MeshList[iMesh].IndexList[0];
				HR_RETURN(m_pDevice->CreateBuffer(&IBDesc, &IData, &m_pObjList[iObj]->m_MeshList[iMesh].pIndexBuffer));
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
	HR_RETURN(D3DX11CompileFromFile(L"PNC.hlsl", NULL, NULL, szVS, "vs_5_0", dwShaderFlags, NULL, NULL, &pVSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreateVertexShader((DWORD*)pVSB->GetBufferPointer(), pVSB->GetBufferSize(), NULL, &m_pVertexShader));

	// 픽셀쉐이더 로드
	ID3DBlob* pPSB = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PNC.hlsl", NULL, NULL, szPS, "ps_5_0", dwShaderFlags, NULL, NULL, &pPSB, NULL, NULL));
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
