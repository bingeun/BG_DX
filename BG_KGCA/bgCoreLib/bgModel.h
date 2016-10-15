#pragma once
#include "bgObject.h"
#include "bgAnimation.h"
#include "bgTextureMgr.h"
#include "bgSys.h"

class bgModel
{
public:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDContext;

	ID3D11InputLayout*		m_pInputLayout;
	ID3D11RasterizerState*	m_pRasterizerState;
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;

	ID3D11InputLayout*		m_pBipedInputLayout;
	ID3D11RasterizerState*	m_pBipedRasterizerState;
	ID3D11VertexShader*		m_pBipedVertexShader;
	ID3D11PixelShader*		m_pBipedPixelShader;

	ID3D11Buffer*				m_pMatrixBuffer;
	ID3D11Buffer*				m_pBoneBuffer;
	ID3D11ShaderResourceView*	m_pBoneBufferRV;

	MATRIX_BUFFER			m_RenderMatrix;
	D3D_PRIMITIVE_TOPOLOGY	m_ePrimitiveTopology;
	float					m_fCurrentTick;

public:
	bgScene*				m_pScene;
	vector<bgObject*>		m_pObjList;
	vector<bgAnimation*>	m_pAnimList;

	float					m_fElapseTime;
	float					m_fLerpTime;
	int						m_iCurrentFrame;
	int						m_iStartFrame;
	int						m_iEndFrame;
	int						m_iTexID;

public:
	bgModel();
	virtual ~bgModel();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	bool	SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11RasterizerState* pRasterizerState, ID3D11Buffer* pCB = NULL);
	void	Clear();

public:
	HRESULT	CreateBuffer();
	HRESULT	LoadShader(CHAR* szVS = "VS", CHAR* szPS = "PS");

	void	SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
	bool	AnimFrame(FLOAT fCurrentFrame, FLOAT fLerpTime, int iStartFrame, int iEndFrame, D3DXMATRIX* pMatrix);
	void	SetBoneMatrices(D3DXMATRIX* pMatrix = NULL, vector<D3DXMATRIX>* pMatrixList = NULL);
};
