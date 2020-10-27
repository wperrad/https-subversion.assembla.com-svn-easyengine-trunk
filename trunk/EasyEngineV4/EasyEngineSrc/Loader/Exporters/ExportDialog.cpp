#if 0

#include <windows.h>
#include "Resource.h"

#include "ExportDialog.h"
#include "MaxExporter.h"

// Autodesk
#include "Max.h"
#include "3dsmaxport.h"
#include "IGame.h"
#include "IGameModifier.h"
#include "Contextids.h"
#include "helpSys.h"


CExportDialog* CExportDialog::s_pCurrentExportDlg = NULL;	

//
//class _3DSClassDesc : public ClassDesc 
//{
//public:
//	int 			IsPublic() 
//	{ 
//		return 1; 
//	}
//
//	void *			Create(BOOL loading = FALSE) 
//	{ 
//		return new _3DSExport; 
//	}
//
//	const TCHAR *	ClassName() 
//	{ 
//		return _T("_3DSClassDesc"); 
//	}
//	SClass_ID		SuperClassID() 
//	{ 
//		return SCENE_EXPORT_CLASS_ID; 
//	}
//	Class_ID		ClassID() 
//	{ 
//		return EASYENGINEEXP_CLASS_ID; 
//	}
//
//	const TCHAR* 	Category() 
//	{ 
//		return "Binary mesh export";
//	}
//};
//
//static _3DSClassDesc _3DSDesc;

//__declspec( dllexport ) const TCHAR * LibDescription() 
//{ 
//	return  _T( "Lib description" ); 
//}

//__declspec( dllexport ) int LibNumberClasses() 
//{ 
//	return 1; 
//}

//__declspec( dllexport ) ClassDesc * LibClassDesc(int i) 
//{
//	switch(i) 
//	{
//		case 0:  return &_3DSDesc; 
//		default: return 0; break;
//	}
//}








_3DSExport::_3DSExport() {
	}

_3DSExport::~_3DSExport() {
	}

int
_3DSExport::ExtCount() {
	return 1;
	}

const TCHAR * _3DSExport::Ext(int n) 
{		// Extensions supported for import/export modules
	switch(n) 
	{
		case 0:
			return _T("BME");
		}
	return _T("");
}

const TCHAR * _3DSExport::LongDesc() 
{			// Long ASCII description (i.e. "Targa 2.0 Image File")
	return "";
}
	
const TCHAR *
_3DSExport::ShortDesc() 
{			// Short ASCII description (i.e. "Targa")
	return "";
}

const TCHAR *
_3DSExport::AuthorName() 
{			// ASCII Author name
	return "Wladimir Perrad";
}

const TCHAR * _3DSExport::CopyrightMessage() 
{	// ASCII Copyright message
	return "";
}

const TCHAR * _3DSExport::OtherMessage1() {		// Other message #1
	return _T("");
}

const TCHAR * _3DSExport::OtherMessage2() 
{		// Other message #2
	return _T("");
}

unsigned int _3DSExport::Version() 
{				// Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void _3DSExport::ShowAbout(HWND hWnd) 
{			// Optional
}

int _3DSExport::DoExport(const TCHAR *filename,ExpInterface *ei,Interface *gi, BOOL suppressPrompts, DWORD options) 
{
	return 0;

#if 0
	// Set a global prompt display switch
	bool showPrompts = suppressPrompts ? FALSE : TRUE;
	bool exportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;

	int status;

    msgList.SetCount(0);

	status = _3ds_save(filename, ei, gi, this);

 // gdf output errors
    if (msgList.Count()) {
		if(showPrompts)
			CreateDialog(hInstance, MAKEINTRESOURCE(IDD_ERROR_MSGS), GetActiveWindow(), MsgListDlgProc);
        for (int i = 0; i < msgList.Count(); i++) {
            delete msgList[i];
            msgList[i] = NULL;
        }
        msgList.Delete(0, msgList.Count());
    }

	if(status == 0)
		return 1;		// Dialog cancelled
	if(status < 0)
		return 0;		// Real, honest-to-goodness error
	return(status);
#endif // 0
}

BOOL _3DSExport::SupportsOptions(int ext, DWORD options) 
{
	assert(ext == 0);	// We only support one extension
	return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}


CExportDialog::CExportDialog()
{
	s_pCurrentExportDlg = this;
}

void CExportDialog::Display( HWND hParent )
{
	DialogBoxParamA( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOGEXPORTEE), hParent, ExportOptionsDlgProc, 0L );
}
	
bool CExportDialog::FlipNormal() const
{
	return m_bFlipNormal;
}


INT_PTR CALLBACK CExportDialog::ExportOptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	static _3DSExport* exp;

	switch(message) {
		case WM_INITDIALOG:
			SetWindowContextHelpId(hDlg, idh_3dsexp_export);
            exp = (_3DSExport*) lParam;
			CenterWindow(hDlg,GetParent(hDlg));
			SetFocus(hDlg); // For some reason this was necessary.  DS-3/4/96
			CheckDlgButton(hDlg, IDC_FLIPNORMALS, FALSE);
			return FALSE;
		case WM_DESTROY:
			return FALSE;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDOK:
					CExportDialog::s_pCurrentExportDlg->m_bFlipNormal = ( bool )( IsDlgButtonChecked(hDlg, IDC_FLIPNORMALS) );
					EndDialog(hDlg, 1);
					return TRUE;
				case IDCANCEL:
					EndDialog(hDlg, 0);
					return TRUE;
			}
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_CONTEXTHELP) 
			{
				//DoHelp(HELP_CONTEXT, idh_3dsexp_export);
				return FALSE;
			}
		}
	return FALSE;
}


#endif // 0