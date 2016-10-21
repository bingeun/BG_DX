#include "objMob.h"
#include "StateMobIdle.h"
#include "StateMobMove.h"
#include "StateMobTrace.h"
#include "StateMobAttack.h"

objMob::objMob()
{
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
}

objMob::~objMob()
{
}

bool objMob::Init()
{
	// FSM 상태 및 상태변이 목록 추가
	int iStateID;
	
	AddState(new StateMobIdle);
	GetState(FSM_STATE_MOB::Idle)->m_pObject = this;
	iStateID = GetState(FSM_STATE_MOB::Idle)->GetStateID();
	m_StateMap[iStateID]->AddTrans(FSM_INPUT_MOB::InBoundaryPlayer, FSM_STATE_MOB::Trace);
	m_StateMap[iStateID]->AddTrans(FSM_INPUT_MOB::IdleLongTime, FSM_STATE_MOB::Move);

	AddState(new StateMobMove);
	GetState(FSM_STATE_MOB::Move)->m_pObject = this;
	iStateID = GetState(FSM_STATE_MOB::Move)->GetStateID();
	m_StateMap[iStateID]->AddTrans(FSM_INPUT_MOB::EndMove, FSM_STATE_MOB::Idle);
	m_StateMap[iStateID]->AddTrans(FSM_INPUT_MOB::InBoundaryPlayer, FSM_STATE_MOB::Trace);

	AddState(new StateMobTrace);
	GetState(FSM_STATE_MOB::Trace)->m_pObject = this;
	iStateID = GetState(FSM_STATE_MOB::Trace)->GetStateID();
	m_StateMap[iStateID]->AddTrans(FSM_INPUT_MOB::OutBoundaryPlayer, FSM_STATE_MOB::Idle);
	m_StateMap[iStateID]->AddTrans(FSM_INPUT_MOB::InRangePlayer, FSM_STATE_MOB::Attack);

	AddState(new StateMobAttack);
	GetState(FSM_STATE_MOB::Attack)->m_pObject = this;
	iStateID = GetState(FSM_STATE_MOB::Attack)->GetStateID();
	m_StateMap[iStateID]->AddTrans(FSM_INPUT_MOB::OutRangePlayer, FSM_STATE_MOB::Trace);

	// FSM 초기 상태 설정
	m_pCurrentState = GetState(FSM_STATE_MOB::Idle);
	m_iCurrentStateID = m_pCurrentState->GetStateID();
	m_pCurrentState->Init();

	return true;
}

bool objMob::Frame()
{
	m_pCurrentState->Frame();

	D3DXMatrixIdentity(&m_MatrixBuffer.matWorld);
	D3DXMatrixTranslation(&m_MatrixBuffer.matWorld, m_vPos.x, m_vPos.y, m_vPos.z);
	SetMatrix(&m_MatrixBuffer.matWorld, &m_MatrixBuffer.matView, &m_MatrixBuffer.matProj);

	return true;
}

bool objMob::Render()
{
	UINT iStride = sizeof(VertexPC);
	UINT iOffset = 0;

	m_pDContext->IASetVertexBuffers(0, 1, &m_pVB, &iStride, &iOffset);
	m_pDContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
	m_pDContext->IASetInputLayout(m_pInputLayout);
	m_pDContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	m_pDContext->VSSetShader(m_pVS, NULL, 0);
	m_pDContext->PSSetShader(m_pPS, NULL, 0);
	m_pDContext->RSSetState(m_pRasterizerState);
	m_pDContext->VSSetConstantBuffers(0, 1, &m_pCB);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pDContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	MATRIX_BUFFER* pCBData = (MATRIX_BUFFER*)MappedResource.pData;
	pCBData->matWorld = m_MatrixBuffer.matWorld;
	pCBData->matView = g_pCameraMatrix->matView;
	pCBData->matProj = g_pCameraMatrix->matProj;
	m_pDContext->Unmap(m_pCB, 0);

	//m_pDContext->DrawIndexed(m_iNumIndex, 0, 0);
	m_pCurrentState->Render();

	return true;
}

bool objMob::Release()
{
	return true;
}

bool objMob::SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11RasterizerState* pRasterizerState, ID3D11Buffer* pCB)
{
	m_pDevice = pDevice;
	m_pDContext = pDeviceContext;
	m_pRasterizerState = pRasterizerState;
	m_pCB = pCB;

	return true;
}

HRESULT objMob::CreateBuffer()
{
	HRESULT hr = S_OK;

	// 버텍스 정보
	VertexPC Vertices[] =
	{
		{ { -1.0f, 0.0f, -1.0f },{ 1.0f, 1.0f, 0.0f, 1.0f } },	// 0	3-------2	몬스터 몸체
		{ { +1.0f, 0.0f, -1.0f },{ 1.0f, 1.0f, 0.0f, 1.0f } },	// 1	|       |	작은 정사각형
		{ { +1.0f, 0.0f, +1.0f },{ 1.0f, 1.0f, 0.0f, 1.0f } },	// 2	|       |	노란색
		{ { -1.0f, 0.0f, +1.0f },{ 1.0f, 1.0f, 0.0f, 1.0f } },	// 3	0-------1

		{ { -5.0f, 0.0f, -5.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } },	// 4	7-------6	몬스터 시야
		{ { +5.0f, 0.0f, -5.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } },	// 5	|       |	중간 정사각형
		{ { +5.0f, 0.0f, +5.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } },	// 6	|       |	하얀색
		{ { -5.0f, 0.0f, +5.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } },	// 7	4-------5

		{ { -9.0f, 0.0f, -9.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },	// 8	11------10	몬스터 추격 영역
		{ { +9.0f, 0.0f, -9.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },	// 9	 |      |	큰 정사각형
		{ { +9.0f, 0.0f, +9.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },	// 10	 |      |	빨간색
		{ { -9.0f, 0.0f, +9.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },	// 11	 8------9
	};
	m_iNumVertex = COUNTOF(Vertices);

	// 인덱스 정보
	UINT iIndices[] =
	{
		0, 1, 1, 2, 2, 3, 3, 0,		// 몬스터 몸체
		4, 5, 5, 6, 6, 7, 7, 4,		// 몬스터 시야
		8, 9, 9, 10, 10, 11, 11, 8,	// 몬스터 추격 영역
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

HRESULT objMob::LoadShader()
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

void objMob::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
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
