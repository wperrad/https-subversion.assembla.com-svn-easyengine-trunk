#ifndef BSELOADER_H
#define BSELOADER_H

#include "ILoader.h"
#include "IEntity.h"

class CBSELoader : public ILoader
{
public:

	CBSELoader( IFileSystem& oFileSystem );
	void	Load( string sFileName, ILoader::IRessourceInfos& ri, IFileSystem& );
	void	Export( string sFileName, IRessourceInfos& si );

private:
	IFileSystem&							m_oFileSystem;	
	void									WriteAnimationSpeed(FILE* pFile, const map< string, float>& mapAnimationName);
	void									ReadAnimationSpeed(FILE* pFile, map< string, float>& mapAnimationName);
};

#endif // BSELOADER_H