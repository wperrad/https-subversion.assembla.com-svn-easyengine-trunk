#ifndef _MYSQLDATABASE_H_
#define _MYSQLDATABASE_H_

// System
//#include <windows.h>

// stl
#include <string>
#include <vector>

// base de données


class CMysqlDatabase
{
	std::string		m_sHost;
	
public:
					CMysqlDatabase( std::string sHost );
					~CMysqlDatabase(void);
	void			Connect( std::string sUser, std::string sPass, std::string sDatabaseName );
	void			Disconnect();
	void			Query( std::string sQuery, std::vector< std::vector< std::string > >& vResult )const;
};

#endif // _MYSQLDATABASE_H_