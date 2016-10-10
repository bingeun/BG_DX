#pragma once
#include "bgStd3D.h"

struct bgAnimNode
{
	D3DXMATRIX			matWorld;
	vector<D3DXMATRIX>	matFrameList;
	vector<PNC_VERTEX>	vBipedList;

	ID3D11Buffer*		pBipedBuffer;
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
