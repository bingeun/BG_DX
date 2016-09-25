#include "bgCamera.h"

MATRIX_BUFFER* g_pCameraMatrix;

bgCamera::bgCamera()
{
}

bgCamera::~bgCamera()
{
}

void bgCamera::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
{
	m_pDevice = pDevice;
	m_pDContext = pDContext;

	m_vEye = { 0.0f, 20.0f, -50.0f };
	m_vAt = { 0.0f, 0.0f, 0.0f };
	m_vUp = { 0.0f, 1.0f, 0.0f };

	m_fFieldOfView = DEFAULT_FOV;
	m_fAspect = 800.0f / 600.0f;
	m_fScreenNear = 0.1f;
	m_fScreenFar = 10000.0f;

	m_fMoveSpeed = MOVE_VALUE;
	m_fRotateSpeed = ROTATE_VALUE;

	g_pCameraMatrix = &m_Matrix;

	CreateMatrixBuffer();
}

void bgCamera::Frame()
{
	TransMatrix();
}

HRESULT bgCamera::CreateMatrixBuffer()
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC BDesc;
	BDesc.ByteWidth = sizeof(MATRIX_BUFFER);
	BDesc.Usage = D3D11_USAGE_DYNAMIC;
	BDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BDesc.MiscFlags = 0;
	BDesc.StructureByteStride = 0;
	HR_RETURN(m_pDevice->CreateBuffer(&BDesc, NULL, &m_pMatrixBuffer));

	return hr;
}

void bgCamera::UpdateMatrixBuffer()
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pDContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	{
		MATRIX_BUFFER* pData = (MATRIX_BUFFER*)MappedResource.pData;
		pData->matWorld = m_Matrix.matWorld;
		pData->matView = m_Matrix.matView;
		pData->matProj = m_Matrix.matProj;
	}
	m_pDContext->Unmap(m_pMatrixBuffer, 0);
}

void bgCamera::TransMatrix(D3DXMATRIX* pParentWorld)
{
	D3DXMatrixIdentity(&m_Matrix.matWorld);
	D3DXMatrixLookAtLH(&m_Matrix.matView, &m_vEye, &m_vAt, &m_vUp);
	D3DXMatrixPerspectiveFovLH(&m_Matrix.matProj, m_fFieldOfView, m_fAspect, m_fScreenNear, m_fScreenFar);

	if(pParentWorld == NULL)
		D3DXMatrixTranspose(&m_Matrix.matWorld, &m_Matrix.matWorld);
	else
		D3DXMatrixTranspose(&m_Matrix.matWorld, pParentWorld);

	D3DXMatrixTranspose(&m_Matrix.matView, &m_Matrix.matView);
	D3DXMatrixTranspose(&m_Matrix.matProj, &m_Matrix.matProj);
}

void bgCamera::SetView(D3DXVECTOR3 vEye, D3DXVECTOR3 vAt, D3DXVECTOR3 vUp)
{
	m_vEye = vEye;
	m_vAt = vAt;
	m_vUp = vUp;
}

void bgCamera::SetProjection(UINT iWidth, UINT iHeight, float fFov, float fNear, float fFar)
{
	m_fFieldOfView = fFov;
	m_fAspect = (float)iWidth / (float)iHeight;
	m_fScreenNear = fNear;
	m_fScreenFar = fFar;
}

void bgCamera::MoveForward(float fValue)
{
	m_vEye;
	m_vAt;
}

void bgCamera::MoveBackward(float fValue)
{
	m_vEye;
	m_vAt;
}

void bgCamera::MoveLeft(float fValue)
{
	m_vEye;
	m_vAt;
}

void bgCamera::MoveRight(float fValue)
{
	m_vEye;
	m_vAt;
}

void bgCamera::MoveUp(float fValue)
{
	m_vEye;
	m_vAt;
}

void bgCamera::MoveDown(float fValue)
{
	m_vEye;
	m_vAt;
}

void bgCamera::RotateLeft(float fValue)
{
	m_vAt;
}

void bgCamera::RotateRight(float fValue)
{
	m_vAt;
}

void bgCamera::RotateUp(float fValue)
{
	m_vAt;
}

void bgCamera::RotateDown(float fValue)
{
	m_vAt;
}
