#pragma once
#include "bgStdMax.h"

using namespace std;

template<class Child>
class bgTemplateMap
{
public:
	typedef map<int, Child*>				TemplateMap;
	typedef typename TemplateMap::iterator	TemplateMapIter;

public:
	TemplateMap			m_TMap;
	TemplateMapIter		m_TMapIter;
	int					m_iCurIndex;

public:
	bgTemplateMap();
	~bgTemplateMap();

public:
	bool		Init();
	bool		Release();

public:
	int			Add(Child*);
	int			Add(const TCHAR* pFileName);
	Child*		GetPtr(DWORD index);
	Child*		GetPtr(const TCHAR* szName);
	int			GetID(Child*);
	int			GetID(const TCHAR* szName);
	int			Count();
};

template<class Child>
bgTemplateMap<Child>::bgTemplateMap()
{
	m_iCurIndex = 0;
	m_TMap.clear();
}

template<class Child>
bgTemplateMap<Child>::~bgTemplateMap()
{
	Release();
}

template<class Child>
bool bgTemplateMap<Child>::Init()
{
	m_iCurIndex = 0;
	m_TMap.clear();
	return true;
}

template<class Child>
bool bgTemplateMap<Child>::Release()
{
	for (TemplateMapIter Iter = m_TMap.begin(); Iter != m_TMap.end(); Iter++)
	{
		Child *pPoint = (Child *)(*Iter).second;
		if (pPoint)
			pPoint->Release();
		else
			return false;
		delete pPoint;
	}
	m_TMap.clear();
	m_iCurIndex = 0;
	return true;
}

template<class Child>
int bgTemplateMap<Child>::Add(Child* pChild)
{
	if (pChild == NULL)
	{
		return 0;
	}
	for (TemplateMapIter Iter = m_TMap.begin(); Iter != m_TMap.end(); Iter++)
	{
		Child *pPoint = (Child *)(*Iter).second;
		if (pPoint == pChild)
		{
			return 0;
		}
	}
	m_TMap.insert(make_pair(m_iCurIndex++, pChild));
	return m_iCurIndex - 1;
}

template<class Child>
int bgTemplateMap<Child>::Add(const TCHAR* pFileName)
{
	if (pFileName)
	{
		TCHAR szFileName[256];
		TCHAR Drive[MAX_PATH];
		TCHAR Dir[MAX_PATH];
		TCHAR FName[MAX_PATH];
		TCHAR Ext[MAX_PATH];

		_tsplitpath_s(pFileName, Drive, Dir, FName, Ext);
		Ext[4] = 0;
		_stprintf_s(szFileName, _T("%s%s"), FName, Ext);

		for (TemplateMapIter Iter = m_TMap.begin(); Iter != m_TMap.end(); Iter++)
		{
			Child *pPoint = (Child *)(*Iter).second;
			if (!_tcsicmp(pPoint->m_szName.c_str(), szFileName))
			{
				return (*Iter).first;
			}
		}
	}

	Child *pPoint = NULL;
	SAFE_NEW(pPoint, Child);
	pPoint->Add(m_iCurIndex, pFileName);
	m_TMap.insert(make_pair(m_iCurIndex++, pPoint));

	return m_iCurIndex - 1;
}

template<class Child>
Child *bgTemplateMap<Child>::GetPtr(DWORD dwIndex)
{
	TemplateMapIter Iter = m_TMap.find(dwIndex);
	if (Iter == m_TMap.end()) return NULL;
	Child *pPoint = (*Iter).second;
	return pPoint;
}

template<class Child>
Child* bgTemplateMap<Child>::GetPtr(const TCHAR* szName)
{
	for (TemplateMapIter Iter = m_TMap.begin(); Iter != m_TMap.end(); Iter++)
	{
		Child *pPoint = (Child *)(*Iter).second;
		if (!_tcsicmp(pPoint->m_szName.c_str(), szName))
		{
			return pPoint;
		}
	}
	return NULL;
}

template<class Child>
int bgTemplateMap<Child>::GetID(Child* pChild)
{
	int iIndex = -1;
	for (TemplateMapIter Iter = m_TMap.begin(); Iter != m_TMap.end(); Iter++)
	{
		Child *pPoint = (Child *)(*Iter).second;
		if (pChild == pPoint)
		{
			iIndex = (*Iter).first;
			break;
		}
	}
	return dwIndex;
}

template<class Child>
int bgTemplateMap<Child>::GetID(const TCHAR* szName)
{
	int iIndex = -1;
	for (TemplateMapIter Iter = m_TMap.begin(); Iter != m_TMap.end(); Iter++)
	{
		Child *pPoint = (Child *)(*Iter).second;
		if (!_tcsicmp(pPoint->m_szName.c_str(), szName))
		{
			iIndex = (*Iter).first;
			break;
		}
	}
	return iIndex;
}

template<class Child>
int bgTemplateMap<Child>::Count()
{
	return (int)m_TMap.size();
}
