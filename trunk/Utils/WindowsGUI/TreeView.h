#ifndef TREEVIEW_H
#define TREEVIEW_H

#include "Widget.h"
#include <map>

class CWindow2;


enum TTreeEvent
{
	eSelChanging		= TVN_SELCHANGING,
	eSelChanged			= TVN_SELCHANGED,
	eGetDispInfo		= TVN_GETDISPINFO,
	eSetDispInfo		= TVN_SETDISPINFO,
	eItemExpanding		= TVN_ITEMEXPANDING,
	eItemExpanded		= TVN_ITEMEXPANDED,
	eBeginDrag			= TVN_BEGINDRAG,
	eBeginRDrag			= TVN_BEGINRDRAG,
	eDeleteItem			= TVN_DELETEITEM,
	eBeginLabelEdit 	= TVN_BEGINLABELEDIT,
	eEndLabelEdit		= TVN_ENDLABELEDIT
};

typedef void(*TREEVIEWCALLBACK)( int, TTreeEvent );


class CTreeView : public CWidget
{
private:
	
	void							CallCallback( CallbackArgs& args );
	std::map< int, HTREEITEM >		m_mTreeItem;
	static TREEVIEWCALLBACK			s_pfnCallback;

	std::map< int, HTREEITEM >		m_mIDToHTREEITEM;

	

public:


	CTreeView( Desc& oDesc );
	~CTreeView();
	int				AddItem( std::string sText, int nParentID = -1 );
	void			SetCallback( TREEVIEWCALLBACK );
	HTREEITEM		GetHTreeItemByID( int nItemID );


	HFONT hTestFont;
};

#endif // TREEVIEW_H