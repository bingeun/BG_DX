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

#include "bgMaxExporter.h"

#define Max01_Exporter100_CLASS_ID	Class_ID(0x6ad6710f, 0x80408113)

class Max01_Exporter100 : public SceneExport {
public:
	bgMaxExporter	m_BGMaxExporter;

	//Constructor/Destructor
	Max01_Exporter100();
	~Max01_Exporter100();

	int				ExtCount();					// Number of extensions supported
	const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
	const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const TCHAR *	AuthorName();				// ASCII Author name
	const TCHAR *	CopyrightMessage();			// ASCII Copyright message
	const TCHAR *	OtherMessage1();			// Other message #1
	const TCHAR *	OtherMessage2();			// Other message #2
	unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
	void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

	BOOL SupportsOptions(int ext, DWORD options);
	int  DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);
};



class Max01_Exporter100ClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new Max01_Exporter100(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return Max01_Exporter100_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("Max01_Exporter100"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetMax01_Exporter100Desc() { 
	static Max01_Exporter100ClassDesc Max01_Exporter100Desc;
	return &Max01_Exporter100Desc; 
}





INT_PTR CALLBACK Max01_Exporter100OptionsDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static Max01_Exporter100* imp = nullptr;

	switch(message) {
		case WM_INITDIALOG:
			imp = (Max01_Exporter100 *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_EXPORT:
			{
				imp->m_BGMaxExporter.Exporter();
				EndDialog(hWnd, 1);
			}
			break;
			}
		}
		break;
		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}


//--- Max01_Exporter100 -------------------------------------------------------
Max01_Exporter100::Max01_Exporter100()
{

}

Max01_Exporter100::~Max01_Exporter100() 
{

}

int Max01_Exporter100::ExtCount()
{
	#pragma message(TODO("Returns the number of file name extensions supported by the plug-in."))
	return 1;
}

const TCHAR *Max01_Exporter100::Ext(int /*i*/)
{		
	#pragma message(TODO("Return the 'i-th' file name extension (i.e. \"3DS\")."))
	return _T("bg3d");
}

const TCHAR *Max01_Exporter100::LongDesc()
{
	#pragma message(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\")"))
	return _T("KGCA BG 3D Model Export Format.");
}
	
const TCHAR *Max01_Exporter100::ShortDesc() 
{			
	#pragma message(TODO("Return short ASCII description (i.e. \"Targa\")"))
	return _T("KGCA BG 3D Model");
}

const TCHAR *Max01_Exporter100::AuthorName()
{			
	#pragma message(TODO("Return ASCII Author name"))
	return _T("BG");
}

const TCHAR *Max01_Exporter100::CopyrightMessage() 
{	
	#pragma message(TODO("Return ASCII Copyright message"))
	return _T("");
}

const TCHAR *Max01_Exporter100::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *Max01_Exporter100::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int Max01_Exporter100::Version()
{				
	#pragma message(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 100;
}

void Max01_Exporter100::ShowAbout(HWND /*hWnd*/)
{			
	// Optional
}

BOOL Max01_Exporter100::SupportsOptions(int /*ext*/, DWORD /*options*/)
{
	#pragma message(TODO("Decide which options to support.  Simply return true for each option supported by each Extension the exporter supports."))
	return TRUE;
}


int	Max01_Exporter100::DoExport(const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts, DWORD options)
{
	m_BGMaxExporter.Init(name, ip);

	if(!suppressPrompts)
		DialogBoxParam(hInstance, 
				MAKEINTRESOURCE(IDD_PANEL), 
				GetActiveWindow(), 
				Max01_Exporter100OptionsDlgProc, (LPARAM)this);

	m_BGMaxExporter.Release();
	return TRUE;
}
