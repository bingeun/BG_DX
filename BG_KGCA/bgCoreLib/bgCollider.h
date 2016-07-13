#pragma once
#include "bgStd.h"

struct SPHERE
{
	POINT pos;
	float rad;
};

class bgCollider
{
public:
	bgCollider();
	virtual ~bgCollider();

public:
	bool	RectInPoint(RECT& rectDest, POINT& posSrc);
	bool	RectInPointWH(RECT& rectDest, POINT& posSrc);
	bool	RectInRect(RECT& rectDest, RECT& rectSrc);
	bool	RectInRectWH(RECT& rectDest, RECT& rectSrc);

	bool	SphereInPoint(SPHERE& sphDest, POINT& posSrc);
	bool	SphereInVLine(SPHERE& sphDest, POINT& posSrc, int iEndY);
	bool	SphereInHLine(SPHERE& sphDest, POINT& posSrc, int iEndX);
	bool	SphereInSphere(SPHERE& sphDest, SPHERE& sphSrc);
	bool	SphereInSphere(RECT& rectDest, RECT& rectSrc);
	bool	SphereInSphereWH(RECT& rectDest, RECT& rectSrc);
};
