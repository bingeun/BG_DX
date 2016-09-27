#include "bgCamera.h"
#include "bgDInput.h"

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

	m_vEye = D3DXVECTOR3(0.0f, 20.0f, -50.0f);
	m_vAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	m_fMouseX = (float)I_DInput.m_iMouseX;
	m_fMouseY = (float)I_DInput.m_iMouseY;
	m_fMouseZ = (float)I_DInput.m_iMouseZ;

	m_fFieldOfView = DEFAULT_FOV;
	m_fAspect = 800.0f / 600.0f;
	m_fScreenNear = 0.1f;
	m_fScreenFar = 10000.0f;

	m_fMoveSpeed = MOVE_VALUE;
	m_fRotateSpeed = ROTATE_VALUE;

	g_pCameraMatrix = &m_Matrix;

	D3DXQuaternionRotationYawPitchRoll(&m_qRotate, 0.0f, 0.0f, 0.0f);
	D3DXQuaternionRotationYawPitchRoll(&m_qDrag, 0.0f, 0.0f, 0.0f);

	CreateMatrixBuffer();
}

void bgCamera::Frame()
{
	D3DXVECTOR3 vAxis;
	FLOAT fAngle;

	// 키보드 입력 - 회전 =================================================================
	if (I_DInput.IsKeyDown(DIK_Q))
	{
	}
	if (I_DInput.IsKeyDown(DIK_E))
	{
	}

	// 마우스 왼쪽 드래그 - 카메라 1인칭시점 회전 =================================================================
	if (I_DInput.m_MouseState.rgbButtons[0])
	{
		// 눌림 지속 (드래그)
		if (I_DInput.m_MouseStateBefore.rgbButtons[0])
		{
			m_fMouseDragX = m_fMouseX + (float)I_DInput.m_iMouseX - m_fMouseDownX;
			m_fMouseDragY = m_fMouseY + (float)I_DInput.m_iMouseY - m_fMouseDownY;
			m_fMouseDragZ = m_fMouseZ + (float)I_DInput.m_iMouseZ - m_fMouseDownZ;

			D3DXQuaternionRotationYawPitchRoll(&m_qDrag, m_fMouseDragX * D3DX_PI / 180.0f * 0.125f, m_fMouseDragY * D3DX_PI / 180.0f * 0.125f, 0.0f);

			D3DXMatrixIdentity(&m_Matrix.matWorld);
			D3DXMatrixAffineTransformation(&m_Matrix.matView, 1.0f, NULL, &m_qDrag, &m_vEye);
			D3DXMatrixInverse(&m_Matrix.matView, NULL, &m_Matrix.matView);
			D3DXMatrixPerspectiveFovLH(&m_Matrix.matProj, m_fFieldOfView, m_fAspect, m_fScreenNear, m_fScreenFar);

			D3DXMatrixTranspose(&m_Matrix.matWorld, &m_Matrix.matWorld);
			D3DXMatrixTranspose(&m_Matrix.matView, &m_Matrix.matView);
			D3DXMatrixTranspose(&m_Matrix.matProj, &m_Matrix.matProj);
		}
		// 누르는 순간
		else
		{
			m_fMouseDownX = (float)I_DInput.m_iMouseX;
			m_fMouseDownY = (float)I_DInput.m_iMouseY;
			m_fMouseDownZ = (float)I_DInput.m_iMouseZ;
		}
	}
	else
	{
		// 떼는 순간
		if (I_DInput.m_MouseStateBefore.rgbButtons[0])
		{
			m_qRotate = m_qDrag;
			m_fMouseX += (float)I_DInput.m_iMouseX - m_fMouseDownX;
			m_fMouseY += (float)I_DInput.m_iMouseY - m_fMouseDownY;
			m_fMouseZ += (float)I_DInput.m_iMouseZ - m_fMouseDownZ;
		}
		// 안눌림 지속
		else
		{
			D3DXMatrixIdentity(&m_Matrix.matWorld);
			D3DXMatrixAffineTransformation(&m_Matrix.matView, 1.0f, NULL, &m_qRotate, &m_vEye);
			D3DXMatrixInverse(&m_Matrix.matView, NULL, &m_Matrix.matView);
			D3DXMatrixPerspectiveFovLH(&m_Matrix.matProj, m_fFieldOfView, m_fAspect, m_fScreenNear, m_fScreenFar);

			D3DXMatrixTranspose(&m_Matrix.matWorld, &m_Matrix.matWorld);
			D3DXMatrixTranspose(&m_Matrix.matView, &m_Matrix.matView);
			D3DXMatrixTranspose(&m_Matrix.matProj, &m_Matrix.matProj);
		}
	}
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
	D3DXMATRIX matRot;
	D3DXQUATERNION qRot;
	D3DXQuaternionNormalize(&qRot, &m_qRotate);
	D3DXMatrixRotationQuaternion(&matRot, &qRot);
	m_vEye.x += matRot._31 * g_fSPF * fValue * 20.0f;
	m_vEye.y += matRot._32 * g_fSPF * fValue * 20.0f;
	m_vEye.z += matRot._33 * g_fSPF * fValue * 20.0f;
}

void bgCamera::MoveBackward(float fValue)
{
	D3DXMATRIX matRot;
	D3DXQUATERNION qRot;
	D3DXQuaternionNormalize(&qRot, &m_qRotate);
	D3DXMatrixRotationQuaternion(&matRot, &qRot);
	m_vEye.x -= matRot._31 * g_fSPF * fValue * 20.0f;
	m_vEye.y -= matRot._32 * g_fSPF * fValue * 20.0f;
	m_vEye.z -= matRot._33 * g_fSPF * fValue * 20.0f;
}

void bgCamera::MoveLeft(float fValue)
{
	D3DXMATRIX matRot;
	D3DXQUATERNION qRot;
	D3DXQuaternionNormalize(&qRot, &m_qRotate);
	D3DXMatrixRotationQuaternion(&matRot, &qRot);
	m_vEye.x -= matRot._11 * g_fSPF * fValue * 20.0f;
	m_vEye.y -= matRot._12 * g_fSPF * fValue * 20.0f;
	m_vEye.z -= matRot._13 * g_fSPF * fValue * 20.0f;
}

void bgCamera::MoveRight(float fValue)
{
	D3DXMATRIX matRot;
	D3DXQUATERNION qRot;
	D3DXQuaternionNormalize(&qRot, &m_qRotate);
	D3DXMatrixRotationQuaternion(&matRot, &qRot);
	m_vEye.x += matRot._11 * g_fSPF * fValue * 20.0f;
	m_vEye.y += matRot._12 * g_fSPF * fValue * 20.0f;
	m_vEye.z += matRot._13 * g_fSPF * fValue * 20.0f;
}

void bgCamera::MoveUp(float fValue)
{
	D3DXMATRIX matRot;
	D3DXQUATERNION qRot;
	D3DXQuaternionNormalize(&qRot, &m_qRotate);
	D3DXMatrixRotationQuaternion(&matRot, &qRot);
	m_vEye.x += matRot._21 * g_fSPF * fValue * 20.0f;
	m_vEye.y += matRot._22 * g_fSPF * fValue * 20.0f;
	m_vEye.z += matRot._23 * g_fSPF * fValue * 20.0f;
}

void bgCamera::MoveDown(float fValue)
{
	D3DXMATRIX matRot;
	D3DXQUATERNION qRot;
	D3DXQuaternionNormalize(&qRot, &m_qRotate);
	D3DXMatrixRotationQuaternion(&matRot, &qRot);
	m_vEye.x -= matRot._21 * g_fSPF * fValue * 20.0f;
	m_vEye.y -= matRot._22 * g_fSPF * fValue * 20.0f;
	m_vEye.z -= matRot._23 * g_fSPF * fValue * 20.0f;
}

void bgCamera::RotateLeft(float fValue)
{
	m_fMouseX -= fValue * g_fSPF * 80.0f;
	m_fMouseDragX = m_fMouseX;
	D3DXQuaternionRotationYawPitchRoll(&m_qDrag, m_fMouseDragX * D3DX_PI / 180.0f * 0.125f, m_fMouseDragY * D3DX_PI / 180.0f * 0.125f, 0.0f);
	m_qRotate = m_qDrag;
}

void bgCamera::RotateRight(float fValue)
{
	m_fMouseX += fValue * g_fSPF * 80.0f;
	m_fMouseDragX = m_fMouseX;
	D3DXQuaternionRotationYawPitchRoll(&m_qDrag, m_fMouseDragX * D3DX_PI / 180.0f * 0.125f, m_fMouseDragY * D3DX_PI / 180.0f * 0.125f, 0.0f);
	m_qRotate = m_qDrag;
}

void bgCamera::RotateUp(float fValue)
{
	m_fMouseY -= fValue * g_fSPF * 80.0f;
	m_fMouseDragY = m_fMouseY;
	D3DXQuaternionRotationYawPitchRoll(&m_qDrag, m_fMouseDragX * D3DX_PI / 180.0f * 0.125f, m_fMouseDragY * D3DX_PI / 180.0f * 0.125f, 0.0f);
	m_qRotate = m_qDrag;
}

void bgCamera::RotateDown(float fValue)
{
	m_fMouseY += fValue * g_fSPF * 80.0f;
	m_fMouseDragY = m_fMouseY;
	D3DXQuaternionRotationYawPitchRoll(&m_qDrag, m_fMouseDragX * D3DX_PI / 180.0f * 0.125f, m_fMouseDragY * D3DX_PI / 180.0f * 0.125f, 0.0f);
	m_qRotate = m_qDrag;
}
