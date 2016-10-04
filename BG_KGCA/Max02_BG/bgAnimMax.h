#pragma once
#include "bgObjectMax.h"

class bgAnimMax : public bgObjectMax
{
public:
	int		m_iDeltaTick;

public:
	bgAnimMax();
	virtual ~bgAnimMax();

public:
	void	DumpPosSample(INode* pNode, bgMesh* pMesh);
	void	DumpRotSample(INode* pNode, bgMesh* pMesh);
	void	DumpSclSample(INode* pNode, bgMesh* pMesh);
	void	DumpFloatKeys(Control* pControl, bgMesh* pMesh);

	void	GetAnimKeys(INode* pNode, bgMesh* pMesh);
	bool	GetDecompAffine(TimeValue time, INode* pNode, AffineParts* pAP, Point3* pRotAxis = NULL, float* pRotAngle = NULL);
	BOOL	CheckForAnimation(INode* pNode, BOOL& bPos, BOOL& bRot, BOOL& bScl);
};
