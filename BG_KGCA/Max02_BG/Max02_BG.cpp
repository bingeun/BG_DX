//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "Max02_BG.h"
#include "bgExporterMax.h"

#define Max02_BG_CLASS_ID	Class_ID(0x820560c3, 0x8f621d1d)


class Max02_BG : public UtilityObj 
{
public:
	bgExporterMax	m_Exporter;
	HWND			hPanel;
	IUtil			*iu;
	Interface		*ip;


	void SelectionSetChanged(Interface *ip, IUtil *iu);

public:

	//Constructor/Destructor
	Max02_BG();
	virtual ~Max02_BG();

	virtual void DeleteThis() { }
	
	virtual void BeginEditParams(Interface *ip,IUtil *iu);
	virtual void EndEditParams(Interface *ip,IUtil *iu);

	virtual void Init(HWND hWnd);
	virtual void Destroy(HWND hWnd);
	
	// Singleton access
	static Max02_BG* GetInstance() { 
		static Max02_BG theMax02_BG;
		return &theMax02_BG; 
	}

private:

	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};


class Max02_BGClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 	{ return Max02_BG::GetInstance(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return UTILITY_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return Max02_BG_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("BG_3D_UTIL"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetMax02_BGDesc() { 
	static Max02_BGClassDesc Max02_BGDesc;
	return &Max02_BGDesc; 
}




//--- Max02_BG -------------------------------------------------------
Max02_BG::Max02_BG()
	: hPanel(nullptr)
	, iu(nullptr)
{
	
}

Max02_BG::~Max02_BG()
{

}

void Max02_BG::BeginEditParams(Interface* ip,IUtil* iu) 
{
	this->iu = iu;
	this->ip = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		DlgProc,
		GetString(IDS_PARAMS),
		0);
}
	
void Max02_BG::EndEditParams(Interface* ip,IUtil*)
{
	this->iu = nullptr;
	this->ip = nullptr;
	ip->DeleteRollupPage(hPanel);
	hPanel = nullptr;
}

void Max02_BG::Init(HWND /*handle*/)
{
	if (I_Exporter.Initialize(ip) == false) return;

	if (I_Exporter.m_MatrixMap.Count() > 0)
	{
		EnableWindow(GetDlgItem(Max02_BG::GetInstance()->hPanel, IDC_BUTTON_SKIN), true);
		EnableWindow(GetDlgItem(Max02_BG::GetInstance()->hPanel, IDC_BUTTON_MATRIX), true);
		EnableWindow(GetDlgItem(Max02_BG::GetInstance()->hPanel, IDC_BUTTON_MODEL), true);
		EnableWindow(GetDlgItem(Max02_BG::GetInstance()->hPanel, IDC_BUTTON_UPDATE), false);
	}
	else
	{
		EnableWindow(GetDlgItem(Max02_BG::GetInstance()->hPanel, IDC_BUTTON_SKIN), false);
		EnableWindow(GetDlgItem(Max02_BG::GetInstance()->hPanel, IDC_BUTTON_MATRIX), false);
		EnableWindow(GetDlgItem(Max02_BG::GetInstance()->hPanel, IDC_BUTTON_MODEL), false);
		EnableWindow(GetDlgItem(Max02_BG::GetInstance()->hPanel, IDC_BUTTON_UPDATE), true);
	}
}

void Max02_BG::Destroy(HWND /*handle*/)
{
	I_Exporter.Release();
}

void Max02_BG::SelectionSetChanged(Interface* ip, IUtil* iu)
{
	if (ip->GetSelNodeCount() <= 0)
	{
		return;
	}
	if (I_Exporter.Initialize(ip) == false)
	{
		return;
	}
	//I_SkinExp.Release();

	for (int iSelectObj = 0; iSelectObj < ip->GetSelNodeCount(); iSelectObj++)
	{
		//I_SkinExp.GetNodeInfo(ip->GetSelNode(iSelectObj), ip->GetTime());
	}

	if (I_Exporter.m_MatrixMap.Count() > 0)
	{
		EnableWindow(GetDlgItem(Max02_BG::GetInstance()->hPanel, IDC_BUTTON_MATRIX), true);
		if (ip->GetSelNodeCount() > 0)
		{
			EnableWindow(GetDlgItem(Max02_BG::GetInstance()->hPanel, IDC_BUTTON_SKIN), true);
		}
		else
		{
			EnableWindow(GetDlgItem(Max02_BG::GetInstance()->hPanel, IDC_BUTTON_SKIN), false);
		}
	}
	else
	{
		EnableWindow(GetDlgItem(Max02_BG::GetInstance()->hPanel, IDC_BUTTON_MATRIX), false);
	}
}

INT_PTR CALLBACK Max02_BG::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
		case WM_INITDIALOG:
			Max02_BG::GetInstance()->Init(hWnd);
			break;

		case WM_DESTROY:
			Max02_BG::GetInstance()->Destroy(hWnd);
			break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON_UPDATE:
			{
				I_Exporter.UpdateObject();
			}
			break;

			case IDC_CHECK_BINDPOSE:
			{

			}
			break;

			// 포맷별 파일 생성 버튼들 =================================================
			case IDC_BUTTON_MODEL:		// Object Anim (*.BG3D)
			{
				I_Exporter.Initialize(Max02_BG::GetInstance()->ip);
				I_Exporter.ExportBG3D();
			}
			break;

			case IDC_BUTTON_SKIN:		// Skin (*.BGSKN)
			{
				I_Exporter.SetBindPose(true);
				I_Exporter.Initialize(Max02_BG::GetInstance()->ip);
				I_Exporter.ExportBGSKN();
			}
			break;

			case IDC_BUTTON_MATRIX:		// Matrix (*.BGMTX)
			{
				I_Exporter.Initialize(Max02_BG::GetInstance()->ip);
				I_Exporter.SetBindPose(true);
				I_Exporter.ExportBGMTX();
			}
			break;

			case IDC_BUTTON_ACTION:		// Action (*.BGACT)
			{
				I_Exporter.ExportBGACT();
			}
			break;

			// 액션 리스트 관리 =======================================================
			case IDC_LIST_ACTION:
			{

			}
			break;

			case IDC_EDIT_ACT_NAME:
			{

			}
			break;

			case IDC_BUTTON_ACT_ADD:
			{

			}
			break;

			case IDC_BUTTON_ACT_DEL:
			{

			}
			break;
			}
		}
		break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			GetCOREInterface()->RollupMouseMessage(hWnd,msg,wParam,lParam);
			break;

		default:
			return 0;
	}
	return 1;
}
