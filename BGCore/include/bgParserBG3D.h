#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgTextureMgr.h"
#include "bgParser.h"
#include "bgModelBG3D.h"

class bgParserBG3D : public bgParser
{
public:
	bgModelBG3D*	m_pModel;

public:
	bgParserBG3D();
	virtual ~bgParserBG3D();

public:
	bool	Init(bgModelBG3D* pModel);

	bool	Read();				// 부모객체의 필수 재정의 함수
	bool	ReadScene();
	bool	ReadMaterial();
	bool	ReadTexture(int iIndex, MaterialInfo* pMat, SubMaterialInfo* pSubMat = NULL);
	bool	ReadObject();
	bool	ReadVertex(GeomObject* pMesh, int iMtrl, int iNumVertex);
	bool	ReadIndex(GeomObject* pMesh, int iMtrl, int iNumIndex);
	bool	ReadAnimation(TMAnimation* pMesh);

	void	ConvertToModel();
	void	LinkNode();
	void	OperationTM();
};
