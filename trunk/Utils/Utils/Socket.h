#ifndef ISOCKET_H
#define ISOCKET_H

#include <winsock2.h>
#include <windows.h>
#include <string>


class ISocket
{

protected:
	SOCKET	m_Socket;

public:
			ISocket(void);
	virtual ~ISocket(void) = 0;	
};

#endif // ISOCKET_H