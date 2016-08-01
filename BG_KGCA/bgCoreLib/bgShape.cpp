#include "bgShape.h"

MATRIX_BUFFER	g_MatrixBuffer;

bgShape::bgShape()
{
	m_pDevice = NULL;
	m_pDContext = NULL;
	m_pInputLayout = NULL;
	m_pRasterizerState = NULL;
	m_ePrimitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;

	m_pVS = NULL;
	m_pPS = NULL;
	m_pGS = NULL;
	m_pHS = NULL;
	m_pDS = NULL;
	m_pCS = NULL;

	m_pVB = NULL;
	m_pIB = NULL;
	m_pCB = NULL;

	m_iNumVertex = 0;
	m_iNumIndex = 0;
}

bgShape::~bgShape()
{
	Release();
}

bool bgShape::Init()
{
	return true;
}

bool bgShape::Frame()
{
	return true;
}

bool bgShape::Render()
{
	return true;
}

bool bgShape::Release()
{
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pGS);
	SAFE_RELEASE(m_pHS);
	SAFE_RELEASE(m_pDS);
	SAFE_RELEASE(m_pCS);

	SAFE_RELEASE(m_pCB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pVB);
	return true;
}

bool bgShape::SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11RasterizerState* pRasterizerState, ID3D11Buffer* pCB)
{
	m_pDevice = pDevice;
	m_pDContext = pDeviceContext;
	m_pRasterizerState = pRasterizerState;
	m_pCB = pCB;
	return true;
}
