#include <windows.h>
#include <commctrl.h>

#include "TreeView.h"
#include "Window2.h"

using namespace std;

TREEVIEWCALLBACK CTreeView::s_pfnCallback = NULL;

CTreeView::CTreeView( Desc& oDesc ):
CWidget( oDesc )
{
    HTREEITEM hitem;
    InitCommonControls();
	CreateWnd( WS_EX_CLIENTEDGE , WC_TREEVIEW, "", WS_CHILD | WS_VISIBLE  | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS , 0, 0, oDesc.m_nWidth, oDesc.m_nHeight, NULL );


	string sFontName = "Lucida Console";
	LOGFONTA lf;
	ZeroMemory( &lf, sizeof( LOGFONTA ) );
	lf.lfHeight = -11;
	lf.lfWeight = 400;
	lf.lfOutPrecision = 1;
	lf.lfClipPrecision = 2;
	lf.lfQuality = 1;
	lf.lfPitchAndFamily = 34;
	strcpy( lf.lfFaceName, sFontName.c_str() );
	hTestFont = CreateFontIndirect( &lf );
}

CTreeView::~CTreeView()
{
	for ( int i = 0; i < m_pParent->GetChildCount(); i++ )
		if ( m_pParent->GetChild( i ) == this )
			m_pParent->UnlinkChild( i );
}

int CTreeView::AddItem( string sText, int nParentID )
{
	TV_INSERTSTRUCT oInsert;
	oInsert.hInsertAfter = TVI_LAST;
	ZeroMemory(&(oInsert.item), sizeof(TV_ITEM));
	oInsert.item.mask  = TVIF_TEXT | TVIF_IMAGE  | TVIF_SELECTEDIMAGE | TVIF_PARAM ;

	if ( nParentID == -1 )
		oInsert.hParent = TVI_ROOT;
	else
		oInsert.hParent = m_mTreeItem[ nParentID ];
	
	int nID = m_mTreeItem.size() + 1;
	oInsert.item.lParam = nID;
	
	oInsert.item.pszText = new char[ sText.size() + 1 ];
	strcpy( oInsert.item.pszText, sText.c_str() );
	HTREEITEM hitem = TreeView_InsertItem( GetHandle(), &oInsert );
	m_mIDToHTREEITEM[ nID ] = hitem;
	delete oInsert.item.pszText;
	m_mTreeItem[ oInsert.item.lParam  ] = hitem;
	return nID;
}

HTREEITEM CTreeView::GetHTreeItemByID( int nItemID )
{
	return m_mIDToHTREEITEM[ nItemID ];
}

void CTreeView::CallCallback( CallbackArgs& args )
{
	if ( args.m_msg == WM_NOTIFY )
    {
		HTREEITEM hitem;
		LPNM_TREEVIEW pntv = (LPNM_TREEVIEW)args.m_lParam;
		if ( s_pfnCallback )
		{
			if( pntv->hdr.code == TVN_SELCHANGED )
				s_pfnCallback( pntv->itemNew.lParam, (TTreeEvent)pntv->hdr.code );
		}
    }
}

void CTreeView::SetCallback( TREEVIEWCALLBACK pfnCallback )
{
	s_pfnCallback = pfnCallback;
}