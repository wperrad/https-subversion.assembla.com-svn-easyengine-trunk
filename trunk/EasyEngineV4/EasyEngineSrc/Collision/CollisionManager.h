// stl
#include <string>
#include <vector>

#include "ICollisionManager.h"

using namespace std;

class IMesh;
class IRenderer;
class ILoaderManager;
class CHeightMap;
class IFileSystem;
class CBox;
class ISphere;

class CCollisionManager : public ICollisionManager
{
	IRenderer&				m_oRenderer;
	ILoaderManager&			m_oLoaderManager;
	IFileSystem*			m_pFileSystem;
	map< int, CHeightMap >	m_mHeigtMap;
	CVector					m_oOriginBackgroundColor;
	int						m_nHeightMapPrecision;
	IGeometryManager&		m_oGeometryManager;

	bool			IsSegmentInsideSegment( float fS1Center, float fS1Radius, float fS2Center, float fS2Radius );
	bool			TestBoxesCollisionIntoFirstBoxBase( const IBox& oB1, const IBox& oB2 );
	static IMesh*	s_pMesh;
	static void		Update( IRenderer* pRenderer );

public:	

	CCollisionManager( const Desc& oDesc );
	//void CreateHeightMap( IMesh* pMesh, vector< unsigned char >& vPixels, IRenderer::TPixelFormat format = IRenderer::T_RGB );
	void	CreateHeightMap( IMesh* pMesh, ILoader::CTextureInfos& ti , IRenderer::TPixelFormat format = IRenderer::T_RGB );
	int		LoadHeightMap( string sFileName, IMesh* pMesh );
	void	LoadHeightMap( string sFileName, vector< vector< unsigned char > >& vPixels  );
	float	GetMapHeight( int nHeightMapID, float x, float z );
	void 	DisplayHeightMap( IMesh* pMesh );
	void 	StopDisplayHeightMap();
	void 	ExtractHeightMapFromTexture( string sFileName, string sOutFileName);
	void	SetHeightMapPrecision( int nPrecision );
	bool	IsIntersection( const IBox& b, const ISphere& s );
	bool	IsIntersection( const IBox& b1, const IBox& b2 );
	bool	IsIntersection( const ISegment& s, const IBox& b2 );
	bool	IsIntersection( const ISegment& s, const CVector& oCircleCenter, float fCircleRadius );
};

extern "C" _declspec(dllexport) CCollisionManager* CreateCollisionManager( const CCollisionManager::Desc& oDesc );