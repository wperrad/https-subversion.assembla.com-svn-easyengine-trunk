#include "Loader.h"

class CMatrix;
class CQuaternion;

class CBKELoader : public ILoader
{
	IFileSystem&	m_oFileSystem;
	void		ExportAscii( const std::string& sFileName, const CChunk& oChunk );
	void		ExportAscii( const std::string& sFileName, const ILoader::CAnimationInfos& oInfos );

public:

	CBKELoader( IFileSystem& oFileSystem );
	void		Load( string sFileName, ILoader::IRessourceInfos& ri, IFileSystem& );
	void		Export( const std::string& sFileName, const CChunk& chunk);
	void		Export( string sFileName, ILoader::IRessourceInfos& ri );
};