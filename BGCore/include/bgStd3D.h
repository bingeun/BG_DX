#pragma once
#include "bgStd.h"

struct PNC_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	D3DXVECTOR4 c;
};

struct PNCT_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	D3DXVECTOR4 c;
	D3DXVECTOR2 t;
};

struct bgScene
{
	int		iVersion;		// 버전
	int		iFirstFrame;	// 시작 프레임
	int		iLastFrame;		// 종료 프레임
	int		iFrameSpeed;	// 프레임 스피드
	int		iTickPerFrame;	// 프레임당 틱
	int		iNumMesh;		// 메쉬 오브젝트 개수
	int		iMaxWeight;		// 정점 당 가중치
	int		iBindPose;		// 바인딩 포즈 에니메이션 여부

	void	Clear()
	{
		iVersion = 0;
		iFirstFrame = 0;
		iLastFrame = 0;
		iFrameSpeed = 0;
		iTickPerFrame = 0;
		iNumMesh = 0;
		iMaxWeight = 0;
		iBindPose = 0;
	}
};

struct bgTexMap
{
	int		iType;
	TCHAR	szBitmap[MAX_PATH];
};

struct bgMaterial
{
	vector<bgTexMap>		TextureList;
	vector<bgMaterial>		SubList;
};
