// bgLoadMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFC05_AI.h"
#include "bgLoadMap.h"
#include "afxdialogex.h"


// bgLoadMap 대화 상자입니다.

IMPLEMENT_DYNAMIC(bgLoadMap, CDialogEx)

bgLoadMap::bgLoadMap(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LOAD_MAP, pParent)
{

}

bgLoadMap::~bgLoadMap()
{
}

void bgLoadMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(bgLoadMap, CDialogEx)
END_MESSAGE_MAP()


// bgLoadMap 메시지 처리기입니다.
