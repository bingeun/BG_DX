#pragma once
#include <Windows.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <map>
#include <list>
#include <vector>
#include <bitset>
#include <string>
#include <algorithm>
#include <functional>
#include <assert.h>

#include "Max02_BG.h"

using namespace std;

#pragma comment(lib, "winmm.lib")

#define RadianToDegree(fAngle)	(fAngle * (3.141592f / 180.0f))
#define DegreeToRadian(fAngle)	(fAngle * (180.0f / 3.141592f))
#define DEBUG_BUFFER_SIZE		(256)

#define NULL_TEXT				_T("(NULL)")

template <class T> class bgSingleton
{
public:
	static T& GetInstance()
	{
		static T theSingleton;
		return theSingleton;
	}
};

typedef basic_string<TCHAR>				T_STR;
typedef basic_string<CHAR>				CHAR_STRING;
typedef vector<basic_string<TCHAR>>		T_STR_VECTOR;
typedef vector<DWORD>					DWORD_VECTOR;

#ifndef COUNTOF
#define COUNTOF(A)		(sizeof(A) / sizeof((A)[0]))
#endif
#ifndef HR
#define HR(A)			{ hr = (A); }
#endif
#ifndef HR_RETURN
#define HR_RETURN(A)	{ hr = (A); if (FAILED(hr)) { return hr; } }
#endif
#ifndef BOOL_RETURN
#define BOOL_RETURN(A)	{ hr = (A); if (FAILED(hr)) { return false; } }
#endif
#ifndef IF_FALSE_RETURN
#define IF_FALSE_RETURN(A)	{ hr = (A); if (hr == false) { return false; } }
#endif

#ifndef SAFE_ZERO
#define SAFE_ZERO(A)			{ (A) = 0; }
#endif
#ifndef SAFE_NEW
#define SAFE_NEW(A, B)			{ if (!(A)) (A) = new (B); }
#endif 
#ifndef SAFE_DEL
#define SAFE_DEL(A)				{ if (A) delete (A); (A) = NULL; }
#endif 
#ifndef SAFE_NEW_ARRAY
#define SAFE_NEW_ARRAY(A, B, C)	{ if (!(A) && (C)) (A) = new (B)[(C)]; }
#endif 
#ifndef SAFE_DEL_ARRAY
#define SAFE_DEL_ARRAY(A)		{ if (A) delete [] (A); (A) = NULL; }
#endif 
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(A)			{ if (A) { (A)->Release(); (A) = NULL; } }
#endif 
#ifndef SAFE_NEW_CLEAR
#define SAFE_NEW_CLEAR(A, B)	{ if (!(A)) (A) = new (B); if (A) memset((A), 0, sizeof(B)); }
#endif 
#ifndef SAFE_NEW_ARRAY_CLEAR
#define SAFE_NEW_ARRAY_CLEAR(A, B, C)	{ if (!(A) && (C)) (A) = new (B)[(C)]; if (A) memset((A), 0, sizeof(B) * (C)); }
#endif
