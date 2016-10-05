// bgSaveMapT.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFC05_AI.h"
#include "bgSaveMapT.h"
#include "afxdialogex.h"


// bgSaveMapT 대화 상자입니다.

IMPLEMENT_DYNAMIC(bgSaveMapT, CDialogEx)

bgSaveMapT::bgSaveMapT(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SAVE_T, pParent)
{

}

bgSaveMapT::~bgSaveMapT()
{
}

void bgSaveMapT::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(bgSaveMapT, CDialogEx)
END_MESSAGE_MAP()


// bgSaveMapT 메시지 처리기입니다.
