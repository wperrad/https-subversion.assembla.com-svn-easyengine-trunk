#include "NPCEntity.h"

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

bool CNPCEntity::IsHitIntersectEnemySphere( IFighterEntity* pEnemy )
{
	return CMobileEntity::IsHitIntersectEnemySphere( pEnemy );
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

bool CNPCEntity::IsHitIntersectEnemyBox( IFighterEntity* pEnemy )
{
	return CMobileEntity::IsHitIntersectEnemyBox( pEnemy );
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