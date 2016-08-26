#include "bgParser.h"

bgParser::bgParser()
{
	Init();
}

bgParser::~bgParser()
{
	Close();
}

bool bgParser::Init()
{
	m_pFile = NULL;
	return true;
}

bool bgParser::Open(TCHAR* szFileName)
{
	m_pFile = _tfopen(szFileName, _T("rt"));

	if (m_pFile == NULL)
		return false;

	Read();

	return true;
}

bool bgParser::Close()
{
	if (m_pFile)
		fclose(m_pFile);

	return true;
}

bool bgParser::FindWord(TCHAR* szFindWord)
{
	while (!feof(m_pFile))
	{
		_fgetts(m_szLine, MAX_PATH, m_pFile);
		_stscanf(m_szLine, _T("%s"), m_szWord);
		if (!_tcsicmp(m_szWord, szFindWord))
		{
			return true;
		}
	}
	return false;
}
