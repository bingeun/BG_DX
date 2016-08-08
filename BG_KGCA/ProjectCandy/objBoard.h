#pragma once
#include "bgShape.h"
#include "bgSys.h"
#include "GameConst.h"

class objBoard : public bgShape
{
public:

	VertexPCT	m_CandyVertex[BOARD_H * BOARD_W * 6];

public:
	objBoard();
	virtual ~objBoard();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	HRESULT	CreateBuffer();
	HRESULT	LoadShader();
};
