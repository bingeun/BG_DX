#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgTextureMgr.h"
#include "bgParser.h"
#include "bgModelASE.h"

class bgParserASE : public bgParser
{
public:
	bgModelASE*	m_pModel;

public:
	bgParserASE();
	virtual ~bgParserASE();

public:
	bool	Init(bgModelASE* pModel);

	bool	Read();				// �θ�ü�� �ʼ� ������ �Լ�
	bool	ReadScene();
	bool	ReadMaterial();
	bool	ReadGeomObject();
	bool	ReadHelperObject();
	bool	ReadShapeObject();
	bool	ReadCameraObject();
	bool	ReadLightObject();

	bool	ReadNodeInfo(int iNumObj);
	bool	ReadTMAnimation(int iNumObj);

	void	ConvertToModel();
	void	LinkNode();
	void	OperationTM();
};
