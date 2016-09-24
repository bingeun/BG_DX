#pragma once
#include "bgBoundBox.h"

class bgTreeNode
{
public:
	DWORD		m_dwDepth;
	BOOL		m_bLeaf;
	bgBoundBox	m_BBox;

	vector<D3DXVECTOR3>		m_CornerList;
	vector<bgTreeNode*>		m_ChildList;

public:
	bgTreeNode();
	virtual ~bgTreeNode();
};
