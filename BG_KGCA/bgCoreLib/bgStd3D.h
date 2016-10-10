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
	int		iVersion;		// ����
	int		iFirstFrame;	// ���� ������
	int		iLastFrame;		// ���� ������
	int		iFrameSpeed;	// ������ ���ǵ�
	int		iTickPerFrame;	// �����Ӵ� ƽ
	int		iNumMesh;		// �޽� ������Ʈ ����
	int		iMaxWeight;		// ���� �� ����ġ
	int		iBindPose;		// ���ε� ���� ���ϸ��̼� ����

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
