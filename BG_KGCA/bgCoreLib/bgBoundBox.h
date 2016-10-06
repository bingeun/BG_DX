#pragma once
#include "bgShape.h"
#include "bgSys.h"

class bgBoundBox : public bgShape
{
public:
	D3DXVECTOR3		m_vCenter;		// Common
	D3DXVECTOR3		m_vMax;			// AABB
	D3DXVECTOR3		m_vMin;			// AABB
	D3DXVECTOR3		m_vAxis[3];		// OBB
	float			m_fExtent[3];	// OBB

public:
	vector<VertexPC>	m_VertexList;
	MATRIX_BUFFER		m_MatrixBuffer;

public:
	bgBoundBox();
	virtual ~bgBoundBox();

public:
	bool	Init();
	bool	Frame();
	bool	Render(D3DXMATRIX* pMatParent);
	bool	Release();

	HRESULT	CreateBuffer();
	HRESULT	LoadShader();
	void	SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);

	void	SetBoundBox(D3DXVECTOR3* pVecMax, D3DXVECTOR3* pVecMin);
	void	UpdateBoundBox();
};
