#include "bgModel.h"

#define MAX_BONE_MATRICES 255

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

	m_fElapseTime = 0.0f;

	Clear();

	return true;
}

bool bgModel::Frame()
{
	m_fElapseTime += (g_fSPF * m_pScene->iFrameSpeed);
	m_iCurrentFrame = (int)m_fElapseTime;
	m_fLerpTime = m_fElapseTime - m_iCurrentFrame;

	if (AnimFrame(m_iCurrentFrame, m_fLerpTime, m_iStartFrame, m_iEndFrame, NULL))
	{
		m_iCurrentFrame = m_iStartFrame;
		m_fElapseTime = (float)m_iStartFrame;
	}
	SetBoneMatrices();

	return true;
}

bool bgModel::Render()
{
	UINT iStride;
	UINT iOffset;

	// ------------------------------------ 오브젝트 렌더링 ----------------------------------

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
			//SetMatrix(&m_pAnimList[0]->m_NodeList[iMesh].matCalculation, &m_RenderMatrix.matView, &m_RenderMatrix.matProj);
			SetMatrix(&m_pObjList[iObj]->m_matNodeList[iMesh], &m_RenderMatrix.matView, &m_RenderMatrix.matProj);

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

	// ------------------------------------ 바이패드 렌더링 ----------------------------------

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
				SetMatrix(&m_pAnimList[iAnim]->m_NodeList[iNode].matCalculation, &m_RenderMatrix.matView, &m_RenderMatrix.matProj);

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

	// 본용 버퍼
	D3D11_BUFFER_DESC vbdesc =
	{
		MAX_BONE_MATRICES * sizeof(D3DXMATRIX),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_SHADER_RESOURCE,
		D3D11_CPU_ACCESS_WRITE,
		0
	};
	m_pDevice->CreateBuffer(&vbdesc, NULL, &m_pBoneBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.ElementOffset = 0;
	SRVDesc.Buffer.ElementWidth = MAX_BONE_MATRICES * 4;
	m_pDevice->CreateShaderResourceView(m_pBoneBuffer, &SRVDesc, &m_pBoneBufferRV);

	// 씬 정보가 있다면...
	if (m_pAnimList.size() > 0)
		m_pScene = &m_pAnimList[0]->m_Scene;
	else if (m_pObjList.size() > 0)
		m_pScene = &m_pObjList[0]->m_Scene;

	m_iStartFrame = 0;
	m_iEndFrame = m_pScene->iLastFrame - m_pScene->iFirstFrame;
	m_fElapseTime = (float)m_iStartFrame;

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

bool bgModel::AnimFrame(FLOAT fCurrentFrame, FLOAT fLerpTime, int iStartFrame, int iEndFrame, D3DXMATRIX* pMatrix)
{
	bool bResult = false;

	D3DXVECTOR3 vPos, vScl;
	D3DXMATRIX matPos, matRot, matScl;
	D3DXQUATERNION qTemp, qCurrent, qNext;
	int iCurrentFrame = (int)fCurrentFrame;
	int iNextFrame = iCurrentFrame + 1;

	D3DXMatrixIdentity(&matScl);
	for (int iAnim = 0; iAnim < m_pAnimList.size(); iAnim++)
	{
		for (int iNode = 0; iNode < m_pAnimList[iAnim]->m_NodeList.size(); iNode++)
		{
			if (m_pAnimList[iAnim]->m_NodeList[iNode].vBipedList.size() > 0)
			{
				bgAnimNode* pNode = &m_pAnimList[iAnim]->m_NodeList[iNode];
				pMatrix = &pNode->matCalculation;
				if (iNextFrame >= iEndFrame)
				{
					iCurrentFrame = iStartFrame;
					qCurrent = pNode->qRotList[iCurrentFrame];
					qNext = pNode->qRotList[iCurrentFrame + 1];
					D3DXQuaternionSlerp(&qTemp, &qCurrent, &qNext, fLerpTime);
					D3DXMatrixRotationQuaternion(&matRot, &qTemp);
					D3DXMatrixScaling(&matScl, pNode->vSclList[iCurrentFrame].x, pNode->vSclList[iCurrentFrame].y, pNode->vSclList[iCurrentFrame].z);

					vPos = pNode->vPosList[iCurrentFrame];
					(*pMatrix) = matScl * matRot;
					(*pMatrix)._41 = vPos.x;
					(*pMatrix)._42 = vPos.y;
					(*pMatrix)._43 = vPos.z;
					bResult = true;
				}
				else
				{
					qCurrent = pNode->qRotList[iCurrentFrame];
					qNext = pNode->qRotList[iNextFrame];
					D3DXQuaternionSlerp(&qTemp, &qCurrent, &qNext, fLerpTime);
					D3DXMatrixRotationQuaternion(&matRot, &qCurrent);
					D3DXMatrixScaling(&matScl, pNode->vSclList[iCurrentFrame].x, pNode->vSclList[iCurrentFrame].y, pNode->vSclList[iCurrentFrame].z);

					vPos = pNode->vPosList[iCurrentFrame];
					(*pMatrix) = matScl * matRot;
					(*pMatrix)._41 = vPos.x;
					(*pMatrix)._42 = vPos.y;
					(*pMatrix)._43 = vPos.z;
				}
			}
		}
	}

	return bResult;
}

void bgModel::SetBoneMatrices(D3DXMATRIX* pMatrix, vector<D3DXMATRIX>* pMatrixList)
{
	if (pMatrix == NULL)
		;
	if (pMatrixList == NULL)
		;

	D3DXMATRIX* pMatrices;
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE MappedFaceDest;

	if (SUCCEEDED(g_pDevice->m_pDContext->Map((ID3D11Resource*)m_pBoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedFaceDest)))
	{
		pMatrices = (D3DXMATRIX*)MappedFaceDest.pData;
		for (DWORD dwObject = 0; dwObject < m_pScene->iNumMesh; dwObject++)
		{
			//pMatrices[dwObject] = (*pMatrixList)[dwObject] * pMatrix[dwObject];
			pMatrices[dwObject] = m_pAnimList[0]->m_NodeList[dwObject].matCalculation;
		}
		g_pDevice->m_pDContext->Unmap(m_pBoneBuffer, 0);
	}
}
