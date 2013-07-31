#ifndef SERVER_H
#define SERVER_H

#include "Socket.h"
#include "Client.h"

class CServer : public ISocket
{
public:
				CServer(void);
				~CServer(void);
	void		Listen( int nPort );
	void		Accept( CClient& client );
	bool		IsUp();
	void		Close();
};


#endif // SERVER_H