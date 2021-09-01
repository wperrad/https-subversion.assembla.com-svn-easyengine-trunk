#ifndef LIGHTLOADER_H
#define LIGHTLOADER_H

#include "loader.h"

class CLightLoader : public ILoader
{
public:
					CLightLoader(void);
	virtual			~CLightLoader(void);
	void			Export( const std::string& szFileName, const CChunk& chunk);
	void			Load( string sFileName, ILoader::IRessourceInfos& ri, IFileSystem& );
	void			Export( string sFileName, ILoader::IRessourceInfos& ri ){throw 1;}
};


#endif //LIGHTLOADER_H