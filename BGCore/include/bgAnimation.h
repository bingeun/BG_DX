#pragma once
#include "bgStd3D.h"

struct bgAnimNode
{
	D3DXMATRIX				matWorld;
	vector<D3DXMATRIX>		matFrameList;
	vector<D3DXQUATERNION>	qRotList;
	vector<D3DXVECTOR3>		vSclList;
	vector<D3DXVECTOR3>		vPosList;
	vector<PNC_VERTEX>		vBipedList;
	D3DXMATRIX				matWorldInv;
	D3DXMATRIX				matCalculation;
	ID3D11Buffer*			pBipedBuffer;
};

class bgAnimation
{
public:
	bgScene				m_Scene;
	vector<bgAnimNode>	m_NodeList;

public:
	bgAnimation();
	virtual ~bgAnimation();

public:
	void	Clear();
};
