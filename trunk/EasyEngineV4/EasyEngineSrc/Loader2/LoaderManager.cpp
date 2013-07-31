#define LOADERMANAGER_CPP
#include "loadermanager.h"

// stl
#include <map>
#include <algorithm>

// Engine
#include "../utils2/chunk.h"
#include "../utils2/StringUtils.h"
#include "TextureLoader.h"
#include "AseLoader.h"
#include "ASMELoader.h"
#include "ahmoLoader.h"
#include "LightLoader.h"
#include "BMELoader.h"
#include "BKELoader.h"
#include "BSELoader.h"
#include "IFileSystem.h"

using namespace std;

CLoaderManager::CLoaderManager( const Desc& oDesc ):
ILoaderManager( oDesc ),
m_oFileSystem( oDesc.m_oFileSystem )
{
	m_mLoaderByExtension[ "ase" ] = new CAseLoader;
	m_mLoaderByExtension[ "ale" ] = new CLightLoader;
	m_mLoaderByExtension[ "bmp" ] = new CBMPLoader;
	m_mLoaderByExtension[ "tga" ] = new CTGALoader;
	m_mLoaderByExtension[ "bme" ] = new CBMELoader( oDesc.m_oFileSystem, oDesc.m_oGeometryManager );
	m_mLoaderByExtension[ "bke" ] = new CBKELoader( oDesc.m_oFileSystem );
	m_mLoaderByExtension[ "bse" ] = new CBSELoader( oDesc.m_oFileSystem );
}

CLoaderManager::~CLoaderManager()
{
	map< string, ILoader* >::iterator itLoader = m_mLoaderByExtension.begin();
	for( ; itLoader != m_mLoaderByExtension.end(); itLoader++ )
		delete itLoader->second;
}

ILoader* CLoaderManager::GetLoader( std::string sExtension )
{
	string sLowExtension = sExtension;
	transform( sExtension.begin(), sExtension.end(), sLowExtension.begin(), tolower );
	map< string, ILoader* >::iterator itLoader = m_mLoaderByExtension.find( sLowExtension );
	if ( itLoader != m_mLoaderByExtension.end() )
		return itLoader->second;
	else
		return NULL;
}

void CLoaderManager::LoadTexture( string sFileName, ILoader::CTextureInfos& ti )
{
	string sExtension;
	CStringUtils::GetExtension( sFileName, sExtension );
	std::transform( sExtension.begin(), sExtension.end(), sExtension.begin(), tolower );
	if( sExtension == "tga" )
	{
		CTGALoader* pLoader = static_cast< CTGALoader* >(  m_mLoaderByExtension[ "bme" ] );
		pLoader->Load( sFileName, ti, m_oFileSystem );
	}
	else if( sExtension == "bmp" )
	{
		CBMPLoader* pLoader = static_cast< CBMPLoader* >(  m_mLoaderByExtension[ "bmp" ] );
		pLoader->Load( sFileName, ti, m_oFileSystem );
	}
}

void CLoaderManager::Load( string sFileName, ILoader::IRessourceInfos& ri )
{
	string sExtension;
	CStringUtils::GetExtension( sFileName, sExtension );
	map< string, ILoader* >::iterator itLoader = m_mLoaderByExtension.find( sExtension );
	if ( itLoader != m_mLoaderByExtension.end() )
	{
		itLoader->second->Load( sFileName, ri, m_oFileSystem );
		ri.m_sFileName = sFileName;
	}
	else
	{
		string sMessage = "Impossible de charger " + sFileName + " : L'extension \"" + sExtension + "\" n'est pas g�r�e par le loader";
		ILoaderManager::CBadExtension e( sMessage );
		throw e;
	}
}

void CLoaderManager::Export( string sFileName, const ILoader::IRessourceInfos& ri )
{
	string sExtension;
	CStringUtils::GetExtension( sFileName, sExtension );
	map< string, ILoader* >::iterator itLoader = m_mLoaderByExtension.find( sExtension );
	if ( itLoader != m_mLoaderByExtension.end() )
	{
		string sDir;
		m_oFileSystem.GetRootDirectory( sDir );
		string sFilePath;
		string sPrefix = sFileName.substr( 0, 3 );
		if( sPrefix != "c:\\" && sPrefix != "C:\\" )
			sFilePath = sDir + "\\" + sFileName;
		else
			sFilePath = sFileName;
		try
		{
			itLoader->second->Export( sFilePath, ri );
		}
		catch( CFileNotFoundException& e )
		{
			itLoader->second->Export( sFileName, ri );
		}
	}
	else
	{
		string sMessage = "Impossible de charger \"" + sFileName + "\" : L'extension \"" + sExtension + "\" n'est pas g�r�e par le loader";
		exception e( sMessage.c_str() );
		throw e;
	}
}

void CLoaderManager::ExportAHMO( const string& sFileName, CChunk& oChunk )
{
	//CAHMOLoader::GetInstance()->Export( sFileName.c_str(), oChunk );
}

void CLoaderManager::CreateBMPFromData( const vector< unsigned char >& vData, int nWidth, int nHeight, int nBitPerPixel, string sFileName )
{
	CBMPLoader* pLoader = static_cast< CBMPLoader* >( m_mLoaderByExtension[ "bmp" ] );
	pLoader->CreateBMPFromData( vData, nWidth, nHeight, nBitPerPixel, sFileName );
}

extern "C" _declspec(dllexport) ILoaderManager* CreateLoaderManager( const ILoaderManager::Desc& oDesc )
{
	return new CLoaderManager( oDesc );
}