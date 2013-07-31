#include "MysqlDatabase.h"
#include <windows.h>
#include <mysql/mysql.h>



using namespace std;

MYSQL*			m_pConnexion;

CMysqlDatabase::CMysqlDatabase( std::string sHost )
{
	m_sHost = sHost;
	m_pConnexion = NULL;
}

CMysqlDatabase::~CMysqlDatabase(void)
{
	if ( m_pConnexion )
		Disconnect();
}

void CMysqlDatabase::Connect( string sUser, string sPass, string sDatabaseName )
{
	m_pConnexion = mysql_init( NULL );
	if ( m_pConnexion == NULL )
	{
		exception e( "Impossible d'initialiser mysql" );
		throw e;
	}
	if ( mysql_real_connect( m_pConnexion, m_sHost.c_str(), sUser.c_str(), sPass.c_str(), sDatabaseName.c_str(), 0, NULL, 0 ) == NULL )
	{
		exception e( " Erreur lors de la connection à la base de données" );
		throw e;
	}
}
void CMysqlDatabase::Disconnect()
{
	mysql_close( m_pConnexion );
	m_pConnexion = NULL;
}

void CMysqlDatabase::Query( std::string sQuery, std::vector< std::vector< std::string > >& vResult )const
{
	if ( mysql_query( m_pConnexion, sQuery.c_str() ) != 0 )
	{
		exception e( "Erreur lors de la requète" );
		throw e;
	}
	MYSQL_RES* pResult		= mysql_store_result( m_pConnexion );
	if ( pResult != NULL )
	{
		my_ulonglong nRowCount	= mysql_num_rows( pResult );
		while ( MYSQL_ROW pRows = mysql_fetch_row( pResult ) )
		{
			vector< string > vRow;
			unsigned int nNumField	= mysql_num_fields( pResult );
			for ( unsigned int i = 0; i < nNumField; i++ )
			{
				char* pRow = pRows[ i ];
				if ( pRow != NULL )
					vRow.push_back( pRow );
				else
					vRow.push_back( "null" );
			}
			vResult.push_back( vRow );
		}
	}
}