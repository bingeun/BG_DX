// bgSaveMapB.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFC05_AI.h"
#include "bgSaveMapB.h"
#include "afxdialogex.h"


// bgSaveMapB 대화 상자입니다.

IMPLEMENT_DYNAMIC(bgSaveMapB, CDialogEx)

bgSaveMapB::bgSaveMapB(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SAVE_B, pParent)
{

}

bgSaveMapB::~bgSaveMapB()
{
}

void bgSaveMapB::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(bgSaveMapB, CDialogEx)
END_MESSAGE_MAP()


// bgSaveMapB 메시지 처리기입니다.
