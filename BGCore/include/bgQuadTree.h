#pragma once
#include "bgTreeNode.h"
#include "bgShape.h"

enum QUADTREE_DIR
{
	QUAD_L,
	QUAD_R,
	QUAD_U,
	QUAD_D,
};

class bgQuadTree
{
public:
	VertexPNCT*		m_pVertexList;
	bgTreeNode*		m_pRootNode;

	float	m_fWidth;
	float	m_fHeight;
	float	m_fMinDivideSize;
	DWORD	m_dwMaxDepth;

public:
	bgQuadTree();
	virtual ~bgQuadTree();

public:
	bgTreeNode*		CreateNode(bgTreeNode* pParentNode, float fMinX, float fMaxX, float fMinZ, float fMaxZ);
	void	BuildTree(bgTreeNode* pNode);
	bool	Build(float fWidth, float fHeight);
	bool	SubDivide(bgTreeNode* pNode);
};
