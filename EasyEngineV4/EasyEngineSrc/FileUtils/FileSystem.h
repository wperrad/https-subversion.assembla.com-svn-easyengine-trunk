#ifndef FILESYSTEM_CPP
#error
#endif

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>

#include "IFileSystem.h"
#include "EEPlugin.h"

using namespace std;
class CFileSystem : public IFileSystem
{
	std::vector< std::string >	m_vDirectory;
	std::string					m_sRootDirectory;
	int				GetDirectoryDepth( string sDirectory );

public:

	CFileSystem( const IFileSystem::Desc& oDesc );
	~CFileSystem();

	void		Mount( std::string sDirectory );
	FILE*		OpenFile( const std::string& sFileName, const std::string& sMode );
	HANDLE		FindFirstFile_EE( const std::string& sMask, WIN32_FIND_DATAA& data ) const;
	void		GetRootDirectory( std::string& sRootDirectory ) const;
};

extern "C" _declspec(dllexport) IFileSystem* CreateFileSystem( const IFileSystem::Desc& );

#endif // FILESYSTEM_H