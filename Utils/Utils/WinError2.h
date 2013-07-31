#ifndef WINERROR2_H
#define WINERROR2_H

#include <map>

class CWinError
{
	std::map< int, std::string > m_mError;
public:
	CWinError(void);
	~CWinError(void);
	void GetErrorString( int nErrorCode, std::string& sError );
};

#endif // WINERROR2_H

