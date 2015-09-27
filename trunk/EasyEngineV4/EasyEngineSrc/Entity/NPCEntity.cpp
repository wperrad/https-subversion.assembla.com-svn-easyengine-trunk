#include "NPCEntity.h"
#include "IGeometry.h"
#include "LineEntity.h"
#include "Scene.h"
#include "ICollisionManager.h"
#include "CylinderEntity.h"
#include "SphereEntity.h"

CNPCEntity::CNPCEntity( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IFileSystem* pFileSystem, ICollisionManager& oCollisionManager, IGeometryManager& oGeometryManager ):
CMobileEntity( sFileName, oRessourceManager, oRenderer, pEntityManager, pFileSystem, oCollisionManager, oGeometryManager )
{
	if( !m_pfnCollisionCallback )
		m_pfnCollisionCallback = OnCollision;
}

int CNPCEntity::GetLife()
{ 
	return CMobileEntity::GetLife(); 
}

void CNPCEntity::SetLife( int nLife )
{ 
	CMobileEntity::SetLife( nLife ); 
}

void CNPCEntity::IncreaseLife( int nLife )
{ 
	CMobileEntity::IncreaseLife( nLife ); 
}

float CNPCEntity::GetDistanceTo2dPoint( const CVector& oPosition )
{
	CVector oThisPosition;
	GetWorldPosition( oThisPosition );
	CVector o2DThisPosition = CVector( oThisPosition.m_x, 0, oThisPosition.m_z );
	CVector o2DPosition = CVector( oPosition.m_x, 0, oPosition.m_z );
	return ( o2DPosition - o2DThisPosition ).Norm();
}

void CNPCEntity::Run()
{ 
	CMobileEntity::Run( true ); 
}

void CNPCEntity::LookAt( float alpha )
{
	float fMustEyeRotH = 0.f;
	float fMustNeckRotH = 0.f;
	//	const float fEyeRotMult = 3.6f;
	const float fEyeRotMult = 4.3f;
	if( ( alpha < m_fMaxEyeRotationH * fEyeRotMult ) && ( alpha > -m_fMaxEyeRotationH * fEyeRotMult  ) )
	{
		fMustEyeRotH = alpha / fEyeRotMult - m_fEyesRotH;
		m_fEyesRotH += fMustEyeRotH;
	}
	else
	{
		fMustEyeRotH = m_fMaxEyeRotationH - m_fEyesRotH;
		if( alpha < 0 )
			fMustEyeRotH = -fMustEyeRotH;
		m_fEyesRotH += fMustEyeRotH;
		alpha -= m_fEyesRotH * fEyeRotMult;
		if( alpha < m_fMaxNeckRotationH - m_fNeckRotH )
			fMustNeckRotH = alpha - m_fNeckRotH;
		else
			fMustNeckRotH = m_fMaxNeckRotationH - m_fNeckRotH;
	}
	m_fNeckRotH += fMustNeckRotH;
	TurnEyesH( fMustEyeRotH );
	TurnNeckH( fMustNeckRotH );
}

void CNPCEntity::ReceiveHit( IAEntity* pEnemy )
{
	CMobileEntity::ReceiveHit( pEnemy );
}

void CNPCEntity::Attack( IFighterEntity* pEnemy )
{
	CMobileEntity::HitLeftFoot( false );
}

void CNPCEntity::Update()
{
	CMobileEntity::Update();
	IAEntity::Update();	
}

IAnimation* CNPCEntity::GetCurrentAnimation()
{
	return CMobileEntity::GetCurrentAnimation();
}

CMatrix& CNPCEntity::GetWorldTM()
{
	return CMobileEntity::GetWorldTM();
}

IFighterEntity* CNPCEntity::GetFirstEnemy()
{
	return CMobileEntity::GetFirstEnemy();
}

IFighterEntity* CNPCEntity::GetNextEnemy()
{
	return CMobileEntity::GetNextEnemy();
}

void CNPCEntity::GetPosition( CVector& v )
{
	CMobileEntity::GetPosition( v );
}

void CNPCEntity::ReceiveHit( IFighterEntity* pEnemy )
{
	RunAction( "PlayReceiveHit", false );
}

void CNPCEntity::Stand()
{
	CMobileEntity::Stand();
}

void CNPCEntity::Turn( float fAngle )
{
	Roll( fAngle );
}

void CNPCEntity::OnCollision( IEntity* pEntity )
{
	CNPCEntity* pNPC = static_cast< CNPCEntity* >( pEntity );
	IAEntity::OnCollision( pNPC );
}

void CNPCEntity::Goto( const CVector& oPosition, float fSpeed )
{
	IAEntity::Goto( oPosition, fSpeed );
}

IBox* CNPCEntity::GetFirstCollideBox()
{
	IEntity* pEntity = m_pEntityManager->GetFirstCollideEntity();
	if( pEntity == this )
		pEntity = m_pEntityManager->GetNextCollideEntity();
	if( pEntity )
	{
		IMesh* pMesh = static_cast< IMesh* >( pEntity->GetRessource() );
		string sAnimationName;
		if( pEntity->GetCurrentAnimation() )
		{
			pEntity->GetCurrentAnimation()->GetName( sAnimationName );
			return pMesh->GetAnimationBBox( sAnimationName );
		}
		else
		{
			IBox* pBox = m_oGeometryManager.CreateBox( *pMesh->GetBBox() );
			pBox->SetWorldMatrix( pEntity->GetWorldMatrix() );
			return pBox;
		}
	}
	return NULL;
}

IBox* CNPCEntity::GetNextCollideBox()
{
	IEntity* pEntity = m_pEntityManager->GetNextCollideEntity();
	IBox* pBoxRet = NULL;
	if( pEntity )
	{
		if( pEntity == this )
			pEntity = m_pEntityManager->GetNextCollideEntity();
		IMesh* pMesh = static_cast< IMesh* >( pEntity->GetRessource() );
		if( pEntity->GetCurrentAnimation() )
		{
			string sAnimationName;
			pEntity->GetCurrentAnimation()->GetName( sAnimationName );
			pBoxRet = pMesh->GetAnimationBBox( sAnimationName );
		}
		else
		{
			IBox* pBox = m_oGeometryManager.CreateBox( *pMesh->GetBBox() );
			pBox->SetWorldMatrix( pEntity->GetWorldMatrix() );
			pBoxRet = pBox;
		}
	}
	return pBoxRet;
}

void CNPCEntity::ComputePathFind2D( const CVector2D& oOrigin, const CVector2D& oDestination, vector< CVector2D >& vPoints )
{
	float fDestinationDistance = (oOrigin - oDestination).Norm() - m_pBBox->ComputeBoundingCylinderRadius( IBox::eAxisY );
	if(fDestinationDistance < 0)
	{
		vPoints.push_back(oDestination);
		return;
	}
	
	float fNearestDistance = -1.f;
	IBox* pNearestCollideBox = NULL;
	
	float fBoundingCylinderRadius = -1.f;
	CVector2D oCircleCenter;
	IBox* pCollideBox = GetFirstCollideBox();
	while( pCollideBox )
	{	
		CVector oCylinderCenter;
		pCollideBox->GetCenter( oCylinderCenter );
		oCylinderCenter = pCollideBox->GetWorldMatrix() * oCylinderCenter;
		oCircleCenter.m_x = oCylinderCenter.m_x;
		oCircleCenter.m_y = oCylinderCenter.m_z;
		fBoundingCylinderRadius = pCollideBox->ComputeBoundingCylinderRadius( IBox::eAxisY );
		float fDistance = ( oOrigin - oCircleCenter ).Norm() - fBoundingCylinderRadius;
		if( fNearestDistance > fDistance )
		{
			fNearestDistance = fDistance;
			pNearestCollideBox = pCollideBox;
		}
		pCollideBox = GetNextCollideBox();
	}

	CVector2D H;
	ICircle* pBoundingCircle = m_oGeometryManager.CreateCircle( oCircleCenter, fBoundingCylinderRadius );
	ISegment2D* pSegment = m_oGeometryManager.CreateSegment2D( oOrigin, oDestination );
	pSegment->ComputeProjectedPointOnLine( pBoundingCircle->GetCenter(), H );
	float fDistanceTocylinder = ( pBoundingCircle->GetCenter() - H ).Norm() - fBoundingCylinderRadius;
	if( fDistanceTocylinder < 0 )
	{
		CVector2D oPathIntersect;
		if( !pBoundingCircle->IsPointIntoCircle( oOrigin ) && !pBoundingCircle->IsPointIntoCircle( oDestination ) ) // le test du cylindre suffit-il ?
		{
			CVector2D oTangentStart, oTangentEnd;
			bool bLeft = pBoundingCircle->IsSegmentAtLeftSide( oOrigin, oDestination );
			pBoundingCircle->ComputeTangent( oOrigin, oTangentStart, bLeft );
			pBoundingCircle->ComputeTangent( oDestination, oTangentEnd, !bLeft);
			m_oCollisionManager.Get2DLineIntersection( oOrigin, oTangentStart, oDestination, oTangentEnd, oPathIntersect );
			vPoints.push_back( oPathIntersect );
			ComputePathFind2D( oPathIntersect, oDestination, vPoints );
		}
		else // le test du cylindre n'est pas assez précis, on doit faire un test sur la box
		{
			const CMatrix& oBoxTM = pNearestCollideBox->GetWorldMatrix();
			CMatrix2X2 oRectTM( oBoxTM.m_00	, oBoxTM.m_02	, oBoxTM.m_03,
								oBoxTM.m_20	, oBoxTM.m_22	, oBoxTM.m_23,
								0			,	0			,		1	);
			if( m_oCollisionManager.IsSegmentRectIntersect( oOrigin, oDestination, pNearestCollideBox->GetDimension().m_x, pNearestCollideBox->GetDimension().m_z, oRectTM ) )
				vPoints.push_back( oDestination ); // temporaire
			else 
				vPoints.push_back( oDestination );			
		}
	}
	else
		vPoints.push_back( oDestination );
}