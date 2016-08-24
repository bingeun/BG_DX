#include "bgTextureMgr.h"

INT bgTextureMgr::Add(TCHAR* szName, UINT iWidth, UINT iHeight)
{
	map<INT, bgTexture*>::iterator itor;

	for (itor = m_List.begin(); itor != m_List.end(); itor++)
	{
		bgTexture* data = (*itor).second;
		if (!_tcsicmp(szName, data->m_szName))
		{
			return (*itor).first;
		}
	}

	bgTexture* data = new bgTexture;
	data->Init();
	if (data->Load(szName, iWidth, iHeight) == S_OK)
	{
		m_List.insert(make_pair(m_iKeyCount++, data));
	}

	return m_iKeyCount - 1;
}

bgTexture* bgTextureMgr::GetPtr(INT iKey)
{
	map<INT, bgTexture*>::iterator itor;

	itor = m_List.find(iKey);
	if (itor == m_List.end())
		return NULL;

	return (*itor).second;
}

bgTexture* bgTextureMgr::GetPtr(TCHAR* szName)
{
	map<INT, bgTexture*>::iterator itor;

	for (itor = m_List.begin(); itor != m_List.end(); itor++)
	{
		if (szName == (*itor).second->m_szName)
		{
			return (*itor).second;
		}
	}

	return NULL;
}

INT bgTextureMgr::GetKey(TCHAR* szName)
{
	map<INT, bgTexture*>::iterator itor;

	for (itor = m_List.begin(); itor != m_List.end(); itor++)
	{
		if (szName == (*itor).second->m_szName)
		{
			return (*itor).first;
		}
	}

	return NULL;
}

bool bgTextureMgr::Release()
{
	map<INT, bgTexture*>::iterator itor;

	for (itor = m_List.begin(); itor != m_List.end(); itor++)
	{
		bgTexture* data = (*itor).second;
		delete data;
	}
	m_List.clear();

	return true;
}

bgTextureMgr::bgTextureMgr()
{
	m_iKeyCount = 0;
	m_List.clear();
}

bgTextureMgr::~bgTextureMgr()
{
	Release();
}
