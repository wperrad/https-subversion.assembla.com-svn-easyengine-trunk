#define FILESYSTEM_CPP

#include "FileSystem.h"

using namespace std;

CFileSystem::CFileSystem( const Desc& oDesc ):
IFileSystem( oDesc )
{
	m_vDirectory.push_back( "" );
}

CFileSystem::~CFileSystem()
{
}

void CFileSystem::Mount( string sDirectory )
{
	m_vDirectory.push_back( sDirectory );
}

FILE* CFileSystem::OpenFile( const string& sFileName, const string& sMode )
{
	FILE* pFile = NULL;
	if (m_sLastDirectory.size() >  0 )
	{
		string sCurrentPath = m_sLastDirectory + "\\" + sFileName;
		fopen_s( &pFile, sCurrentPath.c_str(), sMode.c_str() );
		if ( pFile )
			return pFile;
	}
	unsigned int i = 0;
	while ( pFile == NULL && i < m_vDirectory.size() )
	{
		string sPath;
		if ( m_vDirectory[ i ] == "" )
			sPath = sFileName;
		else
			sPath = m_vDirectory[ i ] + "\\" + sFileName;
		fopen_s( &pFile, sPath.c_str(), sMode.c_str() );
		i++;
	}
	if ( pFile )
		m_sLastDirectory = m_vDirectory[ i - 1 ];
	return pFile;
}

int CFileSystem::GetDirectoryDepth( string sDirectory )
{
	bool bEnd = false;
	int nDepth = 0;
	string s = string( "\\" ) + sDirectory + "\\";
	int n = sDirectory.find( s );
	while( !bEnd )
	{
		string sTemp = sDirectory.substr( n, sDirectory.size() - n );
		int p = sTemp.find( "\\" );
		if( p == -1 )
			bEnd = false;
		else
			nDepth++;
	}
	return nDepth;
}

HANDLE CFileSystem::FindFirstFile_EE( const string& sMask, WIN32_FIND_DATAA& data ) const
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	unsigned int i = 0;
	while ( hFile == INVALID_HANDLE_VALUE && i < m_vDirectory.size() )
	{
		string sPath;
		if ( m_vDirectory[ i ].size() == 0 )
			sPath = sMask;
		else
			sPath = m_vDirectory[ i ] + "\\" + sMask;
		
		hFile = FindFirstFile( sPath.c_str(), &data );
		i++;
	}
	return hFile;
}

void CFileSystem::GetLastDirectory( std::string& sRootDirectory ) const
{
	sRootDirectory = m_sLastDirectory;
}

extern "C" _declspec(dllexport) IFileSystem* CreateFileSystem( const IFileSystem::Desc& oDesc )
{
	return new CFileSystem( oDesc );
}