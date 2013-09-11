#include "IAEntity.h"
#include "TimeManager.h"
#include "ICollisionManager.h"
#include "IGeometry.h"

IAEntity::IAEntity():
m_nRecuperationTime( 1500 ),
m_bHitEnemy( false ),
m_eFightState( eNoFight ),
m_fAngleRemaining( 0.f ),
m_bArriveAtDestination( true ),
m_fDestinationDeltaRadius( 100.f ),
m_nCurrentPathPointNumber( 0 )
{
}



void IAEntity::UpdateFightState()
{
	CVector oEnemyPos;
	switch( m_eFightState )
	{
	case eNoFight:
		break;
	case eBeginHitReceived:
		IncreaseLife( -20 );
		if( GetLife() <= 0 )
			m_eFightState = eNoFight;
		else
			m_eFightState = eReceivingHit;
		break;
	case eReceivingHit:
		break;
	case eEndReceivingHit:
		m_eFightState = eBeginWaitForNextAttack;
		break;
	case eBeginGoToEnemy:
		m_pCurrentEnemy->GetPosition( oEnemyPos );
		Goto( oEnemyPos, -1.f );
		m_eFightState = eGoingToEnemy;
		break;
	case eGoingToEnemy:
		m_pCurrentEnemy->GetPosition( oEnemyPos );
		if( GetDistanceTo2dPoint( oEnemyPos ) > 100.f )
		{
			SetDestination( oEnemyPos );
			Run();
		}
		else
			m_eFightState = eArrivedToEnemy;
		break;
	case eArrivedToEnemy:
		m_eFightState = eWaitingForNextAttack;
		TurnFaceToDestination();
		Stand();
		break;
	case eBeginLaunchAttack:
		Attack( m_pCurrentEnemy );
		m_eFightState = eLaunchingAttack;
		break;
	case eLaunchingAttack:
		break;
	case eEndLaunchAttack:
		if( m_pCurrentEnemy->GetLife() <= 0 )
			m_eFightState = eEndFight;
		else
			m_eFightState = eBeginWaitForNextAttack;
		break;
	case eBeginWaitForNextAttack:
		m_nBeginWaitTimeBeforeNextAttack = CTimeManager::Instance()->GetCurrentTimeInMillisecond();
		m_eFightState = eBeginGoToEnemy;
		break;
	case eWaitingForNextAttack:
		m_pCurrentEnemy->GetPosition( oEnemyPos );
		if( GetDistanceTo2dPoint( oEnemyPos ) > 100.f )
			m_eFightState = eBeginGoToEnemy;
		else
		{
			m_nCurrentWaitTimeBeforeNextAttack = CTimeManager::Instance()->GetCurrentTimeInMillisecond() - m_nBeginWaitTimeBeforeNextAttack;
			if( m_nCurrentWaitTimeBeforeNextAttack > m_nRecuperationTime )
				m_eFightState = eBeginLaunchAttack;
		}
		break;
	case eEndFight:
		Stand();
		m_eFightState = eNoFight;
		break;
	default:
		throw 1;
	}
}

void IAEntity::OnReceiveHit( IFighterEntity* pAgressor )
{
	m_pCurrentEnemy = pAgressor;
	if( m_eFightState == IAEntity::eNoFight )
		m_eFightState = IAEntity::eBeginHitReceived;
	else if( m_eFightState == IAEntity::eLaunchingAttack )
		m_eFightState = IAEntity::eEndLaunchAttack;
	GetCurrentAnimation()->AddCallback( OnHitReceivedCallback, this );
}

void IAEntity::OnHitReceivedCallback( IAnimation::TEvent e, void* pData )
{
	IAEntity* pThisFighter = reinterpret_cast< IAEntity* >( pData );
	switch( e )
	{
	case IAnimation::eBeginRewind:
		pThisFighter->GetCurrentAnimation()->RemoveCallback( OnHitReceivedCallback );
		if( pThisFighter->m_eFightState == IAEntity::eReceivingHit )
			pThisFighter->m_eFightState = IAEntity::eEndReceivingHit;
		else
			pThisFighter->Stand();
		break;
	}
}

void IAEntity::OnEndHitAnimation()
{
	if( m_eFightState == IAEntity::eLaunchingAttack )
		m_eFightState = IAEntity::eEndLaunchAttack;
	Stand();
}

void IAEntity::Goto( const CVector& oDestination, float fSpeed )
{
	m_vCurrentPath.clear();
	CVector oPos;
	GetPosition( oPos );
	CVector2D oDestination2D( oDestination.m_x, oDestination.m_z ), oPos2D( oPos.m_x, oPos.m_z );
	ComputePathFind2D( oPos2D, oDestination2D, m_vCurrentPath );

	if( m_vCurrentPath.size() > 1 )
		m_fDestinationDeltaRadius = 50.f;
	else
		m_fDestinationDeltaRadius  = 100.f;
	m_nCurrentPathPointNumber = 0;
	//m_oDestination = m_vCurrentPath[ m_nCurrentPathPointNumber ];
	oDestination2D = m_vCurrentPath[ m_nCurrentPathPointNumber ];
	m_oDestination = CVector( oDestination2D.m_x, 0.f, oDestination2D.m_y );
	m_fAngleRemaining = GetDestinationAngleRemaining();

	//LookAt( m_fAngleRemaining );
	Run();
	m_bArriveAtDestination = false;
}


void IAEntity::UpdateGoto()
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
				Turn( -fDelta );
			}
			else
				m_fAngleRemaining = GetDestinationAngleRemaining();
		}
		else
			Turn( GetDestinationAngleRemaining() );
		
		if( fDistance < m_fDestinationDeltaRadius )
		{
			if( m_nCurrentPathPointNumber >= m_vCurrentPath.size() - 1 )
			{
				m_bArriveAtDestination = true;
				Stand();
				m_vCurrentPath.clear();
			}
			else
			{
				m_nCurrentPathPointNumber++;
				CVector2D oDestination = m_vCurrentPath[ m_nCurrentPathPointNumber ];
				m_oDestination = CVector( oDestination.m_x, 0.f, oDestination.m_y );
			}
		}
	}
}

void IAEntity::Update()
{
	UpdateGoto();
	UpdateFightState();
}

void IAEntity::SetDestination( const CVector& oDestination )
{
	m_oDestination = oDestination;
	m_bArriveAtDestination = false;
	if( m_vCurrentPath.size() == 0 )
	{
		CVector2D oDestination2D( m_oDestination.m_x, m_oDestination.m_z );
		m_vCurrentPath.push_back( oDestination2D );
	}
}

float IAEntity::GetDestinationAngleRemaining()
{
	CVector v( 0, -1, 0, 1 ), oThisPosition, oTempPosition( m_oDestination.m_x, 0, m_oDestination.m_z );
	CVector oBefore = GetWorldTM().GetRotation() * v;
	
	GetPosition( oThisPosition );
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

void IAEntity::OnCollision( IAEntity* pEntity )
{
	IAEntity* pHuman = static_cast< IAEntity* >( pEntity );
	pHuman->m_bArriveAtDestination = true;
	pHuman->Stand();
}

void IAEntity::TurnFaceToDestination()
{ 
	Turn( GetDestinationAngleRemaining() ); 
}

