#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgTextureMgr.h"
#include "bgParser.h"
#include "bgModel.h"

class bgParserASE : public bgParser
{
public:
	bgModel*	m_pModel;

public:
	bgParserASE();
	virtual ~bgParserASE();

public:
	bool	Init(bgModel* pModel);

	bool	Read();				// �θ�ü�� �ʼ� ������ �Լ�
	bool	ReadScene();
	bool	ReadMaterial();
	bool	ReadGeomObject();
	bool	ReadHelperObject();
	bool	ReadShapeObject();
	bool	ReadCameraObject();
	bool	ReadLightObject();

	bool	ReadTMAnimation(int iNumObject);

	bool	ConvertToModel();
};
