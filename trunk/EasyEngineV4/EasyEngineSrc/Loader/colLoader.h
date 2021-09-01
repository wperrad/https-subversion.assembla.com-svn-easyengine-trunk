#include "Loader.h"

class CColLoader : public ILoader
{
public:
	CColLoader(IGeometryManager& oGeometryManager);
	void Load(string sFileName, ILoader::IRessourceInfos& ri, IFileSystem& oFileSystem);
	void Export(string sFileName, ILoader::IRessourceInfos& ri);


private:
	IGeometryManager&	m_oGeometryManager;
};