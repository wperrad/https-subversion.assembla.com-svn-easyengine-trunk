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
	std::string					m_sLastDirectory;
	int				GetDirectoryDepth( string sDirectory );

public:

	CFileSystem(EEInterface& oInterface);
	~CFileSystem();

	void		Mount( std::string sDirectory );
	FILE*		OpenFile( const std::string& sFileName, const std::string& sMode );
	HANDLE		FindFirstFile_EE( const std::string& sMask, WIN32_FIND_DATAA& data ) const;
	void		GetLastDirectory( std::string& sRootDirectory ) const;
	string		GetName() override;
};

extern "C" _declspec(dllexport) IFileSystem* CreateFileSystem(EEInterface& oInterface);

#endif // FILESYSTEM_H