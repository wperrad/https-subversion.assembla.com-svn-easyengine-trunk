#include "Socket.h"
#include <winsock2.h>
#include <windows.h>

using namespace std;

ISocket::ISocket(void): m_Socket( 0 )
{
}

ISocket::~ISocket(void)
{
}

