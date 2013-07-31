#ifndef BSELOADER_H
#define BSELOADER_H

#include "ILoader.h"

class CBSELoader : public ILoader
{
	IFileSystem&	m_oFileSystem;
	void			ReadString( FILE* pFile, string& sString );
	void			WriteString( FILE* pFile, string sString );
	void			WriteObject( CSceneObjInfos* pObjInfos, FILE* pFile );
	CSceneObjInfos* ReadObject( FILE* pFile );
public:

	CBSELoader( IFileSystem& oFileSystem );
	void	Load( string sFileName, ILoader::IRessourceInfos& ri, IFileSystem& );
	void	Export( string sFileName, const IRessourceInfos& si );
};

#endif // BSELOADER_H