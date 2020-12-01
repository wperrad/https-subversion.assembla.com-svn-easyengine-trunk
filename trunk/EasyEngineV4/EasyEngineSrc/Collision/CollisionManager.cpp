#include "CollisionManager.h"
#include "IRessource.h"
#include "IShader.h"
#include "ILoader.h"
#include "HeightMap.h"
#include "IGeometry.h"
#include "Utils2/RenderUtils.h"
#include "IEntity.h"


IMesh* CCollisionManager::s_pMesh = NULL;

CCollisionManager* g_pCurrentCollisionManager = NULL;

CCollisionManager::CCollisionManager( const ICollisionManager::Desc& oDesc ):
ICollisionManager( oDesc ),
m_oRenderer( oDesc.m_oRenderer ),
m_oLoaderManager( oDesc.m_oLoaderManager ),
m_nHeightMapPrecision( 1 ),
m_pFileSystem( oDesc.m_pFileSystem ),
m_oGeometryManager( oDesc.m_oGeometryManager ),
m_pGround(NULL),
m_sCustomName("a"),
m_fCustomValue(0.f),
m_fGroundWidth(0),
m_fGroundHeight(0),
m_fGridCellSize(500.f),
m_fGridHeight(800.f),
m_pCollisionGrid(NULL),
m_pScene(NULL),
m_fScreenRatio(m_fScreenRatio),
m_pEntityManager(NULL)
{
	g_pCurrentCollisionManager = this;
	m_oRenderer.GetResolution(m_nScreenWidth, m_nScreenHeight);
	m_fScreenRatio = (float)m_nScreenWidth / (float)m_nScreenHeight;
	
}

void CCollisionManager::DisplayHeightMap(IMesh* pMesh)
{
	m_oRenderer.GetBackgroundColor(m_oOriginBackgroundColor);
	ILoader::CTextureInfos ti;
	CreateHeightMap(pMesh, ti);
	s_pMesh = pMesh;
	m_oRenderer.AbonneToRenderEvent(OnRenderHeightMap);
}

void CCollisionManager::StopDisplayHeightMap()
{
	m_oRenderer.DesabonneToRenderEvent(OnRenderHeightMap);
	m_oRenderer.SetBackgroundColor(m_oOriginBackgroundColor.m_x, m_oOriginBackgroundColor.m_y, m_oOriginBackgroundColor.m_z);
}

void CCollisionManager::DisplayCollisionMap()
{
	m_oRenderer.AbonneToRenderEvent(OnRenderCollisionMapCallback);
}

void CCollisionManager::StopDisplayCollisionMap()
{
	m_oRenderer.DesabonneToRenderEvent(OnRenderCollisionMapCallback);
}

void CCollisionManager::OnRenderCollisionMapCallback(IRenderer*)
{
	g_pCurrentCollisionManager->OnRenderCollisionMap();
}

void CCollisionManager::RenderCollisionGeometry(IShader* pCollisionShader, const CMatrix& groundModel, const IBox* const pBox)
{
	pCollisionShader->SendUniformValues("h", pBox->GetDimension().m_y);
	pCollisionShader->SendUniformValues("zMin", pBox->GetMinPoint().m_y);
	pCollisionShader->SendUniformValues("scale", m_fWorldToScreenScaleFactor);
	pCollisionShader->SendUniformMatrix4("modelMatrix", groundModel, true);
	pCollisionShader->SendUniformValues("isGround", 1.f);
	pCollisionShader->SendUniformValues("isGrid", 0.f);
	pCollisionShader->SendUniformValues(m_sCustomName, m_fCustomValue);

	m_pGround->Update();

	pCollisionShader->SendUniformValues("isGround", 0.f);
	for (vector<IEntity*>::iterator it = m_vCollideObjects.begin(); it != m_vCollideObjects.end(); it++) {
		const CMatrix& model = (*it)->GetWorldMatrix();
		pCollisionShader->SendUniformMatrix4("modelMatrix", model, true);
		(*it)->Update();
	}

	pCollisionShader->SendUniformValues("isGround", 0.f);
	pCollisionShader->SendUniformValues("isGrid", 1.f);
	for (vector<IEntity*>::iterator it = m_vGridElements.begin(); it != m_vGridElements.end(); it++) {
		const CMatrix& model = (*it)->GetWorldMatrix();
		pCollisionShader->SendUniformMatrix4("modelMatrix", model, true);
		(*it)->Update();
	}
	
}

void CCollisionManager::OnRenderCollisionMap()
{
	IShader* pOrgShader = m_pGround->GetCurrentShader();
	IShader* pCollisionShader = m_oRenderer.GetShader("collision");
	pCollisionShader->Enable(true);
	m_pGround->SetShader(pCollisionShader);

	CMatrix model, oProj;
	int nWidth, nHeight;
	m_oRenderer.GetResolution(nWidth, nHeight);
	oProj.m_00 = (float)nHeight / (float)nWidth;

	vector<IShader*> vBackStaticObjectShader;
	vector<IShader*> vBackLineShader;

	GetOriginalShaders(m_vCollideObjects, vBackStaticObjectShader);
	SetCollisionShaders(m_vCollideObjects, pCollisionShader);

	GetOriginalShaders(m_vGridElements, vBackLineShader);
	SetCollisionShaders(m_vGridElements, pCollisionShader);

	CMatrix oBakProj;
	m_oRenderer.GetProjectionMatrix(oBakProj);
	m_oRenderer.SetProjectionMatrix(oProj);

	bool wasCullingEnabled = m_oRenderer.IsCullingEnabled();
	if (wasCullingEnabled)
		m_oRenderer.CullFace(false);
	m_oRenderer.EnableDepthTest(false);

	RenderCollisionGeometry(pCollisionShader, model, m_pGround->GetBBox());

	m_oRenderer.EnableDepthTest(true);

	if (wasCullingEnabled)
		m_oRenderer.CullFace(true);

	pCollisionShader->Enable(false);
	m_pGround->SetShader(pOrgShader);

	RestoreOriginalShaders(vBackStaticObjectShader, m_vCollideObjects);
	RestoreOriginalShaders(vBackLineShader, m_vGridElements);

	m_oRenderer.SetProjectionMatrix(oBakProj);
}

void CCollisionManager::ComputeGroundMapDimensions()
{
	IBox* pBox = m_pGround->GetBBox();
	if (pBox->GetDimension().m_x <= pBox->GetDimension().m_z * m_fScreenRatio)
	{
		m_fGroundMapWidth = (pBox->GetDimension().m_x * (float)m_nScreenHeight) / pBox->GetDimension().m_z;
		m_fGroundMapHeight = (float)m_nScreenHeight;
		m_fWorldToScreenScaleFactor = pBox->GetDimension().m_z / 2.f;
	}
	else
	{
		m_fGroundMapHeight = (float)m_nScreenWidth;
		m_fGroundMapHeight = pBox->GetDimension().m_z * (float)m_nScreenWidth / pBox->GetDimension().m_x;
		m_fWorldToScreenScaleFactor = pBox->GetDimension().m_x / 2.f;
	}
	m_fGroundMapWidth = ((int)m_fGroundMapWidth / 4) * 4;
	m_fGroundMapHeight = ((int)m_fGroundMapHeight / 4) * 4;
}

void CCollisionManager::CreateCollisionMap(ILoader::CTextureInfos& ti, vector<IEntity*> collides, IEntity* pScene, IRenderer::TPixelFormat format)
{
	m_pScene = pScene;

	m_pGround = dynamic_cast<IMesh*>(pScene->GetRessource());
	if (!m_pGround) {
		CEException e("Erreur, aucun mesh défini pour la scene");
		throw e;
	}
	
	IShader* pOrgShader = m_pGround->GetCurrentShader();
	IShader* pCollisionShader = m_oRenderer.GetShader("collision");
	pCollisionShader->Enable(true);
	m_pGround->SetShader(pCollisionShader);

	CMatrix model, oProj;
	oProj.m_00 = 1.f / m_fScreenRatio;
	const IBox* pBox = m_pGround->GetBBox();

	ComputeGroundMapDimensions();
	int subdivisionCount = 30;

	int cellMapSize = m_fGroundMapWidth / subdivisionCount;

	float fOriginMapX = ((float)m_nScreenWidth - m_fGroundMapWidth) / 2.f;
	float fOriginMapY = ((float)m_nScreenHeight - m_fGroundMapHeight) / 2.f;

	m_fGroundWidth = pBox->GetDimension().m_x;
	m_fGroundHeight = pBox->GetDimension().m_z;

	m_vCollideObjects = collides;

	vector<IShader*> vBackStaticObjectShader;
	vector<IShader*> vBackLineShader;

	GetOriginalShaders(m_vCollideObjects, vBackStaticObjectShader);
	SetCollisionShaders(m_vCollideObjects, pCollisionShader);

	GetOriginalShaders(m_vGridElements, vBackLineShader);
	SetCollisionShaders(m_vGridElements, pCollisionShader);

	CMatrix oBakProj;
	m_oRenderer.GetProjectionMatrix(oBakProj);
	m_oRenderer.SetProjectionMatrix(oProj);

	bool wasCullingEnabled = m_oRenderer.IsCullingEnabled();
	if (wasCullingEnabled)
		m_oRenderer.CullFace(false);
	m_oRenderer.EnableDepthTest(false);

	m_oRenderer.BeginRender();

	RenderCollisionGeometry(pCollisionShader, model, pBox);

	m_oRenderer.ReadPixels(fOriginMapX, fOriginMapY, m_fGroundMapWidth, m_fGroundMapHeight, ti.m_vTexels, format);
	ti.m_nWidth = (int)m_fGroundMapWidth;
	ti.m_nHeight = (int)m_fGroundMapHeight;

	m_oRenderer.EndRender();

	m_oRenderer.EnableDepthTest(true);

	if (wasCullingEnabled)
		m_oRenderer.CullFace(true);

	pCollisionShader->Enable(false);
	m_pGround->SetShader(pOrgShader);

	RestoreOriginalShaders(vBackStaticObjectShader, m_vCollideObjects);
	RestoreOriginalShaders(vBackLineShader, m_vGridElements);

	m_oRenderer.SetProjectionMatrix(oBakProj);
}

void CCollisionManager::LoadCollisionMap(string sFileName, IEntity* pScene)
{
	m_oLoaderManager.Load(sFileName, m_oCollisionMap);
	IMesh* pGround = dynamic_cast<IMesh*>(pScene->GetRessource());
	m_pGround = pGround;
	m_pScene = pScene;
	if (m_pGround) {
		IBox* pBox = m_pGround->GetBBox();
		m_fGroundWidth = pBox->GetDimension().m_x;
		m_fGroundHeight = pBox->GetDimension().m_z;
		ComputeGroundMapDimensions();
	}
}

void CCollisionManager::SetHeightMapPrecision( int nPrecision )
{
	m_nHeightMapPrecision = nPrecision;
}

void CCollisionManager::CreateHeightMap( IMesh* pGround, ILoader::CTextureInfos& ti, IRenderer::TPixelFormat format )
{
	if (!m_pGround)
		m_pGround = pGround;
 	IShader* pOrgShader = pGround->GetCurrentShader();
	IShader* pHMShader = m_oRenderer.GetShader("hm");
	pHMShader->Enable(true);
	pGround->SetShader(pHMShader);

	CMatrix oModelView, oProj;
	int nWidth, nHeight;
	m_oRenderer.GetResolution(nWidth, nHeight);
	float fScreenRatio = (float)nWidth / (float)nHeight;
	oProj.m_00 = 1.f / fScreenRatio;
	const IBox* pBox = pGround->GetBBox();
	float maxLenght = pBox->GetDimension().m_x;
	if (maxLenght < pBox->GetDimension().m_z)
		maxLenght = pBox->GetDimension().m_z;
	
	ComputeGroundMapDimensions();
	float fOriginMapX = ((float)nWidth - m_fGroundMapWidth) / 2.f;
	float fOriginMapY = ((float)nHeight - m_fGroundMapHeight) / 2.f;

	pHMShader->SendUniformValues("h", pBox->GetDimension().m_y);
	pHMShader->SendUniformValues("zMin", pBox->GetMinPoint().m_y);
	pHMShader->SendUniformValues("scale", m_fWorldToScreenScaleFactor);

	CMatrix oBakProj;
	m_oRenderer.GetProjectionMatrix(oBakProj);
	m_oRenderer.SetProjectionMatrix(oProj);

	m_oRenderer.CullFace(false);
	m_oRenderer.BeginRender();
	pGround->Update();

	m_oRenderer.ReadPixels(fOriginMapX, fOriginMapY, m_fGroundMapWidth, m_fGroundMapHeight, ti.m_vTexels, format);
	ti.m_nWidth = (int)m_fGroundMapWidth;
	ti.m_nHeight = (int)m_fGroundMapHeight;
	m_oRenderer.EndRender();
	m_oRenderer.CullFace(true);

	pHMShader->Enable(false);
	pGround->SetShader(pOrgShader);
	m_oRenderer.SetProjectionMatrix(oBakProj);
}

void CCollisionManager::GetOriginalShaders(const vector<IEntity*>& staticObjects, vector<IShader*>& vBackupStaticObjectShader)
{
	for (vector<IEntity*>::const_iterator it = staticObjects.begin(); it != staticObjects.end(); it++) {
		IMesh* pMesh = dynamic_cast<IMesh*>((*it)->GetRessource());
		if (pMesh)
			vBackupStaticObjectShader.push_back(pMesh->GetCurrentShader());
	}
}

void CCollisionManager::SetCollisionShaders(const vector<IEntity*>& staticObjects, IShader* pCollisionShader)
{
	for (vector<IEntity*>::const_iterator it = staticObjects.begin(); it != staticObjects.end(); it++) {
		(*it)->SetShader(pCollisionShader);
	}
}

void CCollisionManager::RestoreOriginalShaders(const vector<IShader*>& vBackupStaticObjectShader, vector<IEntity*>& staticObjects)
{
	int i = 0;
	for (vector<IEntity*>::iterator it = staticObjects.begin(); it != staticObjects.end(); it++) {
		(*it)->SetShader(vBackupStaticObjectShader[i++]);
	}
}

void CCollisionManager::OnRenderHeightMap( IRenderer* pRenderer )
{
	pRenderer->SetBackgroundColor( 0, 0, 255 );

	IShader* pOrgShader = s_pMesh->GetCurrentShader();
	IShader* pHMShader = pRenderer->GetShader( "hm" );
	pHMShader->Enable( true );
	s_pMesh->SetShader( pHMShader );
	
	CMatrix oModelView, oProj;
	int nWidth, nHeight;
	pRenderer->GetResolution( nWidth, nHeight );
	float fScreenRatio = (float)nWidth / (float)nHeight;
	oProj.m_00 = 1.f / fScreenRatio;
	const IBox* pBox = s_pMesh->GetBBox();
	float maxLenght = pBox->GetDimension().m_x ;
	if( maxLenght < pBox->GetDimension().m_y )
		maxLenght = pBox->GetDimension().m_y;
	float scale = ( maxLenght / 2.f );
	pHMShader->SendUniformValues( "h", pBox->GetDimension().m_z );
	pHMShader->SendUniformValues( "zMin", pBox->GetMinPoint().m_z );
	pHMShader->SendUniformValues( "zMax", pBox->GetMinPoint().m_z + pBox->GetDimension().m_z );
	pHMShader->SendUniformValues( "scale", scale );

	pRenderer->SetModelViewMatrix( oModelView );
	CMatrix oBakProj;
	pRenderer->GetProjectionMatrix( oBakProj );
	pRenderer->SetProjectionMatrix( oProj );

	s_pMesh->Update();

	pHMShader->Enable( false );
	s_pMesh->SetShader( pOrgShader );
	pRenderer->SetProjectionMatrix( oBakProj );
}

int CCollisionManager::LoadHeightMap( string sFileName, IMesh* pMesh  )
{
	IBox* pBox = pMesh->GetBBox();
	CHeightMap hm( sFileName, m_oLoaderManager, *pBox, m_oGeometryManager );
	hm.SetPrecision( m_nHeightMapPrecision );
	int nID = (int)m_mHeigtMap.size();
	m_mHeigtMap[ nID ] = hm;
	return nID;
}

void CCollisionManager::LoadHeightMap(string sFileName, vector< vector< unsigned char > >& vPixels)
{
	ILoader::CTextureInfos ti;
	m_oLoaderManager.Load(sFileName, ti);
	vPixels.clear();
	vPixels.resize(ti.m_nWidth);
	for (int i = 0; i < ti.m_nWidth; i++)
		for (int j = 0; j < ti.m_nHeight; j++)
			vPixels[i].push_back(ti.m_vTexels[j * (ti.m_nWidth + 1)]);
}

float CCollisionManager::GetMapHeight( int nHeightMapID, float xModel, float zModel )
{
	map< int, CHeightMap >::iterator itMap = m_mHeigtMap.find( nHeightMapID );
	float fInterpolate = itMap->second.GetHeight( xModel, zModel );
	return fInterpolate;
}

void CCollisionManager::ExtractHeightMapFromTexture( string sFileName, string sOutFileName)
{
	CHeightMap::ExtractHeightMapFromTexture( sFileName, m_oLoaderManager, m_pFileSystem, sOutFileName );
}

bool CCollisionManager::IsSegmentInsideSegment( float fS1Center, float fS1Radius, float fS2Center, float fS2Radius )
{
	return ( ( fS1Center + fS1Radius ) > ( fS2Center - fS2Radius ) ) && ( ( fS1Center - fS1Radius ) < ( fS2Center + fS2Radius ) );
}

bool CCollisionManager::IsIntersection( const IBox& b, const ISphere& s )
{
	CMatrix invBoxWorldMatrix, oWorldMatrix;
	b.GetWorldMatrix( oWorldMatrix );
	oWorldMatrix.GetInverse( invBoxWorldMatrix );
	CVector vBoxBaseSphereCenter = invBoxWorldMatrix * s.GetCenter();
	bool bInsideX = IsSegmentInsideSegment( 0, b.GetDimension().m_x / 2.f, vBoxBaseSphereCenter.m_x, s.GetRadius() ); //vBoxBaseSphereCenter.m_x - s.GetRadius() < b.GetDimension().m_x / 2.f && vBoxBaseSphereCenter.m_x + s.GetRadius() > - b.GetDimension().m_x / 2.f;
	bool bInsideY = IsSegmentInsideSegment( 0, b.GetDimension().m_y / 2.f, vBoxBaseSphereCenter.m_y, s.GetRadius() );
	bool bInsideZ = IsSegmentInsideSegment( 0, b.GetDimension().m_z / 2.f, vBoxBaseSphereCenter.m_z, s.GetRadius() );
	return ( bInsideX && bInsideY && bInsideZ );
}

float GetMinx( const vector< CVector >& vPoints )
{
	float fMin = vPoints[ 0 ].m_x;
	for( int i = 1; i < vPoints.size(); i++ )
	{
		if( fMin > vPoints[ i ].m_x )
			fMin = vPoints[ i ].m_x;
	}
	return fMin;
}

float GetMiny( const vector< CVector >& vPoints )
{
	float fMin = vPoints[ 0 ].m_y;
	for( int i = 1; i < vPoints.size(); i++ )
	{
		if( fMin > vPoints[ i ].m_y )
			fMin = vPoints[ i ].m_y;
	}
	return fMin;
}

float GetMinz( const vector< CVector >& vPoints )
{
	float fMin = vPoints[ 0 ].m_z;
	for( int i = 1; i < vPoints.size(); i++ )
	{
		if( fMin > vPoints[ i ].m_z )
			fMin = vPoints[ i ].m_z;
	}
	return fMin;
}

float GetMaxx( const vector< CVector >& vPoints )
{
	float fMax = vPoints[ 0 ].m_x;
	for( int i = 1; i < vPoints.size(); i++ )
	{
		if( fMax < vPoints[ i ].m_x )
			fMax = vPoints[ i ].m_x;
	}
	return fMax;
}

float GetMaxy( const vector< CVector >& vPoints )
{
	float fMax = vPoints[ 0 ].m_y;
	for( int i = 1; i < vPoints.size(); i++ )
	{
		if( fMax < vPoints[ i ].m_y )
			fMax = vPoints[ i ].m_y;
	}
	return fMax;
}

float GetMaxz( const vector< CVector >& vPoints )
{
	float fMax = vPoints[ 0 ].m_z;
	for( int i = 1; i < vPoints.size(); i++ )
	{
		if( fMax < vPoints[ i ].m_z )
			fMax = vPoints[ i ].m_z;
	}
	return fMax;
}

bool CCollisionManager::TestBoxesCollisionIntoFirstBoxBase( const IBox& b1, const IBox& b2 )
{
	CMatrix b1Mat, b2Mat;
	b1.GetWorldMatrix( b1Mat );
	b2.GetWorldMatrix( b2Mat );
	CMatrix b1MatInv;
	b1Mat.GetInverse( b1MatInv );
 	CMatrix b2MatBaseB1 = b1MatInv * b2Mat;
	IBox* pB2Temp = m_oGeometryManager.CreateBox( b2 );
	pB2Temp->SetWorldMatrix( b2MatBaseB1 );
	vector< CVector > vPoints2;
	pB2Temp->GetPoints( vPoints2 );
	float fMinx = GetMinx( vPoints2 );
	if( fMinx > b1.GetMinPoint().m_x + b1.GetDimension().m_x )
		return false;
	float fMiny = GetMiny( vPoints2 );
	if( fMiny > b1.GetMinPoint().m_y + b1.GetDimension().m_y )
		return false;
	float fMinz = GetMinz( vPoints2 );
	if( fMinz > b1.GetMinPoint().m_z + b1.GetDimension().m_z )
		return false;
	float fMaxx = GetMaxx( vPoints2 );
	if( fMaxx < b1.GetMinPoint().m_x )
		return false;
	float fMaxy = GetMaxy( vPoints2 );
	if( fMaxy < b1.GetMinPoint().m_y )
		return false;
	float fMaxz = GetMaxz( vPoints2 );
	if( fMaxz < b1.GetMinPoint().m_z )
		return false;
	return true;
}

bool CCollisionManager::IsIntersection( const IBox& b1, const IBox& b2 )
{
	if( TestBoxesCollisionIntoFirstBoxBase( b1, b2 ) )
		return TestBoxesCollisionIntoFirstBoxBase( b2, b1 );
	return false;
}

bool CCollisionManager::IsIntersection( const ISegment& s, const IBox& b2 )
{
	return true;
}

bool CCollisionManager::IsIntersection( const ISegment& s, const CVector& oCircleCenter, float fCircleRadius )
{
	CVector H;
	s.ComputeProjectedPointOnLine( oCircleCenter, H );

	return false;
}

void CCollisionManager::Get2DLineIntersection( const CVector2D& oLine1First, const CVector2D& oLine1Last, const CVector2D& oLine2First, const CVector2D& oLine2Last, CVector2D& oIntersection )
{
	ISegment2D* pL1 = m_oGeometryManager.CreateSegment2D( oLine1First, oLine1Last );
	ISegment2D* pL2 = m_oGeometryManager.CreateSegment2D( oLine2First, oLine2Last );
	Get2DLineIntersection( *pL1, *pL2, oIntersection );
}

void CCollisionManager::Get2DLineIntersection( const ISegment2D& oSeg1, const ISegment2D& oSeg2, CVector2D& oIntersection )
{
	float a1, b1, c1, a2, b2, c2;
	oSeg1.ComputeLineEquation( a1, b1, c1 );
	oSeg2.ComputeLineEquation( a2, b2, c2 );
	oIntersection.m_x = 1 / a1 * ( b1 * ( (a2 * c1 - a1 * c2 ) / ( a2 * b1 - a1 * b2 )  ) - c1 );
	oIntersection.m_y = ( a1 * c2 - a2 * c1 ) / ( a2 * b1 - a1 * b2 );
}

bool CCollisionManager::IsSegmentRectIntersect( const ISegment2D& s, float fRectw, float fRecth, const CMatrix2X2& oRectTM )
{
	CVector2D S1, S2;
	s.GetPoints( S1, S2 );
	return IsSegmentRectIntersect( S1, S2, fRectw, fRecth, oRectTM );

}

bool CCollisionManager::IsSegmentRectIntersect( const CVector2D& S1, const CVector2D& S2, float fRectw, float fRecth, const CMatrix2X2& oRectTM )
{
	CVector2D x( 1, 0 );

	CVector2D R0 = CVector2D( -fRectw / 2.f, -fRecth / 2.f );
	CVector2D R1 = CVector2D( fRectw / 2.f, -fRecth / 2.f );
	CVector2D R2 = CVector2D( fRectw / 2.f, fRecth / 2.f );
	CVector2D R3 = CVector2D( -fRectw / 2.f, fRecth / 2.f );
	CMatrix2X2 oRectInvTM;
	oRectTM.GetInverse( oRectInvTM );
	
	CVector2D S1Inv = oRectInvTM * S1;
	CVector2D S2Inv = oRectInvTM * S2;

	float fMinx = R0.m_x, fMaxx = R1.m_x, fMinz = R0.m_y, fMaxz = R2.m_y;
	float fSegMinx = S1Inv.m_x;
	if( fSegMinx > S2Inv.m_x ) fSegMinx = S2Inv.m_x;
	if( fSegMinx > fMaxx )
		return false;
	float fSegMaxx = S1Inv.m_x;
	if( fSegMaxx < S2Inv.m_x ) fSegMaxx = S2Inv.m_x;
	if( fSegMaxx < fMinx )
		return false;
	float fSegMinz = S1Inv.m_y;
	if( fSegMinz > S2Inv.m_y ) fSegMinz = S2Inv.m_y;
	if( fSegMinz > fMaxz )
		return false;
	float fSegMaxz = S1Inv.m_y;
	if( fSegMaxz < S2Inv.m_y ) fSegMaxz = S2Inv.m_y;
	if( fSegMaxz < fMinz )
		return false;

	CVector2D R0tm = oRectTM * CVector2D( -fRectw / 2.f, -fRecth / 2.f );
	CVector2D R1tm = oRectTM * CVector2D( fRectw / 2.f, -fRecth / 2.f );
	CVector2D R2tm = oRectTM * CVector2D( fRectw / 2.f, fRecth / 2.f );
	CVector2D R3tm = oRectTM * CVector2D( -fRectw / 2.f, fRecth / 2.f );

	float alpha = acosf( ( ( S2 - S1 ) * x ) / ( S2 - S1 ).Norm() ) * 180.f / 3.1415927f;
	CMatrix2X2 oSegTM = CMatrix2X2::GetRotation( alpha ), oSegTMInv;
	oSegTM.AddTranslation( S1 );
	oSegTM.GetInverse( oSegTMInv );

	CVector2D R0Inv = oSegTMInv * R0tm;
	CVector2D R1Inv = oSegTMInv * R1tm;
	CVector2D R2Inv = oSegTMInv * R2tm;
	CVector2D R3Inv = oSegTMInv * R3tm;

	CVector2D S3Inv = oSegTMInv * S2;

	fMinx = R0Inv.m_y;
	if( R1Inv.m_x < fMinx ) fMinx = R1Inv.m_x;
	if( R2Inv.m_x < fMinx ) fMinx = R2Inv.m_x;
	if( R3Inv.m_x < fMinx ) fMinx = R3Inv.m_x;
	if( fMinx > S3Inv.m_x )
		return false;

	fMaxx = R0Inv.m_y;
	if( R1Inv.m_x > fMaxx ) fMaxx = R1Inv.m_y;
	if( R2Inv.m_x > fMaxx ) fMaxx = R2Inv.m_y;
	if( R3Inv.m_x > fMaxx ) fMaxx = R3Inv.m_y;
	if( fMaxx < 0 )
		return false;

	fMinz = R0Inv.m_y;
	if( R1Inv.m_y < fMinz ) fMinz = R1Inv.m_y;
	if( R2Inv.m_y < fMinz ) fMinz = R2Inv.m_y;
	if( R3Inv.m_y < fMinz ) fMinz = R3Inv.m_y;
	if( fMinz > 0 )
		return false;

	fMaxz = R0Inv.m_y;
	if( R1Inv.m_y > fMaxz ) fMaxz = R1Inv.m_y;
	if( R2Inv.m_y > fMaxz ) fMaxz = R2Inv.m_y ;
	if( R3Inv.m_y > fMaxz ) fMaxz = R3Inv.m_y;
	if( fMaxz < 0 )
		return false;

	return true;
}

void CCollisionManager::SendCustomUniformValue(string name, float value)
{
	m_sCustomName = name;
	m_fCustomValue = value;
}

void CCollisionManager::DisplayGrid()
{
	CVector first, last;
	int rowCount = m_fGroundHeight / m_fGridCellSize + 1;
	int columnCount = m_fGroundWidth / m_fGridCellSize + 1;
	int w = m_fGroundWidth;
	int h = m_fGroundHeight;
	int s = m_fGridCellSize;
	if (w % s > 0)
		columnCount++;
	if (h % s > 0)
		rowCount++;

	float lineWidth = 15.f;
	float y = 1130.f;

	for (int i = 0; i < rowCount; i++) {
		first.m_x = -m_fGroundWidth / 2;
		first.m_z = -m_fGroundHeight / 2 + i * m_fGridCellSize;

		first.m_y = y;

		IEntity* line = m_pEntityManager->CreateCylinder(lineWidth, m_fGroundWidth);
		line->Link(m_pScene);
		line->SetWorldPosition(first);
		line->Roll(-90.f);
		line->LocalTranslate(0, m_fGroundWidth / 2, 0);
		line->Colorize(1, 0, 0, 1);
		m_vGridElements.push_back(line);
	}

	for (int i = 0; i < columnCount; i++) {
		first.m_x = -m_fGroundWidth / 2 + i * m_fGridCellSize;
		first.m_z = -m_fGroundHeight / 2;
		
		first.m_y = y;

		IEntity* line = m_pEntityManager->CreateCylinder(lineWidth, m_fGroundWidth);
		line->Link(m_pScene);
		line->SetWorldPosition(first);
		line->Pitch(90.f);
		line->LocalTranslate(0, m_fGroundWidth / 2, 0);
		m_vGridElements.push_back(line);
	}
}

void CCollisionManager::MarkBox(int row, int column, float r, float g, float b, IEntityManager* pEntityManager)
{
	float x, z;
	GetPositionFromCellCoord(row, column, x, z);
	IEntity* pSphere = pEntityManager->CreateSphere(m_fGridCellSize*3. / 8.);
	IShader* pColorShader = m_oRenderer.GetShader("color");
	pSphere->SetShader(pColorShader);
	pSphere->Colorize(r, g, b, 0.5f);
	pSphere->SetWorldPosition(x + m_fGridCellSize / 2, m_fGridHeight , z + m_fGridCellSize / 2);
	pSphere->Link(m_pScene);
	m_vGridElements.push_back(pSphere);
}

void CCollisionManager::MarkMapBox(int row, int column, int r, int g, int b)
{
	float x0, y0, x1, y1;
	GetPositionFromCellCoord(row, column, x0, y0);
	GetPositionFromCellCoord(row + 1, column + 1, x1, y1);

	float mapX0 = WorldToPixel(x0);
	float mapX1 = WorldToPixel(x1);

	int subdivisionCount = 30;
	float fmapBoxSize = mapX1 - mapX0;
	int mapBoxSize = fmapBoxSize;
	int rowCount = m_oCollisionMap.m_nHeight / mapBoxSize;
	int columnCount = m_oCollisionMap.m_nWidth / mapBoxSize;

	bool isObstacle = false;

	float fx = (float)column * fmapBoxSize;
	int ix = (int)fx;
	float fy = (float)row * fmapBoxSize;
	int iy = (int)fy;

	for (int i = 0; i < mapBoxSize; i++) {

		int index = 3 * (ix + i * m_oCollisionMap.m_nWidth + iy * m_oCollisionMap.m_nWidth);
		int fillSize = 3 * mapBoxSize;
		if (column == columnCount) {
			fillSize = (m_oCollisionMap.m_nWidth * 3) - column * 3 * mapBoxSize;
		}
		if (index < m_oCollisionMap.m_vTexels.size()) {
			for (int j = 0; j < fillSize / 3; j++) {
				m_oCollisionMap.m_vTexels[index + 3 * j] = b;
				m_oCollisionMap.m_vTexels[index + 3 * j + 1] = g;
				m_oCollisionMap.m_vTexels[index + 3 * j + 2] = r;
			}
		}
	}
}

void CCollisionManager::GetPositionFromCellCoord(int row, int column, float& x, float& y)
{
	int rowCount, columnCount;
	ComputeRowAndColumnCount(rowCount, columnCount);
	x = (0.5f + (float)column - (float)columnCount / 2.f) * m_fGridCellSize;
	y = (0.5f + (float)row - (float)rowCount / 2.f) * m_fGridCellSize;
}

void CCollisionManager::GetCellCoordFromPosition(float x, float y, int& row, int& column)
{
	row = (int)( (x + m_fGroundWidth / 2) / m_fGridCellSize);
	column = (int)( (y + m_fGroundHeight / 2) / m_fGridCellSize);
}

void CCollisionManager::ComputeRowAndColumnCount(int& rowCount, int& columnCount)
{
	rowCount = m_fGroundHeight / m_fGridCellSize;
	columnCount = m_fGroundWidth / m_fGridCellSize;
}

void CCollisionManager::ConvertLinearToCoord(int pixelNumber, int& x, int& y)
{
	x = pixelNumber / 3 - pixelNumber / (3 * m_oCollisionMap.m_nWidth) * m_oCollisionMap.m_nWidth;
	y = pixelNumber / (3 * m_oCollisionMap.m_nWidth);
}

bool CCollisionManager::TestCellObstacle(int row, int column)
{
	float x0, y0, x1, y1;
	GetPositionFromCellCoord(row, column, x0, y0);
	GetPositionFromCellCoord(row + 1, column + 1, x1, y1);

	float mapX0 = WorldToPixel(x0);
	float mapX1 = WorldToPixel(x1);

	int subdivisionCount = 30;
	int mapBoxSize = mapX1 - mapX0;
	float fmapBoxSize = mapX1 - mapX0;
	int rowCount = m_oCollisionMap.m_nHeight / mapBoxSize;
	int columnCount = m_oCollisionMap.m_nWidth / mapBoxSize;

	float fx = (float)column * fmapBoxSize;
	int ix = (int)fx;
	float fy = (float)row * fmapBoxSize;
	int iy = (int)fy;

	bool isObstacle = false;
	for (int i = 0; i < mapBoxSize; i++) {
		int index = 3 * (ix + i * m_oCollisionMap.m_nWidth + iy * m_oCollisionMap.m_nWidth);
		int fillSize = 3 * mapBoxSize;
		if (column == columnCount) {
			fillSize = (m_oCollisionMap.m_nWidth * 3) - column * 3 * mapBoxSize;
		}

		for (int j = 0; j < fillSize / 3; j++) {
			char rgb[3];
			char obstacle[3] = { 0, 0, 255 };
			memcpy(rgb, &m_oCollisionMap.m_vTexels[index + j * 3], 3);

			int c = memcmp(obstacle, rgb, 3);
			if (c == 0)
				return true;
		}
	}
	return false;
}

float CCollisionManager::WorldToPixel(float worldLenght)
{
	float ret;
	int nWidth, nHeight;
	m_oRenderer.GetResolution(nWidth, nHeight);

	IBox* pBox = m_pGround->GetBBox();
	if (pBox->GetDimension().m_x <= pBox->GetDimension().m_z * m_fScreenRatio)
		ret = (worldLenght * (float)nHeight) / pBox->GetDimension().m_z;
	else
		ret = worldLenght * (float)nWidth / pBox->GetDimension().m_x;
	return ret;
}

void CCollisionManager::MarkObstacles(IEntityManager* pEntityManager)
{
	float originX = -m_fGroundWidth / 2;
	float originY = -m_fGroundHeight / 2;
	int firstRow = 0, firstColumn = 0;
	int rowCount, columnCount;
	GetCellCoordFromPosition(originX, originY, firstRow, firstColumn);
	ComputeRowAndColumnCount(rowCount, columnCount);
	
	m_pCollisionGrid = new char*[rowCount];
	for (int iRow = 0; iRow < rowCount; iRow++) {
		m_pCollisionGrid[iRow] = new char[columnCount];
	}

	for (int iRow = firstRow; iRow < firstRow + rowCount; iRow++) {
		for (int iColumn = firstColumn; iColumn < firstColumn + columnCount; iColumn++) {
			bool obstacle = TestCellObstacle(iRow, iColumn);
			if (obstacle) {
				m_pCollisionGrid[iRow][iColumn] = 'o';
				MarkBox(iRow, iColumn, 1, 0, 0, pEntityManager);
				MarkMapBox(iRow, iColumn, 255, 0, 0);
			}
			else {
				m_pCollisionGrid[iRow][iColumn] = 'f';
			}
		}
	}
	m_oLoaderManager.Export("CollisionFinal_terrain.bmp", m_oCollisionMap);
}

void CCollisionManager::FindPath(float fromX, float fromY, float toX, float toY, IEntityManager* pEntityManager)
{
	MarkObstacles(pEntityManager);
}

void CCollisionManager::Test(IEntityManager* pEntityManager)
{
	float fromX = 2000.f;
	float fromY = 0.f;
	float toX = 1900.f;
	float toY = 6000.f;
	FindPath(fromX, fromY, toX, toY, pEntityManager);
}

void CCollisionManager::Test2()
{
	for (vector<IEntity*>::iterator it = m_vGridElements.begin(); it != m_vGridElements.end(); it++) {
		(*it)->Unlink();
		delete (*it);
	}
	m_vGridElements.clear();
}

void CCollisionManager::SetEntityManager(IEntityManager* pEntityManager)
{
	m_pEntityManager = pEntityManager;
}

extern "C" _declspec(dllexport) CCollisionManager* CreateCollisionManager( const CCollisionManager::Desc& oDesc )
{
	return new CCollisionManager( oDesc );
}