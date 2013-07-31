#include "EEPlugin.h"
#include "IRenderer.h"
#include "ILoader.h"

// stl
#include <vector>

using namespace std;

class IMesh;
class IRenderer;
class ILoaderManager;
class CBox;
class ISphere;
class IGeometryManager;

class ICollisionManager : public CPlugin
{
public:
	struct Desc : public CPlugin::Desc
	{
		IRenderer&			m_oRenderer;
		ILoaderManager&		m_oLoaderManager;
		IFileSystem*		m_pFileSystem;
		IGeometryManager&	m_oGeometryManager;
		Desc( IRenderer& oRenderer, ILoaderManager& oLoaderManager, IGeometryManager& oGeometryManager ) : 
			CPlugin::Desc( NULL, "" ), 
			m_oRenderer( oRenderer ),
			m_oLoaderManager( oLoaderManager ),
			m_pFileSystem( NULL ),
			m_oGeometryManager( oGeometryManager ){}
	};
	ICollisionManager( const Desc& oDesc ) : CPlugin( NULL, "" ){}
	virtual void	CreateHeightMap( IMesh* pMesh, ILoader::CTextureInfos& ti, IRenderer::TPixelFormat format = IRenderer::T_RGB ) = 0;
	virtual void	LoadHeightMap( string sFileName, vector< vector< unsigned char > >& vPixels  ) = 0;
	virtual int		LoadHeightMap( string sFileName, IMesh* pMesh ) = 0;
	virtual float	GetMapHeight( int nHeightMapID, float xModel, float yModel ) = 0;
	virtual void	DisplayHeightMap( IMesh* pMesh ) = 0;
	virtual void	StopDisplayHeightMap() = 0;
	virtual void	ExtractHeightMapFromTexture( string sFileName, string sOutFileName ) = 0;
	virtual	void	SetHeightMapPrecision( int nPrecision ) = 0;
	virtual bool	IsIntersection( const IBox& b, const ISphere& s ) = 0;
	virtual bool	IsIntersection( const IBox& b1, const IBox& b2 ) = 0;
};