#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "bgParser.h"
#include "bgAnimation.h"

class bgParserBGMTX : public bgParser
{
public:
	bgAnimation*	m_pAnimation;

public:
	bgParserBGMTX();
	virtual ~bgParserBGMTX();

public:
	void	Set(bgAnimation* pAnimation) { m_pAnimation = pAnimation; }

	bool	Read();
	bool	ReadScene();
	bool	ReadNode();
	bool	ReadVertex(bgAnimNode* pNode);
	bool	ReadMatrix(bgAnimNode* pNode);

	bool	PostRead();
};
