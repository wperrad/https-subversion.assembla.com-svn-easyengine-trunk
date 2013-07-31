#ifndef MENU2_H
#define MENU2_H

// system
#include <windows.h>

// stl
#include <map>
#include <string>
#include <vector>

class CMenu2
{
	friend class CWidget;
	friend class CWindow2;

	HMENU					m_hHandle;
	std::string				m_sName;
	int						m_nID;
	std::vector< CMenu2* >	m_vChild;
	void					(*m_pfnCallback)(int);

	static int				s_nLastID;

public:
	void					AddMenu( CMenu2* pMenu );
	int						AddString( std::string sString );
							CMenu2( std::string sMenuName );
							~CMenu2();
	CMenu2*					GetSubMenu( int nID )const;
	void					SetCallback( void(*pfnCallback)(int) );
};

#endif // MENU2_H