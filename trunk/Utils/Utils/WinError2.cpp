#include "WinError2.h"
#include <WinError.h>

using namespace std;

CWinError::CWinError(void)
{
	m_mError[ ERROR_RESOURCE_TYPE_NOT_FOUND ] = "The specified resource type cannot be found in the image file";
	m_mError[ ERROR_INVALID_PIXEL_FORMAT ] = "The pixel format is invalid.";
}

CWinError::~CWinError(void)
{
}


void CWinError::GetErrorString( int nErrorCode, string& sError )
{
	map< int, string >::iterator itError = m_mError.find( nErrorCode );
	if ( itError != m_mError.end() )
		sError = m_mError[ nErrorCode ];
	else
		sError = "Undefined error";
}