// System
#include <windows.h>
#include <gl/gl.h>

#define _USE_MATH_DEFINES
#include <math.h>
// stl
#include <exception>
#include <sstream>

#include "../WindowsGUI/Window2.h"
#include "../WindowsGUI/TabControl.h"
#include "OGLAPI.h"
#include "../WindowsGUI/Menu2.h"
#include "Convertissor.h"
#include "../math/matrix.h"
#include "../math/Quaternion.h"
#include "../WindowsGUI/TreeView.h"

using namespace std;


class CTestWindow : public CWindow2
{
public:
	CTestWindow( const CWindow2::Desc& desc ) : CWindow2( desc ){}
	void CallCallback();
};

//void Test1()
//{
//	CDatabase2 db( "DEVBNP2" );
//	db.Connect( "roubnp2", "roubnp200" );
//	db.Execute( "insert into ALERTE(ADREMAIL, CODCENTRE, TYPALERTE, NIVALERTE, LOGINMODIF, DATMODIF ) values( 'aiolia_aiolos@yahoo.fr','08868', 5, 127, NULL, NULL )" );
//	db.Disconnect();
//	db.Execute( "select * from ALERTE" );
//	int nRowCount = db.GetRowCount();
//}


void OnPaint()
{
	//glBegin( GL_TRIANGLES );
	//glVertex3d( 0, 0, -2 );
	//glVertex3d( 0, 1, -2 );
	//glVertex3d( 1, 0, -2 );
	//glEnd();
}

void OnUpdate()
{
	glBegin( GL_TRIANGLES );
	glVertex3d( 0, 0, -2 );
	glVertex3d( 0, 1, -2 );
	glVertex3d( 1, 0, -2 );
	glEnd();
}

void TestOGLWindow()
{
	COGLAPI ogl;
	COGLAPI::GLPARAM param;
	param.OnPaint = OnPaint;
	param.m_bFullscreen = true;
	param.m_nBits = 32;
	param.m_nWidth = 1280;
	param.m_nHeight = 1024;
	param.OnUpdate = OnUpdate;
	int hWindow = ogl.CreateOGLWindow( param );
	ogl.Display( hWindow );
}

void TestTabControl()
{
	CWindow2::Desc desc( 800, 500, "fenetre", NULL );
	CTestWindow tw( desc );

	CTabControl::Desc tcDesc( 700, 200, &tw );
	tcDesc.m_nPosX = 20;
	tcDesc.m_nPosY = 20;
	CTabControl tc( tcDesc );	

	int nItemCount = 10;
	for ( int i = 0; i < nItemCount; i++ )
	{
		ostringstream ssOngletName;
		ssOngletName << "Onglet " << i;
		tc.InsertItem( ssOngletName.str() );
		
		if ( tc.GetRowCount() != i+1 )
		{
			ostringstream ssErrorMessage;
			ssErrorMessage << "Erreur dans TabControl::GetRowCount, " << i+1 
							<< " colonnes attendues, " << tc.GetRowCount() << " obtenues";
			exception e( ssErrorMessage.str().c_str() );
			throw e;
		}
	}
	
	tc.SelectItem( 1 );
	for ( int i = 0; i < nItemCount; i++ )
	{
		ostringstream ssOngletName;
		ssOngletName << "Onglet " << i;
		int iIndex = tc.GetItemIndex( ssOngletName.str() );
		if ( iIndex != i )
		{
			exception e( "Erreur dans TabControl::SelectItem" );
			throw e;
		}
	}

	for ( int i = 0; i < nItemCount; i++ )
	{
		string sItemName;
		tc.GetItemName( i, sItemName );
		ostringstream ssItemName;
		ssItemName << "Onglet " << i;
		if ( sItemName != ssItemName.str() )
		{
			exception e( "Erreur dans TabControl::GetItemName" );
			throw e;
		}
	}

	for ( int i = 0; i < nItemCount; i++ )
	{
		tc.SelectItem( i );
		int iIndex = tc.GetSelectedItemIndex();
		if ( i != iIndex )
		{
			exception e( "Erreur dans TabControl::GetSelectedItemIndex" );
			throw e;
		}
		string sItemName;
		tc.GetSelectedItemName( sItemName );
		ostringstream ssItemName;
		ssItemName << "Onglet " << i;
		if ( sItemName != ssItemName.str() )
		{
			exception e( "Erreur dans TabControl::GetSelectedItemName" );
			throw e;
		}
	}
	tw.ShowModal();
}

map< int, string > g_mMenuIDName;
CWindow2* g_pWindow = NULL;

void OnMenuSelect( int nID )
{
	string sMenuName = g_mMenuIDName[ nID ];
	if ( sMenuName == "Quitter" )
		g_pWindow->Close();

}

void TestMenuWindow()
{
	CMenu2* pMenu = new CMenu2( "Main" );
	CMenu2* pFile = new CMenu2( "Fichier" );
	int nID = pFile->AddString( "Ouvrir" );
	g_mMenuIDName[ nID ] = "Ouvrir";
	nID = pFile->AddString( "Enregistrer" );
	g_mMenuIDName[ nID ] = "Enregistrer";
	nID = pFile->AddString( "Quitter" );
	g_mMenuIDName[ nID ] = "Quitter";
	pMenu->AddMenu( pFile );
	pMenu->SetCallback( OnMenuSelect );

	CWindow2::Desc desc( 100, 200, "wlad", NULL );
	desc.m_pMenu = pMenu;
	CWindow2 w( desc );
	g_pWindow = &w;
	w.ShowModal();
}


LRESULT WindowCallback( CWidget*, UINT, WPARAM, LPARAM )
{
	return 0;
}

#define TEST2

#ifdef TEST1

LRESULT TestWinCallback( CWidget*, UINT, WPARAM, LPARAM );

void OnTreeAction( int nItemID, TTreeEvent e );

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	CWindow2::Desc owDesc( 600, 300, "prout", TestWinCallback );
	owDesc.m_bCentered = true;
	CWindow2 w( owDesc );

	int nWidth, nHeight;
	w.GetDimension( nWidth, nHeight );
	CWidget::Desc oDesc( nWidth, nHeight, &w );
	CTreeView oTV( oDesc );
	oTV.SetCallback( OnTreeAction );
	int nID = oTV.AddItem( "Bonjour" );
	oTV.AddItem( "Comment allez vous ?", nID );
	oTV.AddItem( "Que voulez-vous ?", nID );
	nID = oTV.AddItem( "Au revoir" );
	oTV.AddItem( "Et à une prochaine", nID );
	oTV.AddItem( "Et que je ne vous revois jamais", nID );
	nID = oTV.AddItem( "En garde !" );
	oTV.AddItem( "Marin d'eau douce !", nID );
	int nID2 = oTV.AddItem( "Tu va tâter de mon fleuret !", nID );
	oTV.AddItem( "Shlack !", nID2 );
	nID2 = oTV.AddItem( "Tu va tâter de mon mousquet !", nID );
	oTV.AddItem( "Pan !", nID2 );
	w.ShowModal();
	return TRUE;
}

void OnTreeAction( int nItemID, TTreeEvent e )
{
	if ( e == eSelChanged )
	{
		ostringstream oss;
		oss << "Vous avez sélectionné l'option " << nItemID;
		MessageBox( NULL, oss.str().c_str(), "", MB_OK );
	}
}

LRESULT TestWinCallback( CWidget* pWidget, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return 0;
}
#endif //  TEST1


#ifdef TEST3

#ifdef STRICT
  WNDPROC wpOrigEditProc ;
#else
  FARPROC wpOrigEditProc ;
#endif

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT APIENTRY EditProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hinst;

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance,
                                                  LPSTR lpCmdLine, int nCmdShow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASS wc;

    hinst = hinstance;

    wc.style = 0 ;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hinstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "MaWinClass";

    if(!RegisterClass(&wc)) return FALSE;

    hwnd = CreateWindow("MaWinClass", "prout", WS_OVERLAPPEDWINDOW, 0, 0, 600, 300, NULL, NULL, hinstance, NULL);
    if (!hwnd)  return FALSE;

    ShowWindow(hwnd, nCmdShow);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

/******************************************************************************/

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HWND hTreeView;

    switch (uMsg)
    {
        case WM_CREATE:
           {
            TV_INSERTSTRUCT tviis;
            TV_ITEM tvi;
            HTREEITEM hitem;

            //HINSTANCE dllhinst;
            InitCommonControls();
            
            hTreeView = CreateWindowEx(WS_EX_CLIENTEDGE , WC_TREEVIEW, "", WS_CHILD | WS_VISIBLE  | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS , 0, 0, 0, 0, hwnd, NULL, hinst, NULL );

            tviis.hInsertAfter = TVI_LAST;
            ZeroMemory(&(tviis.item), sizeof(TV_ITEM));
            tviis.item.mask  = TVIF_TEXT | TVIF_IMAGE  | TVIF_SELECTEDIMAGE | TVIF_PARAM ;

            tviis.hParent = TVI_ROOT;
            tviis.item.lParam = 1;
            tviis.item.pszText = "Option 1";
            hitem = TreeView_InsertItem(hTreeView, &tviis);

            tviis.hParent = hitem ;
            tviis.item.lParam = 3;
            tviis.item.pszText = "Sous Option A";
            TreeView_InsertItem(hTreeView, &tviis);

            tviis.item.lParam = 4;
            tviis.item.pszText = "Sous Option B";
            TreeView_InsertItem(hTreeView, &tviis);

            tviis.hParent = TVI_ROOT;
            tviis.item.lParam = 2;
            tviis.item.pszText = "Option 2";
            hitem = TreeView_InsertItem(hTreeView, &tviis);

            tviis.hParent = hitem ;
            tviis.item.lParam = 5;
            tviis.item.pszText = "Sous Option C";
            TreeView_InsertItem(hTreeView, &tviis);

            tviis.item.lParam = 6;
            tviis.item.pszText = "Sous Option D";
            TreeView_InsertItem(hTreeView, &tviis);

            return 0;
           }

        case WM_NOTIFY:
           {
             HTREEITEM hitem;

             LPNM_TREEVIEW pntv = (LPNM_TREEVIEW)lParam;

             if(pntv->hdr.code == TVN_SELCHANGED)
              {
				  ostringstream oss;
				  oss << "Vous avez sélectionné l'option " << pntv->itemNew.lParam;
				  MessageBox( NULL, oss.str().c_str(), "", MB_OK );
              }
             return 0;
           }
        case WM_SIZE:
            MoveWindow(hTreeView, 0, 0, 600, 300, TRUE);
			break;
            //return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

/******************************************************************************/

LRESULT APIENTRY EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (uMsg == WM_SETFOCUS || uMsg == WM_CONTEXTMENU || uMsg == WM_CHAR ||
                                                             uMsg == WM_KEYDOWN)
                                                                       return 0;
   return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam);
}

#endif // TEST3



#ifdef TEST2

#ifdef STRICT
  WNDPROC wpOrigEditProc ;
#else
  FARPROC wpOrigEditProc ;
#endif

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT APIENTRY EditProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hinst;

void TestQuaternion()
{
	//CQuaternion q2, q2b;
	//CMatrix m( -2.3841858e-007f,		-1.0000000f,		5.8091246e-008f,	0.00000000f,
	//			-0.99999934f,			1.1920929e-007f,	-0.0011870498f,		-1.0773603e-007f,
	//			0.0011870499f,			-5.8265869e-008f,	-0.99999940f,		2.4647131f,
	//				0.f,						0.f,				0.f,			1.f );

	//m.GetQuaternion( q2 );
	//CMatrix mb;
	//q2.GetMatrix( mb );

	//CMatrix m2( -1.1920929e-007f,	-0.99999994f,			5.9604645e-008f,	0.00000000f	, 
	//			-0.99930769f,		2.3841858e-007f,		0.037203975f,		-1.0773603e-007f,
	//			-0.037203975f,		-5.4016709e-008f,		-0.99930763f,		2.4647131f,	
	//				0.f,				0.f,						0.f,			1.f  );
	//
	//m2.GetQuaternion( q2b );
	//CMatrix m2b;
	//q2b.GetMatrix( m2b );

	CQuaternion q1( 0.5 , -0.5 , 0.5 , 0.5 );
	CQuaternion q2( 0.706515 , -0.706515 , 0.0284994 , 0.028501 );
	CQuaternion q2p( -0.706984 , 0.706984 , 0.0131547 , 0.0131593 );

	CVector vAxis1, vAxis2, vAxis2p, vAxisr;
	float fAngle1, fAngle2, fAngle2p, fAngler;
	q1.GetAngleAxis( vAxis1, fAngle1 );
	q2.GetAngleAxis( vAxis2, fAngle2 );
	q2p.GetAngleAxis( vAxis2p, fAngle2p );

	CQuaternion q1Inverse;
	q1.GetConjugate( q1Inverse );
	q1Inverse.Normalize();
	CQuaternion qr = q1 * q2;
	qr.Normalize();
	qr.GetAngleAxis( vAxisr, fAngler );

	qr = q1 * q2 * q1Inverse;
	qr.Normalize();
	qr.GetAngleAxis( vAxisr, fAngler );
}

#include "CsvReader.h"

class IFighter
{
public:
	virtual void f() = 0;
};

class CMobile : public virtual IFighter
{
public:
	void f(){ MessageBoxA( NULL, "f()", "", MB_OK ); }
};

class IAEntity : public virtual IFighter
{
public:
	virtual void g() = 0;
};

class CNPC : public CMobile, public IAEntity
{
public:
	void f()
	{ 
		CMobile::f();
	}

	void g()
	{
		MessageBox( NULL, "g()", "", MB_OK );
	}
};

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance,
                                                  LPSTR lpCmdLine, int nCmdShow)
{
	CNPC* pNPC = new CNPC;
	IFighter* pFighter = dynamic_cast< IFighter* >( pNPC );
	pFighter->f();

	/*
	IBase* pBase = pDerived;
	IBase1* pBase1 = dynamic_cast< IBase* >( pBase );
	d.f();
	d.g();
*/
	
	return TRUE;
}


/******************************************************************************/

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HWND hEdit;
    static HWND hTreeView;

    switch (uMsg)
    {
        case WM_CREATE:
           {
            TV_INSERTSTRUCT tviis;
            TV_ITEM tvi;
            HTREEITEM hitem;

            HINSTANCE dllhinst;
            HICON hIcon;
            HIMAGELIST himgList;
            InitCommonControls();
            himgList = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
                             GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 , 4, 4);
            ImageList_SetBkColor(himgList, GetSysColor(COLOR_WINDOW));
            dllhinst = LoadLibrary("shell32.dll");
            hIcon = LoadIcon(dllhinst, MAKEINTRESOURCE(4));
            ImageList_AddIcon(himgList, hIcon);
            hIcon = LoadIcon(dllhinst, MAKEINTRESOURCE(5));
            ImageList_AddIcon(himgList, hIcon);
            hIcon = LoadIcon(dllhinst, MAKEINTRESOURCE(6));
            ImageList_AddIcon(himgList, hIcon);
            hIcon = LoadIcon(dllhinst, MAKEINTRESOURCE(12));
            ImageList_AddIcon(himgList, hIcon);
            FreeLibrary(dllhinst);

            hTreeView =CreateWindowEx(WS_EX_CLIENTEDGE , WC_TREEVIEW, "",
                   WS_CHILD | WS_VISIBLE  | TVS_HASLINES | TVS_LINESATROOT
                                        | TVS_HASBUTTONS | TVS_SHOWSELALWAYS
                                         , 0, 0, 0, 0, hwnd, NULL, hinst, NULL);

            hEdit =CreateWindowEx(WS_EX_CLIENTEDGE , "edit",
                                          "\r\nTest de la ListView.",
              WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL,
                                           0, 0, 0, 0, hwnd, NULL, hinst, NULL);

            SetClassLong(hEdit, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_ARROW));
            wpOrigEditProc = (WNDPROC)SetWindowLong(hEdit,
                                                   GWL_WNDPROC, (LONG)EditProc);

            TreeView_SetImageList(hTreeView, himgList, TVSIL_NORMAL);

            tviis.hInsertAfter = TVI_LAST;
            ZeroMemory(&(tviis.item), sizeof(TV_ITEM));
            tviis.item.mask  = TVIF_TEXT | TVIF_IMAGE  |
                                               TVIF_SELECTEDIMAGE | TVIF_PARAM ;

            tviis.hParent = TVI_ROOT;
            tviis.item.iImage = 0;
            tviis.item.iSelectedImage = 1;
            tviis.item.lParam = 1;
            tviis.item.pszText = "Option 1";
            hitem = TreeView_InsertItem(hTreeView, &tviis);

            tviis.hParent = hitem ;
            tviis.item.iImage = 2;
            tviis.item.iSelectedImage = 2;
            tviis.item.lParam = 3;
            tviis.item.pszText = "Sous Option A";
            TreeView_InsertItem(hTreeView, &tviis);

            tviis.item.iImage = 3;
            tviis.item.iSelectedImage = 3;
            tviis.item.lParam = 4;
            tviis.item.pszText = "Sous Option B";
            TreeView_InsertItem(hTreeView, &tviis);

            tviis.hParent = TVI_ROOT;
            tviis.item.iImage = 0;
            tviis.item.iSelectedImage = 1;
            tviis.item.lParam = 2;
            tviis.item.pszText = "Option 2";
            hitem = TreeView_InsertItem(hTreeView, &tviis);

            tviis.hParent = hitem ;
            tviis.item.iImage = 3;
            tviis.item.iSelectedImage = 3;
            tviis.item.lParam = 5;
            tviis.item.pszText = "Sous Option C";
            TreeView_InsertItem(hTreeView, &tviis);

            tviis.item.iSelectedImage = 2;
            tviis.item.iImage = 2;
            tviis.item.lParam = 6;
            tviis.item.pszText = "Sous Option D";
            TreeView_InsertItem(hTreeView, &tviis);

            return 0;
           }

        case WM_NOTIFY:
           {
             HTREEITEM hitem;

             LPNM_TREEVIEW pntv = (LPNM_TREEVIEW)lParam;

             if(pntv->hdr.code == TVN_SELCHANGED)
              {
               if(pntv->itemNew.lParam == 1)
               SetWindowText(hEdit,
                               "\r\n\r\n\r\nVous avez sélectionné l'option 1.");
               if(pntv->itemNew.lParam == 2)
               SetWindowText(hEdit,
                               "\r\n\r\n\r\nVous avez sélectionné l'option 2.");
               if(pntv->itemNew.lParam == 3)
               SetWindowText(hEdit,
                         "\r\n\r\n\r\nVous avez sélectionné la sous Option A.");
               if(pntv->itemNew.lParam == 4)
               SetWindowText(hEdit,
                         "\r\n\r\n\r\nVous avez sélectionné la sous Option B.");
               if(pntv->itemNew.lParam == 5)
               SetWindowText(hEdit,
                         "\r\n\r\n\r\nVous avez sélectionné la sous Option C.");
               if(pntv->itemNew.lParam == 6)
               SetWindowText(hEdit,
                         "\r\n\r\n\r\nVous avez sélectionné la sous Option D.");
              }
             return 0;
           }
        case WM_SIZE:
            MoveWindow(hEdit, 200, 0, LOWORD(lParam)-200, HIWORD(lParam), TRUE);
            MoveWindow(hTreeView, 0, 0, 200, HIWORD(lParam), TRUE);
            return 0;

        case WM_DESTROY:
            SetWindowLong(hEdit, GWL_WNDPROC,(LONG) wpOrigEditProc);
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

/******************************************************************************/

LRESULT APIENTRY EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (uMsg == WM_SETFOCUS || uMsg == WM_CONTEXTMENU || uMsg == WM_CHAR ||
                                                             uMsg == WM_KEYDOWN)
                                                                       return 0;
   return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam);
}

#endif // TEST2