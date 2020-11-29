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
class ISegment2D;
class ISegment;
class IEntity;
class IEntityManager;

class ICollisionManager : public CPlugin
{
public:
	struct Desc : public CPlugin::Desc
	{
		IRenderer&			m_oRenderer;
		ILoaderManager&		m_oLoaderManager;
		IFileSystem*		m_pFileSystem;
		IGeometryManager&	m_oGeometryManager;
		Desc( IRenderer& oRenderer, ILoaderManager& oLoaderManager, IGeometryManager& oGeometryManager) :
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
	virtual bool	IsIntersection( const ISegment& s, const IBox& b2 ) = 0;
	virtual bool	IsIntersection( const ISegment& s, const CVector& oCircleCenter, float fCircleRadius ) = 0;
	virtual void	Get2DLineIntersection( const CVector2D& oLine1First, const CVector2D& oLine1Last, const CVector2D& oLine2First, const CVector2D& oLine2Last, CVector2D& oIntersection ) = 0;
	virtual bool	IsSegmentRectIntersect( const ISegment2D& s, float fRectw, float fRecth, const CMatrix2X2& oSquareTM ) = 0;
	virtual bool	IsSegmentRectIntersect( const CVector2D& S1, const CVector2D& S2, float fRectw, float fRecth, const CMatrix2X2& oRectTM ) = 0;
	virtual void	SendCustomUniformValue(string name, float value) = 0;
	
	// Collision map
	virtual void	DisplayCollisionMap() = 0;
	virtual void	StopDisplayCollisionMap() = 0;
	virtual void	CreateCollisionMap(ILoader::CTextureInfos& ti, vector<IEntity*> collides, IEntity* pScene, IRenderer::TPixelFormat format = IRenderer::T_RGB) = 0;
	virtual void	LoadCollisionMap(string sFileName, IEntity* pScene) = 0;
	virtual void	DisplayGrid() = 0;
	virtual void	ComputeRowAndColumnCount(int& rowCount, int& columnCount) = 0;
	virtual void	GetCellCoordFromPosition(float x, float y, int& row, int& column) = 0;
	virtual void	GetPositionFromCellCoord(int row, int column, float& x, float& y) = 0;
	virtual bool	TestCellObstacle(int iRow, int iColumn) = 0;

	// temp
	virtual void	Test(IEntityManager* pEntityManager) = 0;
	virtual void	Test2() = 0;
	virtual void	SetEntityManager(IEntityManager* pEntityManager) = 0;
};