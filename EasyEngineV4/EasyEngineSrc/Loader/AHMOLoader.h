#ifndef HIERARCHMESHLOADER_H
#define HIERARCHMESHLOADER_H

#include <map>
#include <vector>

#include "Loader.h"

class CEasyFile;

class CAHMOLoader : ILoader
{
	void						LoadHierarchy(CChunk& chunk);

public:
	
	CAHMOLoader(void){}
	~CAHMOLoader(void){}
	bool						Load( const std::string&, CChunk& chunk, IFileSystem& oFileSystem ){return true;}
	void						Export( const std::string& sFileName, CChunk& chunk ){}

};

#endif /HIERARCHMESHLOADER_H