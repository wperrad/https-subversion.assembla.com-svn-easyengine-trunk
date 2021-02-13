#ifndef BSELOADER_H
#define BSELOADER_H

#include "ILoader.h"
#include "IEntity.h"

class CBSELoader : public ILoader
{
public:

	CBSELoader( IFileSystem& oFileSystem );
	void	Load( string sFileName, ILoader::IRessourceInfos& ri, IFileSystem& );
	void	Export( string sFileName, const IRessourceInfos& si );

private:
	IFileSystem&							m_oFileSystem;	
	void									ReadString(FILE* pFile, string& sString);
	void									WriteString(FILE* pFile, string sString);
	void									WriteObject(CSceneObjInfos* pObjInfos, FILE* pFile);
	CSceneObjInfos*							ReadObject(FILE* pFile);
	void									WriteAnimationSpeed(FILE* pFile, const map< string, float>& mapAnimationName);
	void									ReadAnimationSpeed(FILE* pFile, map< string, float>& mapAnimationName);
};

#endif // BSELOADER_H