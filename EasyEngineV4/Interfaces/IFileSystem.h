#ifndef IFILESYSTEM_H
#define IFILESYSTEM_H

#include <string>
#include <windows.h>
#include "EEPlugin.h"

class IFileSystem : public CPlugin
{
public:
	IFileSystem() : CPlugin(nullptr, "") {}
	virtual void	Mount( std::string sDirectory ) = 0;
	virtual FILE*	OpenFile( const std::string& sFileName, const std::string& sMode ) = 0;
	virtual HANDLE	FindFirstFile_EE( const std::string& sMask, WIN32_FIND_DATAA& data ) const = 0;
	virtual void	GetLastDirectory( std::string& sRootDirectory ) const = 0;
	virtual bool	DeleteDirectory(string dir) = 0;
};

#endif // IFILESYSTEM_H