#pragma once
#include "bgSys.h"
#include "bgShape.h"
#include "bgFSMClass.h"

typedef enum
{
	NONE = -1,
	Idle,
	Move,
	Trace,
	Attack,
	MAX
} FSM_STATE_MOB;

typedef enum
{
	InBoundaryPlayer,
	OutBoundaryPlayer,
	InRangePlayer,
	OutRangePlayer,
	IdleLongTime,
	EndMove,
} FSM_INPUT_MOB;

class objMob : public bgFSMClass
{
public:
	float			m_fStateStartTick;
	float			m_fStateEndTick;

public:
	D3DXVECTOR3		m_vPos;
	D3DXVECTOR3		m_vDir;
	D3DXVECTOR3*	m_pHeroPos;

public:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDContext;
	ID3D11RasterizerState*	m_pRasterizerState;
	ID3D11InputLayout*		m_pInputLayout;
	D3D_PRIMITIVE_TOPOLOGY	m_ePrimitiveTopology;

	ID3D11VertexShader*		m_pVS;
	ID3D11PixelShader*		m_pPS;

	ID3D11Buffer*			m_pVB;
	ID3D11Buffer*			m_pIB;
	ID3D11Buffer*			m_pCB;

	UINT			m_iNumVertex;
	UINT			m_iNumIndex;
	MATRIX_BUFFER	m_MatrixBuffer;

public:
	objMob();
	virtual ~objMob();

	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	bool	SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11RasterizerState* pRasterizerState, ID3D11Buffer* pCB = NULL);
	HRESULT	CreateBuffer();
	HRESULT	LoadShader();
	void	SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
};
