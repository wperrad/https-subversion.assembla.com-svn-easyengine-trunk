#pragma pack(1)

// System
#include <windows.h>
#include <shlobj.h>

// Program
#include "Browser.h"

//string CBrowser::s_sDefaultPath;

void CBrowser::SelectFile( HWND hOwner, string sTitle, string sMask, string sDefaultPath, string& sOutPath  )
{
	char szFileName[MAX_PATH] = "";
	strcpy_s( szFileName, sMask.c_str() );
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	ofn.hwndOwner = hOwner;
	ofn.lpstrDefExt = "";
	ofn.lpstrFile = szFileName;
	ofn.lpstrFilter = "*.exe";
	ofn.lpstrInitialDir = ".";
	ofn.lpstrTitle = sTitle.c_str();
	ofn.lStructSize = sizeof( OPENFILENAME );
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = sDefaultPath.c_str();
	ofn.lpstrFileTitle = "*.exe";
	ofn.lpTemplateName = "*.exe";
	if( GetOpenFileName(&ofn) )
		sOutPath = szFileName;
}

void CBrowser::SaveFile( HWND hOwner, string sTitle, string sMask, string sDefaultPath, string& sOutPath  )
{
	char szFileName[MAX_PATH] = "";
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.Flags = OFN_ENABLESIZING;
	ofn.hwndOwner = hOwner;
	ofn.lpstrFile = szFileName;
	ofn.lpstrFilter = "*.txt";
	ofn.lpstrInitialDir = sDefaultPath.c_str();
	ofn.lStructSize = sizeof( OPENFILENAME );
	ofn.lpstrTitle = sTitle.c_str();
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = "*.txt";
	ofn.lpTemplateName = "*.txt";
	if( GetSaveFileName( &ofn ) )
		sOutPath = szFileName;
}

void CBrowser::SelectFolder( HWND hOwner, string sTitle, string sDefaultFolder, string& sOutFolder )
{
	char pName[ MAX_PATH ] = "";
	BROWSEINFO bi;
	ZeroMemory( &bi, sizeof( BROWSEINFO ) );
	bi.lParam = (LPARAM)&sDefaultFolder;
	bi.hwndOwner = hOwner;
	bi.lpszTitle = sTitle.c_str();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = pName;
	bi.ulFlags = BIF_STATUSTEXT	;
	bi.lpfn = pBrowseCallback;
	ITEMIDLIST* pIl = SHBrowseForFolder( &bi );
	char pFolder[ MAX_PATH ];
	SHGetPathFromIDList( pIl, pFolder );
	sOutFolder = pFolder;
}

int CALLBACK CBrowser::pBrowseCallback( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	switch ( uMsg )
	{
	case BFFM_INITIALIZED:
		{
			string sDefaultPath = *((string*)lpData);
			SendMessage( hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)sDefaultPath.c_str() );
		}
		break;
	}
	return 0;
}