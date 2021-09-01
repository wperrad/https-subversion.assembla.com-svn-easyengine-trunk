// System
#include <windows.h>

// Utils
#include "../Utils2/StringUtils.h"
#include "../Utils2/Rectangle.h"

// Engine
#include "Interface.h"
#include "xmlparser.h"
#include "Exception.h"
#include "IFileSystem.h"


using namespace std;


CXMLInfo::CXMLInfo( const string& sProperty, float x, float y, float width, float height) :
IXMLInfo( sProperty, x, y, width, height ),
m_Rect( CRectangle( x, y, width, height ) )
{
	for ( unsigned int i = 0 ;i < sProperty.size(); i++ )
		m_sName.push_back( sProperty[ i ] );
	m_Rect = CRectangle( x, y, width, height );
}

CXMLInfo::CXMLInfo( const string& sProperty, CRectangle rect) :
IXMLInfo( sProperty, rect ),
m_Rect( rect )
{
	m_sName = sProperty;
}

void CXMLInfo::GetRectangle( CRectangle& rect )
{
	rect = m_Rect;
}


void CXMLInfo::GetName( string& sName )
{
	sName = m_sName;
}


//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------

CXMLParser::CXMLParser(EEInterface& oInterface) :
IXMLParser(),
m_oFileSystem(*static_cast<IFileSystem*>(oInterface.GetPlugin("FileSystem"))),
m_bIsParsing( false )
{
}

void CXMLParser::OpenFile( const string& sXMLFileName )
{
	if ( m_bIsParsing )
	{
		CXMLParserException e( sXMLFileName );
		throw e;
	}
	try
	{
		m_File.Open( sXMLFileName, m_oFileSystem );
	}
	catch ( CEasyFileException& e )
	{
		string strMessage = sXMLFileName + " not found";
		MessageBoxA(NULL , strMessage.c_str() , "File exception", MB_ICONERROR);		
	}
	size_t nSlachPos = sXMLFileName.find_last_of("/");
	string sShortName = sXMLFileName.substr( nSlachPos+1 , sXMLFileName.size() - nSlachPos - 5 );
	string sOpenBalideName = string( "<" ) + sShortName + ">";
	bool bOpenBaliseFound = false;
	do
	{
		string sBuffer;
		m_File.GetLine( sBuffer );
		int nPos = (int)sBuffer.find( sOpenBalideName );
		if ( nPos != -1 )
			bOpenBaliseFound = true;
	}
	while ( !bOpenBaliseFound );
	m_bIsParsing = true;
}

void CXMLParser::CloseFile()
{
	if ( !m_bIsParsing )
	{
		CXMLParserException e( "" );
		throw e;
	}
	m_File.Close();
	m_bIsParsing = false;
}

void CXMLParser::GetProperty( const string& sFieldName, CPosition& Pos, int& nDimWidth, int& nDimHeight )
{
	if ( !m_bIsParsing )
	{
		string sName;
		m_File.GetName(sName);
		CXMLParserException e(sName.c_str());
		throw e;
	}
	
	bool bFound = false;
	string sBuffer;
	do
	{
		m_File.GetLine( sBuffer );
		int nFirstPos = (int)sBuffer.find( sFieldName );
		if ( nFirstPos > 0 )
		{
			if ( sBuffer[ nFirstPos - 1 ] == '<' )
				bFound = true;
		}
		else
			sBuffer.clear();
	}
	while ( !bFound );

	vector< float > vFloat;
	CStringUtils::ExtractFloatFromString( sBuffer, vFloat, 4 );
	Pos.SetX( vFloat[ 0 ] );
	Pos.SetY( vFloat[ 1 ] );
	//Dim.SetWidth( vFloat[ 2 ] );
	//Dim.SetHeight( vFloat[ 3 ] );	
	nDimWidth = static_cast< int >( vFloat[ 2 ] );
	nDimHeight = static_cast< int >( vFloat[ 3 ] );
}


void CXMLParser::ParseFile( const string& sFileName, vector< IXMLInfo* >& vInfos )
{
	OpenFile( sFileName );
	size_t nSlachPos = sFileName.find_last_of("/");
	string strShortName = sFileName.substr( nSlachPos+1 , sFileName.size() - nSlachPos - 5 );
	string sBalise = string("<") + strShortName + string("/>");	
	do
	{
		string sBuffer;
		try
		{
			m_File.GetLine( sBuffer );
		}
		catch (CEOFException& e)
		{
			m_File.Close();
			m_bIsParsing = false;
			return;
		}

		int nBeginCarPos = (int)sBuffer.find_first_of( "<" );
		int nSlashPos = (int)sBuffer.find_first_of( "/" );
		int nEndCarPos = (int)sBuffer.find_first_of( ">" );
		if ( (nBeginCarPos != -1) && (nEndCarPos !=-1) && (nSlashPos != -1) )
		{
			string sProperty;
			CStringUtils::GetWordByIndex( sBuffer, 0 , sProperty);
			sProperty = sProperty.substr( 1, sProperty.size() );
			vector< float > vFloat;
			string s = sBuffer.substr( sProperty.size() + 1, sBuffer.size() );
			CStringUtils::ExtractFloatFromString( s, vFloat, 4);
			CRectangle rect( vFloat[0] , vFloat[1], vFloat[2], vFloat[3]  );
			CXMLInfo* pInfo = new CXMLInfo( sProperty, rect );
			vInfos.push_back( pInfo );
		}
	} 
	while( !m_File.Eof() );
	CloseFile();	
}

string CXMLParser::GetName()
{
	return "XMLParser";
}

extern "C" _declspec(dllexport) IXMLParser* CreateXMLParser(EEInterface& oInterface)
{
	return new CXMLParser(oInterface);
}