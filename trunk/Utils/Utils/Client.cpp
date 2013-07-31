#include "Client.h"
#include <winsock2.h>
#include <windows.h>

using namespace std;

CClient::CClient(void)
{
}

CClient::~CClient(void)
{
}

void CClient::Connect( string sIP, int nPort )
{
	if ( m_Socket != 0 )
		Disconnect();
	m_Socket = socket( PF_INET, SOCK_STREAM, 0);
	if( m_Socket == INVALID_SOCKET )
	{
		exception e( "Impossible de créer la socket client" );
		throw e;
	}
	SOCKADDR_IN SockDesc;
	ZeroMemory( &SockDesc, sizeof(SockDesc) );
	SockDesc.sin_addr.s_addr = inet_addr( sIP.c_str() );
	SockDesc.sin_family = PF_INET;
	SockDesc.sin_port = htons( nPort );
	int nConnect = connect( m_Socket, (SOCKADDR*)&SockDesc, sizeof(SockDesc) );
	if ( nConnect != 0 )
	{
		exception e( "Serveur introuvable, connection impossible" );
		throw e;
	}
}

void CClient::Disconnect()
{
	closesocket( m_Socket );
	m_Socket = 0;
}

bool CClient::IsConnected()
{
	return ( m_Socket != INVALID_SOCKET  );
}

void CClient::Send( string sMsg )
{
	int nTotalCount = 0, nCount = 0;
	do
	{
		nCount = send( m_Socket, sMsg.c_str(), (int)sMsg.size() + 1, 0);
		if ( nCount < 0 )
		{
			exception e( "Server_disconnected" );
			throw e;
		}
		nTotalCount += nCount;
	}
	while ( nTotalCount < (int)sMsg.size() );
}

void CClient::Receive( string& sMsg )
{
	char buffer[ 256 ];
	int nCount = recv( m_Socket, buffer, 256, 0 );
	if ( nCount == -1 || nCount == 0 )
	{
		exception e( "client_disconnected" );
		throw e;
	}
	buffer[ nCount + 1 ] = 0;
	sMsg = buffer;
}