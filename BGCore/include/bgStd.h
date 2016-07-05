#pragma once
#include <Windows.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <assert.h>
#include <d3d10_1.h>
#include <D3DX10math.h>
#include <d3dx11.h> 
#include <D3Dcompiler.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3dx10d.lib")
#pragma comment(lib, "d3dx11d.lib")

#define RadianToDegree(fAngle)	(fAngle * (3.141592f / 180.0f))
#define DegreeToRadian(fAngle)	(fAngle * (180.0f / 3.141592f))
#define DEBUG_BUFFER_SIZE		(256)

template <class T> class bgSingleton
{
public:
	static T& GetInstance()
	{
		static T theSingleton;
		return theSingleton;
	}
};

#ifndef SAFE_ZERO
#define SAFE_ZERO(A)			{ A = 0; }
#endif

#ifndef SAFE_NEW
#define SAFE_NEW(A, B)			{ if (!A) A = new B; }
#endif 

#ifndef SAFE_DEL
#define SAFE_DEL(A)				{ if (A) delete A; (A)=NULL; }
#endif 

#ifndef SAFE_NEW_ARRAY
#define SAFE_NEW_ARRAY(A, B, C)	{ if (!A && C) A = new B[C]; }
#endif 

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(A)	{ if (A) delete [] A; (A)=NULL; }
#endif 

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(A)			{ if(A) { (A)->Release(); (A)=NULL; } }
#endif 

#ifndef SAFE_NEW_CLEAR
#define SAFE_NEW_CLEAR(A, B)	{ if (!A) A = new B; if(A) memset(A, 0, sizeof(B)); }
#endif 

#ifndef SAFE_NEW_ARRAY_CLEAR
#define SAFE_NEW_ARRAY_CLEAR(A, B, C)	{ if (!A && C) A = new B[C]; if(A) memset(A, 0, sizeof(B)*C); }
#endif

#ifndef COUNTOF
#define COUNTOF(A)	( sizeof(A) / sizeof((A)[0]) )
#endif
