#include <exception>
#include "Server.h"
#include <winsock2.h>
#include <windows.h>

using namespace std;

CServer::CServer(void)
{
}

CServer::~CServer(void)
{
}

void CServer::Listen( int nPort )
{
	m_Socket = socket( PF_INET, SOCK_STREAM, 0);
	if ( m_Socket == INVALID_SOCKET )
	{
		exception e( "Impossible de créer la socket du serveur" );
		throw e;
	}

	SOCKADDR_IN SockServerDesc;
	ZeroMemory( &SockServerDesc, sizeof( SockServerDesc ) );
	SockServerDesc.sin_addr.s_addr = htonl( ADDR_ANY );
	SockServerDesc.sin_family = AF_INET;
	SockServerDesc.sin_port = htons( nPort );
	bind( m_Socket, (SOCKADDR*)&SockServerDesc, sizeof( SockServerDesc ) );
	listen( m_Socket, 0);
}

void CServer::Accept( CClient& client )
{
	SOCKADDR_IN SockClientDesc;
	int nAddrLen = sizeof(SockClientDesc);
	SOCKET SockClient = accept( m_Socket, (SOCKADDR*)&SockClientDesc, &nAddrLen );
	if ( SockClient == -1 )
	{
		exception e( "InvalidClientSocket" );
		throw e;
	}
	client.m_Socket = SockClient;
}

void CServer::Close()
{
	closesocket( m_Socket );
	m_Socket = 0;
}

bool CServer::IsUp()
{
	return m_Socket != 0;
}