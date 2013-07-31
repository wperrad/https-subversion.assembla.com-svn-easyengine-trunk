#include "Convertissor.h"
#include "math.h"

using namespace std;

CConvertissor*	CConvertissor::m_pInstance;

CConvertissor::CConvertissor(void)
{
}

CConvertissor::~CConvertissor(void)
{
}
CConvertissor* CConvertissor::GetInstance()
{
	if ( !m_pInstance )
		m_pInstance = new CConvertissor;
	return m_pInstance;
}

void CConvertissor::DoubleToHexString( double d, string& sString )
{
	char buff[ 32 ];
	sprintf( buff, "%X%p", d );
	sString = buff;
}

double CConvertissor::HexStringToDouble( std::string s )
{
	
	double dRet = 0;
	for ( int i = s.size() - 1; i >= 0; i-- )
	{
		char c = s[ i ];
		int n;
		if ( c >= '0' && c <='9' )
			n = ( c - '0' );
		if ( ( c >= 'A' && c <= 'F' ) || ( c >= 'a' && c <= 'f' ) )
			n = c - 'A' + 10;
		int nPow = pow( 16., s.size() - (double)i - 1 );
		dRet += n * nPow;
	}
	return dRet;
}


void CConvertissor::HexStringToBinString( std::string s, string& sBin )
{
	for ( int i = 0; i < s.size(); i++ )
	{
		char c = s[ i ];
		int n;
		if ( c >= '0' && c <='9' )
			n = ( c - '0' );
		if ( ( c >= 'A' && c <= 'F' ) || ( c >= 'a' && c <= 'f' ) )
			n = c - 'A' + 10;

		int nReste = n;
		string sTempBin;
		for ( int j = 0; j < 4; j++ )
		{
			int nPow = pow( 2., (double) ( 4 - j - 1 ) );
			int p = nReste / nPow;
			if ( p > 0 )
			{
				sTempBin += "1";
				nReste -= nPow;
			}
			else
				sTempBin += "0";			
		}
		sBin += sTempBin;
	}
}