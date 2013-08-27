#include "CollisionManager.h"
#include "IRessource.h"
#include "IShader.h"
#include "ILoader.h"
#include "HeightMap.h"
#include "IGeometry.h"
#include "RenderUtils.h"

IMesh* CCollisionManager::s_pMesh = NULL;

CCollisionManager* g_pCurrentCollisionManager = NULL;

CCollisionManager::CCollisionManager( const ICollisionManager::Desc& oDesc ):
ICollisionManager( oDesc ),
m_oRenderer( oDesc.m_oRenderer ),
m_oLoaderManager( oDesc.m_oLoaderManager ),
m_nHeightMapPrecision( 1 ),
m_pFileSystem( oDesc.m_pFileSystem ),
m_oGeometryManager( oDesc.m_oGeometryManager )
{
	g_pCurrentCollisionManager = this;
}

void CCollisionManager::SetHeightMapPrecision( int nPrecision )
{
	m_nHeightMapPrecision = nPrecision;
}

void CCollisionManager::CreateHeightMap( IMesh* pMesh, ILoader::CTextureInfos& ti, IRenderer::TPixelFormat format )
{
	IShader* pOrgShader = pMesh->GetCurrentShader();
	IShader* pHMShader = m_oRenderer.GetShader( "hm" );
	pHMShader->Enable( true );
	pMesh->SetShader( pHMShader );
	
	CMatrix oModelView, oProj;
	int nWidth, nHeight;
	m_oRenderer.GetResolution( nWidth, nHeight );
	float fScreenRatio = (float)nWidth / (float)nHeight;
	oProj.m_00 = 1.f / fScreenRatio;
	const IBox* pBox = pMesh->GetBBox();
	float maxLenght = pBox->GetDimension().m_x;
	float fMapWidth, fMapHeight;
	if( maxLenght < pBox->GetDimension().m_y )
	{
		maxLenght = pBox->GetDimension().m_y;
		fMapWidth = ( pBox->GetDimension().m_x * (float)nWidth ) / ( maxLenght * fScreenRatio );
		fMapHeight = (float)nHeight;
	}
	else
	{
		fMapWidth = (float)nWidth;
		fMapHeight = pBox->GetDimension().m_y * (float)nHeight / maxLenght;
	}
	fMapWidth = ( (int)fMapWidth / 4 ) * 4;
	fMapHeight = ( (int)fMapHeight / 4 ) * 4;
	float fOriginMapX = ( (float)nWidth - fMapWidth ) / 2.f;
	float fOriginMapY = ( (float)nHeight - fMapHeight ) / 2.f;
	float scale = ( maxLenght / 2.f );
	
	pHMShader->SendUniformValues( "h", pBox->GetDimension().m_z );
	pHMShader->SendUniformValues( "zMin", pBox->GetMinPoint().m_z );	
	pHMShader->SendUniformValues( "scale", scale );
	pHMShader->SendUniformValues( "nPrecision", m_nHeightMapPrecision );

	CMatrix oBakProj;
	m_oRenderer.GetProjectionMatrix( oBakProj );
	m_oRenderer.SetProjectionMatrix( oProj );

	m_oRenderer.BeginRender();
	pMesh->Update();
	
	m_oRenderer.ReadPixels( fOriginMapX, fOriginMapY, fMapWidth, fMapHeight, ti.m_vTexels, format );
	ti.m_nWidth = (int)fMapWidth;
	ti.m_nHeight = (int)fMapHeight;
	m_oRenderer.EndRender();

	pHMShader->Enable( false );
	pMesh->SetShader( pOrgShader );
	m_oRenderer.SetProjectionMatrix( oBakProj );
}

void CCollisionManager::DisplayHeightMap( IMesh* pMesh )
{
	m_oRenderer.GetBackgroundColor( m_oOriginBackgroundColor );
	ILoader::CTextureInfos ti;
	CreateHeightMap( pMesh, ti );
	s_pMesh = pMesh;
	m_oRenderer.AbonneToRenderEvent( Update );
}

void CCollisionManager::StopDisplayHeightMap()
{
	m_oRenderer.DesabonneToRenderEvent( Update );
	m_oRenderer.SetBackgroundColor( m_oOriginBackgroundColor.m_x, m_oOriginBackgroundColor.m_y, m_oOriginBackgroundColor.m_z );
}

void CCollisionManager::LoadHeightMap( string sFileName, vector< vector< unsigned char > >& vPixels )
{
	ILoader::CTextureInfos ti;
	m_oLoaderManager.Load( sFileName, ti );
	vPixels.clear();
	vPixels.resize( ti.m_nWidth );
	for( int i = 0; i < ti.m_nWidth ; i++ )
		for( int j = 0; j < ti.m_nHeight; j++ )
			vPixels[ i ].push_back( ti.m_vTexels[ j * ( ti.m_nWidth + 1 ) ] );
}

void CCollisionManager::Update( IRenderer* pRenderer )
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

void CCollisionManager::Get2DLineIntersection( const CVector& oLine1First, const CVector& oLine1Last, const CVector& oLine2First, const CVector& oLine2Last, CVector& oIntersection )
{
	ISegment* pL1 = m_oGeometryManager.CreateSegment( oLine1First, oLine1Last );
	ISegment* pL2 = m_oGeometryManager.CreateSegment( oLine2First, oLine2Last );
	float a1, b1, c1, a2, b2, c2;
	pL1->Compute2DLineEquation( a1, b1, c1 );
	pL2->Compute2DLineEquation( a2, b2, c2 );
	oIntersection.m_x = 1 / a1 * ( b1 * ( (a2 * c1 - a1 * c2 ) / ( a2 * b1 - a1 * b2 )  ) - c1 );
	oIntersection.m_z = ( a1 * c2 - a2 * c1 ) / ( a2 * b1 - a1 * b2 );
}

void CCollisionManager::Get2DLineIntersection( const ISegment& oSeg1, const ISegment& oSeg2, CVector& oIntersection )
{
	float a1, b1, c1, a2, b2, c2;
	oSeg1.Compute2DLineEquation( a1, b1, c1 );
	oSeg2.Compute2DLineEquation( a2, b2, c2 );
	oIntersection.m_x = 1 / a1 * ( b1 * ( (a2 * c1 - a1 * c2 ) / ( a2 * b1 - a1 * b2 )  ) - c1 );
	oIntersection.m_z = ( a1 * c2 - a2 * c1 ) / ( a2 * b1 - a1 * b2 );
}

bool CCollisionManager::Is2DSegmentRectIntersect( const ISegment& s, float fRectw, float fRecth, const CMatrix& oRectTM )
{
	CVector S1, S2;
	s.GetPoints( S1, S2 );
	return Is2DSegmentRectIntersect( S1, S2, fRectw, fRecth, oRectTM );

}

bool CCollisionManager::Is2DSegmentRectIntersect( const CVector& S1, const CVector& S2, float fRectw, float fRecth, const CMatrix& oRectTM )
{
	CVector x( 1, 0, 0 );

	CVector R0 = CVector( -fRectw / 2.f, 0, -fRecth / 2.f );
	CVector R1 = CVector( fRectw / 2.f, 0, -fRecth / 2.f );
	CVector R2 = CVector( fRectw / 2.f, 0, fRecth / 2.f );
	CVector R3 = CVector( -fRectw / 2.f, 0, fRecth / 2.f );
	CMatrix oRectInvTM;
	oRectTM.GetInverse( oRectInvTM );
	
	CVector S1Inv = oRectInvTM * S1;
	CVector S2Inv = oRectInvTM * S2;

	float fMinx = R0.m_x, fMaxx = R1.m_x, fMinz = R0.m_z, fMaxz = R2.m_z;
	float fSegMinx = S1Inv.m_x;
	if( fSegMinx > S2Inv.m_x ) fSegMinx = S2Inv.m_x;
	if( fSegMinx > fMaxx )
		return false;
	float fSegMaxx = S1Inv.m_x;
	if( fSegMaxx < S2Inv.m_x ) fSegMaxx = S2Inv.m_x;
	if( fSegMaxx < fMinx )
		return false;
	float fSegMinz = S1Inv.m_z;
	if( fSegMinz > S2Inv.m_z ) fSegMinz = S2Inv.m_z;
	if( fSegMinz > fMaxz )
		return false;
	float fSegMaxz = S1Inv.m_z;
	if( fSegMaxz < S2Inv.m_z ) fSegMaxz = S2Inv.m_z;
	if( fSegMaxz < fMinz )
		return false;

	CVector R0tm = oRectTM * CVector( -fRectw / 2.f, 0, -fRecth / 2.f );
	CVector R1tm = oRectTM * CVector( fRectw / 2.f, 0, -fRecth / 2.f );
	CVector R2tm = oRectTM * CVector( fRectw / 2.f, 0, fRecth / 2.f );
	CVector R3tm = oRectTM * CVector( -fRectw / 2.f, 0, fRecth / 2.f );

	float alpha = acosf( ( ( S2 - S1 ) * x ) / ( S2 - S1 ).Norm() ) * 180.f / 3.1415927f;
	CMatrix oSegTM = CMatrix::GetyRotation( alpha ), oSegTMInv;
	oSegTM.AddTranslation( S1 );
	oSegTM.GetInverse( oSegTMInv );

	CVector R0Inv = oSegTMInv * R0tm;
	CVector R1Inv = oSegTMInv * R1tm;
	CVector R2Inv = oSegTMInv * R2tm;
	CVector R3Inv = oSegTMInv * R3tm;

	CVector S3Inv = oSegTMInv * S2;

	fMinx = R0Inv.m_z;
	if( R1Inv.m_x < fMinx ) fMinx = R1Inv.m_x;
	if( R2Inv.m_x < fMinx ) fMinx = R2Inv.m_x;
	if( R3Inv.m_x < fMinx ) fMinx = R3Inv.m_x;
	if( fMinx > S3Inv.m_x )
		return false;

	fMaxx = R0Inv.m_z;
	if( R1Inv.m_x > fMaxx ) fMaxx = R1Inv.m_z;
	if( R2Inv.m_x > fMaxx ) fMaxx = R2Inv.m_z;
	if( R3Inv.m_x > fMaxx ) fMaxx = R3Inv.m_z;
	if( fMaxx < 0 )
		return false;

	fMinz = R0Inv.m_z;
	if( R1Inv.m_z < fMinz ) fMinz = R1Inv.m_z ;
	if( R2Inv.m_z < fMinz ) fMinz = R2Inv.m_z;
	if( R3Inv.m_z < fMinz ) fMinz = R3Inv.m_z;
	if( fMinz > 0 )
		return false;

	fMaxz = R0Inv.m_z;
	if( R1Inv.m_z > fMaxz ) fMaxz = R1Inv.m_z;
	if( R2Inv.m_z > fMaxz ) fMaxz = R2Inv.m_z ;
	if( R3Inv.m_z > fMaxz ) fMaxz = R3Inv.m_z;
	if( fMaxz < 0 )
		return false;

	return true;
}

extern "C" _declspec(dllexport) CCollisionManager* CreateCollisionManager( const CCollisionManager::Desc& oDesc )
{
	return new CCollisionManager( oDesc );
}