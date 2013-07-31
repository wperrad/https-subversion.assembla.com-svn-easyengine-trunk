#ifndef TAB_CONTROL_H
#define TAB_CONTROL_H

#include <windows.h>
#include "Control.h"

class CTabControl : public CControl
{
public:
	struct Desc : public CControl::Desc
	{
		Desc( int nWidth, int nHeight, CWidget* pParent );
	};
				CTabControl( CControl::Desc& desc );
	virtual		~CTabControl(void);	


	void		InsertItem( std::string sItemName );
	void		SelectItem( std::string sItemName );
	void		SelectItem( int nIndex );
	int			GetItemIndex( std::string sItemName )const;
	int			GetRowCount()const;
	void		GetItemName( int nIndex, std::string& sItemName )const;
	int			GetSelectedItemIndex()const;
	void		GetSelectedItemName( std::string& sItemName )const;
	void		CallCallback( CallbackArgs& args );
				
};

#endif // TAB_CONTROL_H