#pragma once
#include "bgGlobalMax.h"

class bgObjectMax : public bgGlobalMax
{
public:
	bgObjectMax();
	virtual ~bgObjectMax();

public:
	void		GetMaterial();
	void		GetTexture(bgMaterial* pMaterial, Mtl* pMtl);

	void		UpdateObject();
	void		GetObject(bgMatrixIndex* pPoint);
	void		GetNodeHeader(INode* node, bgMesh* pMesh);
	void		GetNodeTM(INode* node, bgMesh* pMesh);
	void		GetMesh(INode* node, bgMesh* pMesh);
	Point3		GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rvertex);
	TriObject*	GetTriObject(INode* node, TimeValue time, int& deleteIt);

	virtual void	GetAnimKeys(INode* node, bgMesh* pMesh) {}
};
