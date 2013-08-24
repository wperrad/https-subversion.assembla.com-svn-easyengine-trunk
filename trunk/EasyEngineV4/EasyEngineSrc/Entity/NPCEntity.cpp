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

void CNPCEntity::ComputePathFind( const CVector& oOrigin, const CVector& oDestination, vector< CVector >& vPoints )
{
	unsigned float fNearestDistance = -1.f;
	IBox* pNearestCollideBox = NULL;
	CVector oCylinderCenter;
	unsigned float fBoundingCylinderRadius = -1.f;
	CVector oPos = oOrigin;
	oPos.m_y = 0;
	IBox* pCollideBox = GetFirstCollideBox();
	while( pCollideBox )
	{		
		pCollideBox->GetCenter( oCylinderCenter );
		oCylinderCenter = pCollideBox->GetWorldMatrix() * oCylinderCenter;
		fBoundingCylinderRadius = pCollideBox->ComputeBoundingCylinderRadius( IBox::eAxisY );
		float fDistance = ( oPos - oCylinderCenter ).Norm() - fBoundingCylinderRadius;
		if( fNearestDistance > fDistance )
		{
			fNearestDistance = fDistance;
			pNearestCollideBox = pCollideBox;
		}
		pCollideBox = GetNextCollideBox();
	}


	CVector H;
	ICylinder* pBoundingCylinder = m_oGeometryManager.CreateCylinder( oCylinderCenter, fBoundingCylinderRadius, pNearestCollideBox->GetDimension().m_y );
	ISegment* pSegment = m_oGeometryManager.CreateSegment( oPos, oDestination );
	pSegment->ComputeProjectedPointOnLine( pBoundingCylinder->GetBase(), H );
	float fDistanceTocylinder = ( pBoundingCylinder->GetBase() - H ).Norm() - fBoundingCylinderRadius;
	if( fDistanceTocylinder < 0 )
	{
		CVector oTangentStart, oTangentEnd;
		// On calcul le point de contact de la tangente du cylindre au point de départ
		CVector oCenterCylDirection = pBoundingCylinder->GetBase() - oPos;
		oCenterCylDirection.m_y = 0.f;
		oCenterCylDirection.Normalize();
		CVector oDirection = oDestination - oPos;
		oDirection.m_y = 0.f;
		oDirection.Normalize();
		CVector v = oCenterCylDirection ^ oDirection;
		bool bLeft = v.m_y > 0.f;
		pBoundingCylinder->ComputeTangent( oPos, oTangentStart, bLeft );

		// On calcule le point de contact de la tangente du cylindre au point d'arrivée
		oCenterCylDirection = pBoundingCylinder->GetBase() - oDestination;
		oCenterCylDirection.m_y = 0.f;
		oCenterCylDirection.Normalize();
		oDirection = -oDirection;
		v = oCenterCylDirection ^ oDirection;
		bLeft = v.m_y > 0.f;
		pBoundingCylinder->ComputeTangent( oDestination, oTangentEnd, bLeft );

		CVector oTangent;
		m_oCollisionManager.Get2DIntersection( oPos, oTangentStart, oDestination, oTangentEnd, oTangent );

		vPoints.push_back( oTangent );

		// test
#if 0
		CVector u1 = oPos;
		u1.m_y = 700.f;
		CVector u2 = oTangentStart;
		u2.m_y = 300.f;
		CVector ouDir = oTangentStart - oPos;
		ouDir.Normalize();
		u2 += ouDir * 1000.f;
		//u1 -= ouDir * 1000.f;

		CVector v1 = oTangentEnd;
		v1.m_y = 500.f;
		CVector v2 = oDestination;
		v2.m_y = -300.f;
		CVector ovDir = oDestination - oTangentEnd;
		ovDir.Normalize();
		v1 -= ovDir * 1000.f;

		IEntity* pTangentStart = m_pEntityManager->CreateLineEntity( u1, u2 );
		pTangentStart->Link( m_pScene );

		IEntity* pTangentEnd = m_pEntityManager->CreateLineEntity( v1, v2 );
		pTangentEnd->Link( m_pScene );

		IEntity* pSphere = m_pEntityManager->CreateSphere( 10 );
		CVector w = oTangent;
		w.m_y = 700.f;
		pSphere->SetWorldPosition( w );
		//pSphere->Link( m_pScene );
#endif
		// fin test

		ComputePathFind( oTangent, oDestination, vPoints );
	}
	else
		vPoints.push_back( oDestination );

}