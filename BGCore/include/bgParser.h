#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgStd.h"

class bgParser
{
public:
	FILE*	m_pFile;
	TCHAR	m_szLine[MAX_PATH];
	TCHAR	m_szWord[MAX_PATH];
	TCHAR	m_szFileName[MAX_PATH];

public:
	bgParser();
	virtual ~bgParser();

public:
	bool	Init();
	bool	Open(TCHAR* szFileName);
	bool	Close();

	bool	FindWord(TCHAR* szFindWord);
	int		FindWordArray(TCHAR szFindWords[][MAX_PATH], int iNumArray);

	TCHAR*	GetPathToFileName(TCHAR* szPath);

public:
	virtual	bool	Read() = 0;
};
