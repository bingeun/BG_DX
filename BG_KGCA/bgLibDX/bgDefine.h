#pragma once
#include <Windows.h>
#include <d3d11.h>

using namespace std;

template <class T> class bgSingleton
{
public:
	static T& GetInstance()
	{
		static T theSingleton;
		return theSingleton;
	}
};

#ifndef COUNTOF
#define COUNTOF(A)		(sizeof(A) / sizeof((A)[0]))
#endif
#ifndef HR
#define HR(A)			{ hr = (A); }
#endif
#ifndef HR_RETURN
#define HR_RETURN(A)	{ hr = (A); if (FAILED(hr)) { return hr; } }
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
#define SAFE_DELETE_ARRAY(A)	{ if (A) delete [] (A); (A) = NULL; }
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
