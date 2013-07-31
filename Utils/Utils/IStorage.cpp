#include "IStorage.h"
#include <sstream>
#include <iomanip>

using namespace std;

CFileStorage::CFileStorage():
m_pFile( NULL )
{
}


void CFileStorage::CloseFile()
{
	fclose( m_pFile );
	m_pFile = NULL;
}

CFileStorage::~CFileStorage()
{
	if( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = NULL;
	}
}

bool CBinaryFileStorage::OpenFile( string sFileName, TOpenMode mode )
{
	if( m_pFile )
		fclose( m_pFile );
	string sMode;
	if( mode == eRead )
		sMode = "r";
	else if( mode == eWrite )
		sMode = "w";
	sMode += "b";
	m_pFile = fopen( sFileName.c_str(), sMode.c_str() );
	return m_pFile != NULL;
}

IBaseStorage& CBinaryFileStorage::operator<<( int i )
{
	fwrite( &i, sizeof( int ), 1, m_pFile );
	return *this;
}

IBaseStorage& CBinaryFileStorage::operator<<( unsigned int i )
{
	fwrite( &i, sizeof( unsigned int ), 1, m_pFile );
	return *this;
}

IBaseStorage& CBinaryFileStorage::operator<<( float f )
{
	fwrite( &f, sizeof( float ), 1, m_pFile );
	return *this;
}

IBaseStorage& CBinaryFileStorage::operator<<( string s )
{
	int nSize = (int)s.size();
	fwrite( &nSize, sizeof( int ), 1, m_pFile );
	fwrite( s.c_str(), sizeof( char ), s.size(), m_pFile );
	return *this;
}

IBaseStorage& CBinaryFileStorage::operator>>( int& i ) 
{
	fread( &i, sizeof( int ), 1, m_pFile );
	return *this;
}

IBaseStorage& CBinaryFileStorage::operator>>( unsigned int& i )
{
	fread( &i, sizeof( unsigned int ), 1, m_pFile );
	return *this;
}

IBaseStorage& CBinaryFileStorage::operator>>( float& f )
{
	fread( &f, sizeof( float ), 1, m_pFile );
	return *this;
}

IBaseStorage& CBinaryFileStorage::operator>>( string& s )
{
	int nSize;
	fread( &nSize, sizeof( int ), 1, m_pFile );
	s.resize( nSize );
	fread( &s[0], sizeof( char ), nSize, m_pFile );
	return *this;
}

IBaseStorage& CBinaryFileStorage::operator<<( const IPersistantObject& object )
{
	object.Store( *this );
	return *this;
}

IBaseStorage& CBinaryFileStorage::operator>>( IPersistantObject& object )
{
	object.Load( *this );
	return *this;
}


CAsciiFileStorage::CAsciiFileStorage():
m_nWidth( 10 ),
m_dCap( 0. ),
m_nPrecision( 7 ),
m_bDisplayVectorInLine( false ),
m_bEnableVectorEnumeration( true )
{
}


bool CAsciiFileStorage::OpenFile( string sFileName, TOpenMode mode )
{
	if( m_pFile )
		fclose( m_pFile );
	string sMode;
	if( mode == eRead )
		sMode = "r";
	else if( mode == eWrite )
		sMode = "w";
	m_pFile = fopen( sFileName.c_str(), sMode.c_str() );
	return m_pFile != NULL;
}

void CAsciiFileStorage::SetCurrentMapNames( string sKey, string sValue )
{
	m_sCurrentMapKeyName = sKey;
	m_sCurrentMapValueName = sValue;
}

void CAsciiFileStorage::SetWidth( int nWidth )
{
	m_nWidth = nWidth;
}

void CAsciiFileStorage::SetPrecision( int nPrecision )
{
	m_nPrecision = nPrecision;
}

void CAsciiFileStorage::SetCap( double dCap )
{
	m_dCap = dCap;
}

void CAsciiFileStorage::DisplayVectorInLine( bool dDisplayInLine )
{
	m_bDisplayVectorInLine = dDisplayInLine;
}

void CAsciiFileStorage::EnableVectorEnumeration( bool bEnable )
{
	m_bEnableVectorEnumeration = bEnable;
}

int CAsciiFileStorage::GetWidth()
{
	return m_nWidth;
}
	
int	CAsciiFileStorage::GetPrecision()
{
	return m_nPrecision;
}

double CAsciiFileStorage::GetCap()
{
	return m_dCap;
}

IBaseStorage& CAsciiFileStorage::operator<<( int nVal )
{
	//ostringstream oss;
	//oss << nVal;
	CStringStorage ss;
	ss.SetWidth( m_nWidth );
	ss << nVal;
	*this << ss.GetValue();
	return *this;
}

IBaseStorage& CAsciiFileStorage::operator<<( unsigned int nVal )
{
	//ostringstream oss;
	//oss << nVal;
	CStringStorage ss;
	ss.SetWidth( m_nWidth );
	ss << nVal;
	*this << ss.GetValue();
	return *this;
}
	
IBaseStorage& CAsciiFileStorage::operator<<( float f )
{
	//ostringstream oss;
	//oss << f;
	//fwrite( oss.str().c_str(), sizeof( char ), oss.str().size(), m_pFile );
	CStringStorage ss;
	ss.SetWidth( m_nWidth );
	ss << f;
	*this << ss.GetValue();
	return *this;
}
	
CAsciiFileStorage& CAsciiFileStorage::operator<<( string s )
{
	fwrite( s.c_str(), sizeof( char ), s.size(), m_pFile );
	return *this;
}



IBaseStorage& CAsciiFileStorage::operator>>( int& )
{
	return *this;
}

IBaseStorage& CAsciiFileStorage::operator>>( unsigned int& )
{
	return *this;
}
	
IBaseStorage& CAsciiFileStorage::operator>>( float& )
{
	return *this;
}
	
IBaseStorage& CAsciiFileStorage::operator>>( string& )
{
	return *this;
}


IBaseStorage& CAsciiFileStorage::operator<<( const IPersistantObject& object )
{
	object.Store( *this );
	return *this;
}

IBaseStorage& CAsciiFileStorage::operator>>( IPersistantObject& object )
{
	return *this;
}

void CAsciiFileStorage::SetVectorElementsName( string sVectorElementName )
{
	m_sVectorElementName = sVectorElementName;
}

CStringStorage::CStringStorage():
m_dCap( 0. ),
m_nWidth( 10 ),
m_nPrecision( 7 )
{
}

double CStringStorage::GetCaped( double dNumber, double dCap )
{
	if( dNumber > dCap || dNumber < -dCap )
		return dNumber;
	return 0.;
}
	
void CStringStorage::SetWidth( int nWidth )
{
	m_nWidth = nWidth;
}
	
void CStringStorage::SetPrecision( int nPrecision )
{
	m_nPrecision = nPrecision;
}

void CStringStorage::SetCap( double dCap )
{
	m_dCap = dCap;
}

string& CStringStorage::GetValue()
{
	return m_sValue;
}

IBaseStorage& CStringStorage::operator<<( int nValue )
{
	ostringstream oss;
	oss << setw( m_nWidth ) << setprecision( m_nPrecision ) << nValue;
	*this << oss.str();
	return *this;
}

IBaseStorage& CStringStorage::operator<<( unsigned int uValue )
{
	ostringstream oss;
	oss << setw( m_nWidth ) << setprecision( m_nPrecision ) << uValue;
	*this << oss.str();
	return *this;
}

IBaseStorage& CStringStorage::operator<<( float fValue )
{
	ostringstream oss;
	oss << setw( m_nWidth ) << setprecision( m_nPrecision ) << GetCaped( fValue, m_dCap );
	*this << oss.str();
	return *this;
}

IBaseStorage& CStringStorage::operator<<( string sValue )
{
	m_sValue += sValue;
	return *this;
}

IBaseStorage& CStringStorage::operator>>( int& )
{
	return *this;
}

IBaseStorage& CStringStorage::operator>>( unsigned int& )
{
	return *this;
}

IBaseStorage& CStringStorage::operator>>( float& )
{
	return *this;
}

IBaseStorage& CStringStorage::operator>>( string& sValue )
{
	sValue = m_sValue;
	return *this;
}

IBaseStorage& CStringStorage::operator<<( const IPersistantObject& object )
{
	object.Store( *this );
	return *this;
}

IBaseStorage& CStringStorage::operator>>( IPersistantObject& object )
{
	object.Load( *this );
	return *this;
}