#include "TabControl.h"
#include <commctrl.h>

using namespace std;


CTabControl::Desc::Desc( int nWidth, int nHeight, CWidget* pParent ) : 
CControl::Desc( nWidth, nHeight, pParent )
{
}

CTabControl::CTabControl( CControl::Desc& desc ) : 
CControl( desc )
{
	DWORD dwStyle = WS_VISIBLE | WS_CHILD;
//	m_hWnd = CreateWindowExA( 0, WC_TABCONTROLA, "", dwStyle, desc.m_nPosX, desc.m_nPosY, desc.m_nWidth, desc.m_nHeight,
//								desc.m_pParent->GetHandle(), 0, GetModuleHandle(0), 0 );
	CreateWnd( 0, WC_TABCONTROLA, "", dwStyle, desc.m_nPosX, desc.m_nPosY, desc.m_nWidth, desc.m_nHeight, NULL );
}

CTabControl::~CTabControl(void)
{
}

void CTabControl::InsertItem( string sItemName )
{
	// On insère un onglet
	TCITEMA tcItem;
	tcItem.mask = TCIF_TEXT | TCIF_PARAM;
	tcItem.pszText = (char*)sItemName.c_str();
	tcItem.cchTextMax = (int)sItemName.size() + 1;
	LRESULT lResult = SendMessageA( GetHandle(), TCM_INSERTITEMA, GetRowCount(), (LPARAM)&tcItem );
}

void CTabControl::SelectItem( std::string sItemName )
{
	int iIndex = GetItemIndex( sItemName );
	LRESULT lResult = SendMessageA( GetHandle(), TCM_SETCURSEL, iIndex, 0);
}

void CTabControl::SelectItem( int nIndex )
{
	int nRowCount = GetRowCount();
	if( nIndex >= nRowCount || nIndex < 0 )
	{
		exception e( "TabControl index out of bound" );
		throw e;
	}
	LRESULT lResult = SendMessageA( GetHandle(), TCM_SETCURSEL, nIndex, 0);
}

int CTabControl::GetItemIndex( string sItemName )const
{
	string sBuffer;		
	LRESULT lRowCount = SendMessageA( GetHandle(), TCM_GETITEMCOUNT, 0, 0 );
	int iIndex = 0;
	do
	{
		GetItemName( iIndex, sBuffer );
		iIndex++;
	}
	while( sBuffer != sItemName && iIndex < lRowCount );
	return iIndex - 1;
}

int CTabControl::GetRowCount()const
{
	return (int)SendMessageA( GetHandle(), TCM_GETITEMCOUNT, 0, 0 );
}

void CTabControl::GetItemName( int nIndex, string& sItemName )const
{
	TCITEMA item;
	char bufLogin[ 256 ] = {0};
	ZeroMemory( &item, sizeof(item) );
	item.mask = TCIF_TEXT;
	item.cchTextMax=sizeof(bufLogin);
	item.pszText = bufLogin;
	SendMessageA( GetHandle(), TCM_GETITEMA ,nIndex, (LONG)&item );
	sItemName = bufLogin;
}

int CTabControl::GetSelectedItemIndex()const
{
	return (int)SendMessageA( GetHandle(), TCM_GETCURSEL, 0, 0 );
}

void CTabControl::GetSelectedItemName( std::string& sItemName )const
{
	LRESULT lTabIdx = SendMessageA( GetHandle(), TCM_GETCURSEL, 0, 0 );
	TCITEMA item;
	char bufLogin[ 256 ] = {0};
	ZeroMemory( &item, sizeof(item) );
	item.mask = TCIF_TEXT;
	item.cchTextMax=sizeof(bufLogin);
	item.pszText = bufLogin;
	SendMessageA( GetHandle(),TCM_GETITEMA ,lTabIdx, (LONG)&item );
	sItemName = bufLogin;
}

void CTabControl::CallCallback( CallbackArgs& args )
{

}