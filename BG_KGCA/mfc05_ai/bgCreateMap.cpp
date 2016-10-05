// bgCreateMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFC05_AI.h"
#include "bgCreateMap.h"
#include "afxdialogex.h"


// bgCreateMap 대화 상자입니다.

IMPLEMENT_DYNAMIC(bgCreateMap, CDialogEx)

bgCreateMap::bgCreateMap(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CREATE_MAP, pParent)
{

}

bgCreateMap::~bgCreateMap()
{
}

void bgCreateMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(bgCreateMap, CDialogEx)
END_MESSAGE_MAP()


// bgCreateMap 메시지 처리기입니다.
