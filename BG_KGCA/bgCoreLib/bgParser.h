#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgStd.h"

class bgParser
{
public:
	FILE*	m_pFile;
	TCHAR	m_szFileName[MAX_PATH * 4];
	TCHAR	m_szLine[MAX_PATH * 4];
	TCHAR	m_szWord[MAX_PATH * 4];
	TCHAR*	m_pszToken;

public:
	bgParser();
	virtual ~bgParser();

public:
	bool	Init();
	bool	Release();
	bool	Open(TCHAR* szFileName);
	virtual	bool	Read() = 0;
	bool	Close();


public:
	bool	FindWord(TCHAR* szFindWord);
	int		FindWordArray(TCHAR szFindWords[][MAX_PATH * 4], int iNumArray);
	TCHAR*	GetPathToFileName(TCHAR* szPath);
};
