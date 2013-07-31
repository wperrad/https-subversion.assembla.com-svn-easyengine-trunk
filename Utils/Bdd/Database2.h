#ifndef DATABASE2_H
#define DATABASE2_H

#include <windows.h>
#include <sql.h>
#include <string>

class CDatabase2
{
	std::string		m_sDSN;
	SQLHANDLE		m_hdbc;
	SQLHANDLE		m_hstmt;
public:
					CDatabase2( std::string sDSN );
					~CDatabase2(void);
	void			SQLTest( SQLRETURN ret, std::string sMsg );
	void			Connect( std::string sUser, std::string sPassword );
	void			Disconnect();
	void			Execute( std::string sQuery );
	int				GetRowCount();
};

#endif // DATABASE2_H