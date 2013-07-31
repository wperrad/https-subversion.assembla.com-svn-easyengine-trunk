#ifndef ISOCKET_H
#define ISOCKET_H

//#include <winsock2.h>
//#include <windows.h>
#include <string>

typedef _W64 unsigned int UINT_PTR, *PUINT_PTR;
typedef UINT_PTR        SOCKET;

class ISocket
{

protected:
	SOCKET	m_Socket;

public:
			ISocket(void);
	virtual ~ISocket(void) = 0;	
};

#endif // ISOCKET_H