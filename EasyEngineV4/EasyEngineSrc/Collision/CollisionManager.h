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
class ISegment2D;
class IEntity;

class CCollisionManager : public ICollisionManager
{
	IRenderer&				m_oRenderer;
	ILoaderManager&			m_oLoaderManager;
	IFileSystem*			m_pFileSystem;
	map< int, CHeightMap >	m_mHeigtMap;
	CVector					m_oOriginBackgroundColor;
	int						m_nHeightMapPrecision;
	IGeometryManager&		m_oGeometryManager;
	IEntity*				m_pScene;
	IMesh*					m_pGround;
	vector<IEntity*>		m_vCollideObjects;
	string					m_sCustomName;
	float					m_fCustomValue;
	IEntity*				m_pSphere;
	float					m_fGroundWidth;
	float					m_fGroundHeight;
	float					m_fScreenRatio;
	float					m_fMaxLenght;
	int						m_nScreenWidth;
	int						m_nScreenHeight;
	
	// Collision map
	float						m_fGroundMapWidth;
	float						m_fGroundMapHeight;
	float						m_fWorldToScreenScaleFactor;
	float						m_fGridCellSize;
	float						m_fGridHeight;
	char**						m_pCollisionGrid;
	vector<IEntity*>			m_vGridElements;
	ILoader::CTextureInfos		m_oCollisionMap;

	// temp
	IEntityManager*				m_pEntityManager;
	// fin temp
	
	
	static IMesh*			s_pMesh;
	
	static void		OnRenderHeightMap( IRenderer* pRenderer );

	static void OnRenderCollisionMapCallback(IRenderer*);
	void OnRenderCollisionMap();
	void GetOriginalShaders(const vector<IEntity*>& staticObjects, vector<IShader*>& vBackupStaticObjectShader);
	void SetCollisionShaders(const vector<IEntity*>& staticObjects, IShader* pCollisionShader);
	void RestoreOriginalShaders(const vector<IShader*>& vBackupStaticObjectShader, vector<IEntity*>& staticObjects);
	void RenderCollisionGeometry(IShader* pCollisionShader, const CMatrix& groundModel, const IBox* const pBox);
	bool IsSegmentInsideSegment(float fS1Center, float fS1Radius, float fS2Center, float fS2Radius);
	void ComputeGroundMapDimensions(IMesh* pMesh, float& width, float& height, float& groundToScreenScaleFactor);

	// Collision map
	void MarkBox(int row, int column, float r, float g, float b, IEntityManager* pEntityManager);
	void MarkMapBox(int iRow, int iColumn, int r, int g, int b);
	void MarkObstacles(IEntityManager* pEntityManager);
	void GetCellCoordFromPosition(float x, float y, int& row, int& column);
	void GetPositionFromCellCoord(int row, int column, float& x, float& y);
	void ComputeRowAndColumnCount(int& rowCount, int& columnCount);
	bool TestCellObstacle(int iRow, int iColumn);
	float WorldToPixel(float worldLenght);
	void ConvertLinearToCoord(int pixelNumber, int& x, int& y);

	// Path finding
	void FindPath(float fromX, float fromY, float toX, float toY, IEntityManager* pEntityManager);

public:	

	CCollisionManager( const Desc& oDesc );
	void	CreateHeightMap( IMesh* pGround, ILoader::CTextureInfos& ti , IRenderer::TPixelFormat format = IRenderer::T_RGB );	
	int		LoadHeightMap( string sFileName, IMesh* pMesh );
	void	LoadHeightMap( string sFileName, vector< vector< unsigned char > >& vPixels  );
	float	GetMapHeight( int nHeightMapID, float x, float z );
	void 	DisplayHeightMap( IMesh* pMesh );
	void 	StopDisplayHeightMap();
	void 	ExtractHeightMapFromTexture( string sFileName, string sOutFileName);
	void	SetHeightMapPrecision( int nPrecision );
	bool	IsIntersection( const IBox& b, const ISphere& s );
	bool	IsIntersection( const ISegment& s, const IBox& b2 );
	bool	IsIntersection( const ISegment& s, const CVector& oCircleCenter, float fCircleRadius );
	void	Get2DLineIntersection( const ISegment2D& pSeg1, const ISegment2D& pSeg2, CVector2D& oIntersection );
	void	Get2DLineIntersection( const CVector2D& oLine1First, const CVector2D& oLine1Last, const CVector2D& oLine2First, const CVector2D& oLine2Last, CVector2D& oIntersection );
	bool	IsSegmentRectIntersect( const ISegment2D& s, float fRectw, float fRecth, const CMatrix2X2& oRectTM );
	bool	IsSegmentRectIntersect( const CVector2D& S1, const CVector2D& S2, float fRectw, float fRecth, const CMatrix2X2& oRectTM );

	// Collision map
	void	CreateCollisionMap(ILoader::CTextureInfos& ti, vector<IEntity*> collides, IEntity* pScene, IRenderer::TPixelFormat format = IRenderer::T_RGB);
	void	LoadCollisionMap(string sFileName, IEntity* pScene);
	void	SendCustomUniformValue(string name, float value);
	void	DisplayCollisionMap();
	void	StopDisplayCollisionMap();
	void	DisplayGrid();

	// temp
	void	Test(IEntityManager* pEntityManager);
	void	Test2();
	void	SetEntityManager(IEntityManager* pEntityManager);
};

extern "C" _declspec(dllexport) CCollisionManager* CreateCollisionManager( const CCollisionManager::Desc& oDesc );