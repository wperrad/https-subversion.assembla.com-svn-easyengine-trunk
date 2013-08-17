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
	if( pEntity )
	{
		if( pEntity == this )
			pEntity = m_pEntityManager->GetNextCollideEntity();
		IMesh* pMesh = static_cast< IMesh* >( pEntity->GetRessource() );
		if( pEntity->GetCurrentAnimation() )
		{
			string sAnimationName;
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

void CNPCEntity::ComputePathFind( const CVector& oDestination, vector< CVector >& vPoints )
{
	IBox* pCollideBox = GetFirstCollideBox();
	while( pCollideBox )
	{
		float fBoundingCylinderRadius = pCollideBox->ComputeBoundingCylinderRadius( IBox::TAxis::eAxisY );
		CVector oPos;
		GetPosition( oPos );
		ISegment* pSegment = m_oGeometryManager.CreateSegment( oPos, oDestination );

		CVector oCylinderCenter, H;
		pCollideBox->GetCenter( oCylinderCenter );
		oCylinderCenter = pCollideBox->GetWorldMatrix() * oCylinderCenter;
		//pSegment->ComputeProjectedPointOnLine( oCylinderCenter, H );
		float fDistanceTocylinder = pSegment->ComputeDistanceToPoint( oCylinderCenter ) - fBoundingCylinderRadius;
		if( fDistanceTocylinder < 0 )
		{
			// calculer le point de contact de la tangente du cylindre
			CVector oTangent;
		}


		ISphere* pSphere = m_oGeometryManager.CreateSphere(CVector(), 20 );
		CSphereEntity* pSphereEntity = new CSphereEntity( m_oRenderer, *pSphere );
		pSphereEntity->Link( m_pScene );
		pSphereEntity->SetWorldPosition( H.m_x, H.m_y, H.m_z );

		CLineEntity* pLineTrajectoire = new CLineEntity( m_oRenderer, oPos, oDestination );
		pLineTrajectoire->Link( m_pScene );

		float fBoxHeight = pCollideBox->GetDimension().m_y;
		CCylinderEntity* pCylinderEntity = new CCylinderEntity( m_oGeometryManager, m_oRenderer, m_oRessourceManager, m_oCollisionManager, fBoundingCylinderRadius, fBoxHeight );
		pCylinderEntity->SetWorldPosition( oCylinderCenter.m_x, oCylinderCenter.m_y, oCylinderCenter.m_z );
		pCylinderEntity->Link( m_pScene );

		CLineEntity* pLineTangent = new CLineEntity( m_oRenderer, oCylinderCenter, H );
		pLineTangent->Link( m_pScene );
		

		pCollideBox = GetNextCollideBox();
		break;

		if( GetCollisionManager().IsIntersection( *pSegment, *pCollideBox ) )
		{
			vector< CVector > vBoxPoints;
			pCollideBox->GetPoints( vBoxPoints );
		}
		pCollideBox = GetNextCollideBox();
	}
}