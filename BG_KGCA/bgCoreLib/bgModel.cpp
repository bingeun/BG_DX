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
	m_fCurrentTick += g_fSPF * m_Scene.iFrameSpeed * m_Scene.iTicksPerFrame;
	if (m_fCurrentTick >= m_Scene.iLastFrame * m_Scene.iTicksPerFrame)
	{
		m_fCurrentTick = 0.0f; // m_Scene.iLastFrame * m_Scene.iTicksPerFrame;
	}

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	for (int iLoop = 0; iLoop < m_ObjectList.size(); iLoop++)
	{
		if (m_ObjectList[iLoop].bAnim)
		{
			Interpolate(&m_ObjectList[iLoop], m_fCurrentTick, &matWorld);
		}
	}

	for (int iLoop = 0; iLoop < m_ObjectList.size(); iLoop++)
	{
		//D3DXMatrixMultiply(&m_ObjectList[iLoop].matCalculation, &m_ObjectList[iLoop].matCalculation, &m_ObjectList[iLoop].matControlWorld);
	}

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

	for (int i = 0; i < m_IndexList.size(); i++)
	{
		SetMatrix(&m_ObjectList[0].matCalculation, &m_MatrixBuffer.matView, &m_MatrixBuffer.matProj);

		D3D11_MAPPED_SUBRESOURCE MappedResource;
		m_pDContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		MATRIX_BUFFER* pCBData = (MATRIX_BUFFER*)MappedResource.pData;
		if (m_ObjectList[0].bAnim)
			pCBData->matWorld = m_MatrixBuffer.matWorld;
		else
			pCBData->matWorld = g_MatrixBuffer.matWorld;
		pCBData->matView = g_MatrixBuffer.matView;
		pCBData->matProj = g_MatrixBuffer.matProj;
		m_pDContext->Unmap(m_pCB, 0);

		m_pDContext->IASetVertexBuffers(0, 1, &m_pVBList[i], &iStride, &iOffset);
		m_pDContext->IASetIndexBuffer(m_pIBList[i], DXGI_FORMAT_R32_UINT, 0);
		if (m_TexIDList[0].SubIDList.size() == 0)
			I_TextureMgr.GetPtr(m_TexIDList[0].iID)->Apply();
		else
			I_TextureMgr.GetPtr(m_TexIDList[0].SubIDList[i].iID)->Apply();

		m_pDContext->DrawIndexed(m_IndexList[i].size(), 0, 0);
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

void bgModel::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
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

void bgModel::Interpolate(ObjectNode* pNodeObject, float fFrameTick, D3DXMATRIX* matParent)
{
	D3DXQUATERNION qRot, qScl;
	D3DXMATRIX matAnim, matPos, matRot, matScl;
	AnimTrackInfo* pPrevTrack;
	AnimTrackInfo* pNextTrack;
	float fPrevTick, fNextTick;
	int iTrackIndex;

	D3DXMatrixIdentity(&pNodeObject->matCalculation);
	matPos = pNodeObject->matWorldPos;
	matRot = pNodeObject->matWorldRot;
	matScl = pNodeObject->matWorldScl;

	D3DXQuaternionRotationMatrix(&qRot, &matRot);
	D3DXQuaternionRotationMatrix(&qScl, &matScl);

	fPrevTick = m_Scene.iFirstFrame * m_Scene.iTicksPerFrame;
	fNextTick = 0.0f;

	// 회전 처리
	if (pNodeObject->Anim.RotTrack.size())
	{
		iTrackIndex = GetTrackIndex(fFrameTick, &pNodeObject->Anim.RotTrack);

		// 제일 첫번째 트랙 이후이면
		if (iTrackIndex >= 0)
		{
			qRot = pNodeObject->Anim.RotTrack[iTrackIndex].qRotate;
			fPrevTick = pNodeObject->Anim.RotTrack[iTrackIndex].iTick;
		}
		// 제일 마지막 트랙 이전이면
		if (iTrackIndex < pNodeObject->Anim.RotTrack.size() - 1)
		{
			fNextTick = pNodeObject->Anim.RotTrack[iTrackIndex + 1].iTick;
			D3DXQuaternionSlerp(&qRot, &qRot,
				&pNodeObject->Anim.RotTrack[iTrackIndex + 1].qRotate,
				(fFrameTick - fPrevTick) / (fNextTick - fPrevTick));
		}
		D3DXMatrixRotationQuaternion(&matRot, &qRot);
	}

	// 이동 처리
	D3DXVECTOR3 vPos(matPos._41, matPos._42, matPos._43);
	if (pNodeObject->Anim.PosTrack.size())
	{
		iTrackIndex = GetTrackIndex(fFrameTick, &pNodeObject->Anim.PosTrack);

		// 제일 첫번째 트랙 이후이면
		if (iTrackIndex >= 0)
		{
			vPos = pNodeObject->Anim.PosTrack[iTrackIndex].vVector;
			fPrevTick = pNodeObject->Anim.PosTrack[iTrackIndex].iTick;
		}
		// 제일 마지막 트랙 이전이면
		if (iTrackIndex < pNodeObject->Anim.PosTrack.size() - 1)
		{
			fNextTick = pNodeObject->Anim.PosTrack[iTrackIndex + 1].iTick;
			D3DXVec3Lerp(&vPos, &vPos,
				&pNodeObject->Anim.PosTrack[iTrackIndex + 1].vVector,
				(fFrameTick - fPrevTick) / (fNextTick - fPrevTick));
		}
		D3DXMatrixTranslation(&matPos, vPos.x, vPos.y, vPos.z);
	}

	// 신축 처리
	D3DXMATRIX matScaleRot, matInvScaleRot;
	D3DXVECTOR3 vScl(matScl._11, matScl._22, matScl._33);
	if (pNodeObject->Anim.SclTrack.size())
	{
		iTrackIndex = GetTrackIndex(fFrameTick, &pNodeObject->Anim.SclTrack);

		// 제일 첫번째 트랙 이후이면
		if (iTrackIndex >= 0)
		{
			vScl = pNodeObject->Anim.SclTrack[iTrackIndex].vVector;
			qScl = pNodeObject->Anim.SclTrack[iTrackIndex].qRotate;
			fPrevTick = pNodeObject->Anim.SclTrack[iTrackIndex].iTick;
		}
		// 제일 마지막 트랙 이전이면
		if (iTrackIndex < pNodeObject->Anim.SclTrack.size() - 1)
		{
			fNextTick = pNodeObject->Anim.SclTrack[iTrackIndex + 1].iTick;
			D3DXVec3Lerp(&vScl, &vScl,
				&pNodeObject->Anim.SclTrack[iTrackIndex + 1].vVector,
				(fFrameTick - fPrevTick) / (fNextTick - fPrevTick));
			D3DXQuaternionSlerp(&qScl, &qScl,
				&pNodeObject->Anim.SclTrack[iTrackIndex + 1].qRotate,
				(fFrameTick - fPrevTick) / (fNextTick - fPrevTick));
		}
		D3DXMatrixScaling(&matScl, vScl.x, vScl.y, vScl.z);
		D3DXMatrixRotationQuaternion(&matScaleRot, &qScl);
		D3DXMatrixInverse(&matInvScaleRot, NULL, &matScaleRot);
		matScl = matInvScaleRot  * matScl * matScaleRot;
	}

	// 최종 행렬
	D3DXMatrixMultiply(&matAnim, &matScl, &matRot);
	matAnim._41 = matPos._41;
	matAnim._42 = matPos._42;
	matAnim._43 = matPos._43;
	D3DXMatrixMultiply(&pNodeObject->matCalculation, &matAnim, matParent);

	// 인버스 매트릭스 확인 코드
	D3DXVECTOR3 v0, v1, v2, v3;
	v0 = pNodeObject->matCalculation.m[0];
	v1 = pNodeObject->matCalculation.m[1];
	v2 = pNodeObject->matCalculation.m[2];
	D3DXVec3Cross(&v3, &v1, &v2);
	if (D3DXVec3Dot(&v3, &v0) < 0.0f)
	{
		D3DXMATRIX matW;
		D3DXMatrixScaling(&matW, -1.0f, -1.0f, -1.0f);
		D3DXMatrixMultiply(&pNodeObject->matCalculation, &pNodeObject->matCalculation, &matW);
	}
}

int bgModel::GetTrackIndex(float fTick, vector<AnimTrackInfo>* pTrackList)
{
	int iTrack;
	for (iTrack = 0; iTrack < pTrackList->size(); iTrack++)
	{
		if (fTick < pTrackList->at(iTrack).iTick)
		{
			return iTrack - 1;
		}
	}
	return iTrack;
}
