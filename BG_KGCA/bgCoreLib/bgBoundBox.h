#pragma once
#include "bgStd.h"

class bgBoundBox
{
public:
	D3DXVECTOR3		vCenter;

	// AABB
	D3DXVECTOR3		vMax;
	D3DXVECTOR3		vMin;

	// OBB
	D3DXVECTOR3		vAxis[3];
	float			fExtent[3];

public:
	bgBoundBox();
	virtual ~bgBoundBox();
};
