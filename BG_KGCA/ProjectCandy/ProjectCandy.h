#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgCore.h"
#include "GameConst.h"

#include "objBoard.h"

struct CandyData
{
	int		iType;
	int		iValue;
};

struct TileData
{
	int		iType;
	int		iValue;
};

struct BoardData
{
	CandyData	Candy;
	TileData	Tile;
};

class ProjectCandy : public bgCore
{
public:
	BoardData	m_Board[BOARD_W][BOARD_H];
	objBoard	m_objBoard;

	D3DXVECTOR2	m_v2BoardPos;
	D3DXVECTOR2	m_v2TileStride;
	D3DXVECTOR2	m_v2TileStrideUV;

public:
	ProjectCandy();
	virtual ~ProjectCandy();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	LRESULT CALLBACK AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	void	InitPlay();
	void	PutTile(float fX, float fY, int iNum);
	void	PutBoard();
};
