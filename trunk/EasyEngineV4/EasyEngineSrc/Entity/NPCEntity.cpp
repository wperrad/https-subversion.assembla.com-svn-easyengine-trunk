#include "NPCEntity.h"

CNPCEntity::CNPCEntity( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IFileSystem* pFileSystem, ICollisionManager& oCollisionManager, IGeometryManager& oGeometryManager ):
CMobileEntity( sFileName, oRessourceManager, oRenderer, pEntityManager, pFileSystem, oCollisionManager, oGeometryManager )
{
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

void CNPCEntity::TurnFaceToDestination()
{ 
	Roll( GetDestinationAngleRemaining() ); 
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

void CNPCEntity::UpdateGoto()
{
	if( !m_bArriveAtDestination )
	{
		float fDistance = GetDistanceTo2dPoint( m_oDestination );
		if( fDistance > 200 )
		{
			const float fRotateSpeed = 2.f;
			if( m_fAngleRemaining > fRotateSpeed || m_fAngleRemaining < -fRotateSpeed )
			{
				float fDelta = m_fAngleRemaining > 0 ? -fRotateSpeed : fRotateSpeed;
				m_fAngleRemaining = m_fAngleRemaining + fDelta;
				Roll( -fDelta );
			}
			else
				m_fAngleRemaining = GetDestinationAngleRemaining();
		}
		else
			Roll( GetDestinationAngleRemaining() );
		
		if( fDistance < 100.f )
		{
			m_bArriveAtDestination = true;
			if( m_eCurrentAnimationType == eRun )
				CMobileEntity::Stand();
		}
	}
}

void CNPCEntity::SetDestination( const CVector& oDestination )
{
	m_oDestination = oDestination;
	m_bArriveAtDestination = false;
}

void CNPCEntity::Goto( IAEntity* pEntity, float fSpeed )
{
	CVector oPosition;
	pEntity->GetPosition( oPosition );
	Goto( oPosition, fSpeed );
}

void CNPCEntity::Goto( const CVector& oPosition, float fSpeed )
{
	m_oDestination = oPosition;
	m_fAngleRemaining = GetDestinationAngleRemaining();
	//LookAt( m_fAngleRemaining );
	Run();
	m_bArriveAtDestination = false;
}

void CNPCEntity::Goto( IEntity* pEntity, float fSpeed )
{
	CVector oPosition;
	pEntity->GetWorldPosition( oPosition );
	Goto( oPosition, fSpeed );
}

float CNPCEntity::GetDestinationAngleRemaining()
{
	CVector v( 0, -1, 0, 1 ), oThisPosition, oTempPosition( m_oDestination.m_x, 0, m_oDestination.m_z );	
	CVector oBefore = m_oWorldMatrix.GetRotation() * v;
	
	GetWorldPosition( oThisPosition );
	oThisPosition = CVector( oThisPosition.m_x, 0, oThisPosition.m_z );
	CVector oDirection = oTempPosition - oThisPosition;
	float n = ( oBefore.Norm() * oDirection.Norm() );
	float cosAlpha = 1.f;
	if( n != 0 )
		cosAlpha  = ( oBefore * oDirection ) / n;
	if( cosAlpha > 1.f ) cosAlpha = 1.f;
	else if( cosAlpha < -1.f ) cosAlpha = -1.f;

	
	float alpha = acosf( cosAlpha ) * 180.f / 3.1415927f;
	CVector up = ( oBefore ^ oDirection ) / n;
	if( up.m_y < 0 )
		alpha = -alpha;
	return alpha;
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
	UpdateFightState();
	UpdateGoto();
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