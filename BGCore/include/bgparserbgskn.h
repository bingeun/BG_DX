#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgParser.h"
#include "bgObject.h"

class bgParserBGSKN : public bgParser
{
public:
	bgObject*	m_pObject;

public:
	bgParserBGSKN();
	virtual ~bgParserBGSKN();

public:
	void	Set(bgObject* pObject) { m_pObject = pObject; }

	bool	Read();
	bool	ReadScene();
	bool	ReadObject();
	bool	ReadVertex(bgMesh* pMesh);
	bool	ReadIndex(bgMesh* pMesh);
	bool	ReadTexture(bgMesh* pMesh);
	bool	ReadMatrix();

	bool	PostRead();
};
