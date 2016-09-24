#include "bgQuadTree.h"

bgQuadTree::bgQuadTree()
{
}

bgQuadTree::~bgQuadTree()
{
}

bgTreeNode* bgQuadTree::CreateNode(bgTreeNode* pParentNode, float fMinX, float fMaxX, float fMinZ, float fMaxZ)
{
	bgTreeNode* pNode = NULL;
	SAFE_NEW(pNode, bgTreeNode);

	pNode->m_ChildList.reserve(4);
	pNode->m_CornerList.reserve(4);

	pNode->m_BBox.vMin = D3DXVECTOR3(fMinX, 0.0f, fMinZ);
	pNode->m_BBox.vMax = D3DXVECTOR3(fMaxX, 0.0f, fMaxZ);
	pNode->m_BBox.vCenter = (pNode->m_BBox.vMax + pNode->m_BBox.vMin) / 2.0f;
	pNode->m_BBox.fExtent[0] = pNode->m_BBox.vMax.x - pNode->m_BBox.vCenter.x;
	pNode->m_BBox.fExtent[1] = pNode->m_BBox.vMax.y - pNode->m_BBox.vCenter.y;
	pNode->m_BBox.fExtent[2] = pNode->m_BBox.vMax.z - pNode->m_BBox.vCenter.z;

	pNode->m_CornerList.push_back(D3DXVECTOR3(fMinX, 0.0f, fMaxZ));	// 1	1---2	1 = -x +z
	pNode->m_CornerList.push_back(D3DXVECTOR3(fMaxX, 0.0f, fMaxZ));	// 2	|   |	2 = +x +z
	pNode->m_CornerList.push_back(D3DXVECTOR3(fMinX, 0.0f, fMinZ));	// 3	|   |	3 = -x -z
	pNode->m_CornerList.push_back(D3DXVECTOR3(fMaxX, 0.0f, fMinZ));	// 4	3---4	4 = +x -z

	if (pParentNode)
	{
		pNode->m_dwDepth = pParentNode->m_dwDepth + 1;
	}

	return pNode;
}

void bgQuadTree::BuildTree(bgTreeNode* pNode)
{
	if (SubDivide(pNode))
	{
		for (int iNode = 0; iNode < 4; iNode++)
		{
			BuildTree(pNode->m_ChildList[iNode]);
		}
	}
}

bool bgQuadTree::Build(float fWidth, float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_pRootNode = CreateNode(NULL, -fWidth / 2, fWidth / 2, -fHeight / 2, fHeight / 2);

	//if (BuildTree(m_pRootNode))
	{
		return true;
	}
	return false;
}

bool bgQuadTree::SubDivide(bgTreeNode * pNode)
{
	if (pNode == NULL)
	{
		return false;
	}

	if (m_dwMaxDepth <= pNode->m_dwDepth)
	{
		pNode->m_bLeaf = TRUE;
		return false;
	}

	float fSplitWidth = (pNode->m_CornerList[1].x - pNode->m_CornerList[0].x) / 2;
	float fSplitHeight = (pNode->m_CornerList[0].z - pNode->m_CornerList[2].z) / 2;
	if (fSplitWidth < m_fMinDivideSize || fSplitHeight < m_fMinDivideSize)
	{
		pNode->m_bLeaf = TRUE;
		return false;
	}

	//pNode->m_ChildList.push_back(CreateNode(pNode, pNode->m_CornerList[]))

	return true;
}
