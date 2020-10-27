#if 0

#ifndef EXPORT_DIALOG_H
#define EXPORT_DIALOG_H

#include "strbasic.h"
#include "ImpExp.h"

class _3DSExport : public SceneExport 
{
	friend INT_PTR CALLBACK ExportOptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
					_3DSExport();
					~_3DSExport();
	int				ExtCount();					// Number of extensions supported
    BOOL            MaxUVs;                     // TRUE if generating extra verts for mismatched UV coords
	const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
	const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const TCHAR *	AuthorName();				// ASCII Author name
	const TCHAR *	CopyrightMessage();			// ASCII Copyright message
	const TCHAR *	OtherMessage1();			// Other message #1
	const TCHAR *	OtherMessage2();			// Other message #2
	unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
	void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box
	int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options);	// Export file
	BOOL			SupportsOptions(int ext, DWORD options);
};


class CExportDialog
{

	static CExportDialog* s_pCurrentExportDlg;


	bool	m_bFlipNormal;
	

	static INT_PTR CALLBACK CExportDialog::ExportOptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
	
public:
	CExportDialog();
	void	Display( HWND hParent );
	bool	FlipNormal() const;
};

#endif // EXPORT_DIALOG_H

#endif // 0