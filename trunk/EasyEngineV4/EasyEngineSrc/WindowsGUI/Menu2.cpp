#include "Menu2.h"

using namespace std;

int	CMenu2::s_nLastID = 1;


CMenu2::CMenu2( string sMenuName ):
m_nID( -1 ),
m_pfnCallback( NULL )
{
	m_sName = sMenuName;
	m_hHandle = CreateMenu();
	//m_nID = s_nLastID;
	s_nLastID++;
}

CMenu2::~CMenu2(void)
{
}

int CMenu2::AddString( string sString )
{
	m_nID = s_nLastID;
	s_nLastID++;
	AppendMenuA( m_hHandle, MF_STRING,m_nID ,sString.c_str() );
	return m_nID;
}

void CMenu2::AddMenu( CMenu2* pMenu )
{
	m_vChild.push_back( pMenu );
	AppendMenuA( m_hHandle, MF_POPUP,(UINT_PTR)pMenu->m_hHandle, pMenu->m_sName.c_str() );
}

CMenu2* CMenu2::GetSubMenu( int nID )const
{
	for ( unsigned int i = 0; i < m_vChild.size(); i++ )
	{
		CMenu2* pChild = m_vChild[ i ];
		if ( pChild->m_nID == nID )
			return pChild;
		CMenu2* pSubChild = GetSubMenu( nID );
		if ( pSubChild )
			return pSubChild;
	}
	return NULL;
}

void CMenu2::SetCallback( void(*pfnCallback)(int) )
{
	m_pfnCallback = pfnCallback;
}