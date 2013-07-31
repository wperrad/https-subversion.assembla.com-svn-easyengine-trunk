#include "Database2.h"

#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <mysql/mysql.h>

using namespace std;



CDatabase2::CDatabase2(  string sDSN )
{
	m_sDSN = sDSN;
	SQLHANDLE henv;
	SQLTest( SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv ), "Allocation du handle pour la base de donn�e impossible" );
	SQLTest( SQLSetEnvAttr( henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0 ), "Impossible d'attribuer un attribut d'environnement" );
	SQLTest( SQLAllocHandle( SQL_HANDLE_DBC, henv, &m_hdbc ), "Impossible d'allouer un handle" );
	int nTime = 10;
	SQLTest( SQLSetConnectAttr( m_hdbc, SQL_LOGIN_TIMEOUT, &nTime, 0 ), "Impossible d'attribuer un attribut d'environnement" );
}

void CDatabase2::Connect( std::string sUser, std::string sPassword )
{
	SQLTest( SQLConnectA( m_hdbc, (SQLCHAR*) m_sDSN.c_str(), (SQLSMALLINT)m_sDSN.size(), (SQLCHAR*)sUser.c_str(), (SQLSMALLINT)sUser.size(), (SQLCHAR*)sPassword.c_str(), (SQLSMALLINT)sPassword.size() ), "Erreur lors de la connection � la base de donn�es" );
}

void CDatabase2::Execute( std::string sQuery )
{
	SQLTest( SQLAllocStmt( m_hdbc, &m_hstmt ), "Erreur lors de la cr�ation du statement" );
	try
	{
		SQLTest( SQLExecDirectA( m_hstmt, (SQLCHAR*)sQuery.c_str(), (SQLINTEGER)sQuery.size() ), "" );
	}
	catch( exception& e )
	{
		string sErrorMsg = string( "Erreur lors de l'ex�cution de la requ�te : \n" ) + sQuery;
		exception e2( sErrorMsg.c_str() );
		throw e2;
	}
	
}


void CDatabase2::SQLTest( SQLRETURN ret, string sMsg )
{
	if ( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
	{
		exception e( sMsg.c_str() );
		throw e;
	}
}

int CDatabase2::GetRowCount()
{
	SQLINTEGER nb = 0;
	SQLTest( SQLRowCount ( m_hstmt, &nb), "Erreur lors de l'ex�cution de GetRowCount()" );
	return nb;
}

void CDatabase2::Disconnect()
{
	SQLTest( SQLFreeConnect( m_hdbc ), "Probl�me lors de la d�connection" );
}

CDatabase2::~CDatabase2(void)
{
	
}
