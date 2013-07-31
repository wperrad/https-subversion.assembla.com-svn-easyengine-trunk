#include "NetworkManager.h"
#include <winsock2.h>
#include <windows.h>


CNetworkManager::CNetworkManager(void)
{
	WSADATA data;
	WSAStartup( MAKEWORD(2,0), &data );
}

CNetworkManager::~CNetworkManager(void)
{
	WSACleanup();
}
