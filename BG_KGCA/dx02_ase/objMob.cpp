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
	// FSM ���� �� ���º��� ��� �߰�
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

	// FSM �ʱ� ���� ����
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

	// ���ؽ� ����
	VertexPC Vertices[] =
	{
		{ { -1.0f, 0.0f, -1.0f },{ 1.0f, 1.0f, 0.0f, 1.0f } },	// 0	3-------2	���� ��ü
		{ { +1.0f, 0.0f, -1.0f },{ 1.0f, 1.0f, 0.0f, 1.0f } },	// 1	|       |	���� ���簢��
		{ { +1.0f, 0.0f, +1.0f },{ 1.0f, 1.0f, 0.0f, 1.0f } },	// 2	|       |	�����
		{ { -1.0f, 0.0f, +1.0f },{ 1.0f, 1.0f, 0.0f, 1.0f } },	// 3	0-------1

		{ { -5.0f, 0.0f, -5.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } },	// 4	7-------6	���� �þ�
		{ { +5.0f, 0.0f, -5.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } },	// 5	|       |	�߰� ���簢��
		{ { +5.0f, 0.0f, +5.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } },	// 6	|       |	�Ͼ��
		{ { -5.0f, 0.0f, +5.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } },	// 7	4-------5

		{ { -9.0f, 0.0f, -9.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },	// 8	11------10	���� �߰� ����
		{ { +9.0f, 0.0f, -9.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },	// 9	 |      |	ū ���簢��
		{ { +9.0f, 0.0f, +9.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },	// 10	 |      |	������
		{ { -9.0f, 0.0f, +9.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },	// 11	 8------9
	};
	m_iNumVertex = COUNTOF(Vertices);

	// �ε��� ����
	UINT iIndices[] =
	{
		0, 1, 1, 2, 2, 3, 3, 0,		// ���� ��ü
		4, 5, 5, 6, 6, 7, 7, 4,		// ���� �þ�
		8, 9, 9, 10, 10, 11, 11, 8,	// ���� �߰� ����
	};
	m_iNumIndex = COUNTOF(iIndices);

	// ���ؽ����� ����
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

	// �ε������� ����
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

	// �������̴� �ε�
	ID3DBlob* pVSB = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PC.hlsl", NULL, NULL, "VS", "vs_5_0", dwShaderFlags, NULL, NULL, &pVSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreateVertexShader((DWORD*)pVSB->GetBufferPointer(), pVSB->GetBufferSize(), NULL, &m_pVS));

	// �ȼ����̴� �ε�
	ID3DBlob* pPSB = NULL;
	HR_RETURN(D3DX11CompileFromFile(L"PC.hlsl", NULL, NULL, "PS", "ps_5_0", dwShaderFlags, NULL, NULL, &pPSB, NULL, NULL));
	HR_RETURN(m_pDevice->CreatePixelShader((DWORD*)pPSB->GetBufferPointer(), pPSB->GetBufferSize(), NULL, &m_pPS));

	// ���̾ƿ� ����
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
