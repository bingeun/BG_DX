#pragma once
#include "bgArcBall.h"

const float DEFAULT_FOV = (float)D3DX_PI / 4.0f;
const float MOVE_VALUE = 10.0f;
const float ROTATE_VALUE = 10.0f;

class bgCamera
{
public:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDContext;

	ID3D11Buffer*			m_pMatrixBuffer;
	MATRIX_BUFFER			m_Matrix;
	//bgArcBall				m_ArcBall;
	D3DXQUATERNION			m_qRotate;
	D3DXQUATERNION			m_qDrag;

public:
	D3DXVECTOR3		m_vEye;
	D3DXVECTOR3		m_vAt;
	D3DXVECTOR3		m_vUp;

	float	m_fMouseX;
	float	m_fMouseY;
	float	m_fMouseZ;
	float	m_fMouseDragX;
	float	m_fMouseDragY;
	float	m_fMouseDragZ;
	float	m_fMouseDownX;
	float	m_fMouseDownY;
	float	m_fMouseDownZ;

	float	m_fFieldOfView;
	float	m_fAspect;
	float	m_fScreenNear;
	float	m_fScreenFar;

	float	m_fMoveSpeed;
	float	m_fRotateSpeed;

public:
	bgCamera();
	virtual ~bgCamera();

public:
	void	Init(ID3D11Device* pDevice = NULL, ID3D11DeviceContext* pDContext = NULL);
	void	Frame();

	HRESULT	CreateMatrixBuffer();
	void	UpdateMatrixBuffer();
	void	TransMatrix(D3DXMATRIX* pParentWorld = NULL);

public:
	void	SetView(D3DXVECTOR3 vEye, D3DXVECTOR3 vAt, D3DXVECTOR3 vUp);
	void	SetProjection(UINT iWidth, UINT iHeight, float fFov = DEFAULT_FOV, float fNear = 0.1f, float fFar = 10000.0f);
	void	SetSpeed(float fMoveSpeed, float fRotateSpeed) { m_fMoveSpeed = fMoveSpeed; m_fRotateSpeed = fRotateSpeed; }

	void	MoveForward(float fValue = MOVE_VALUE);
	void	MoveBackward(float fValue = MOVE_VALUE);
	void	MoveLeft(float fValue = MOVE_VALUE);
	void	MoveRight(float fValue = MOVE_VALUE);
	void	MoveUp(float fValue = MOVE_VALUE);
	void	MoveDown(float fValue = MOVE_VALUE);

	void	RotateLeft(float fValue = ROTATE_VALUE);
	void	RotateRight(float fValue = ROTATE_VALUE);
	void	RotateUp(float fValue = ROTATE_VALUE);
	void	RotateDown(float fValue = ROTATE_VALUE);
};
