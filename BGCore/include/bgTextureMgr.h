#pragma once
#include "bgTexture.h"

#define I_TextureMgr bgTextureMgr::GetInstance()

struct MaterialTexID
{
	int		iID;
	vector<MaterialTexID>	SubIDList;
};

class bgTextureMgr : public bgSingleton<bgTextureMgr>
{
private:
	friend class bgSingleton<bgTextureMgr>;

public:
	map<INT, bgTexture*>	m_List;
	INT			m_iKeyCount;

public:
	INT			Add(TCHAR* szName, UINT iWidth = 0, UINT iHeight = 0);
	bgTexture*	GetPtr(INT iKey);
	bgTexture*	GetPtr(TCHAR* szName);
	INT			GetKey(TCHAR* szName);
	bool		Release();

public:
	bgTextureMgr();
	virtual ~bgTextureMgr();
};
