#ifndef CLIENT_H
#define CLIENT_H

#include "Socket.h"

class CClient : public ISocket
{
	friend class CServer;
public:
	CClient(void);
	virtual ~CClient(void);
	void	Connect( std::string sIP, int nPort );
	void	Disconnect();
	bool	IsConnected();
	void	Send( std::string sMsg );
	void	Receive( std::string& sMsg );
};


#endif // CLIENT_H