#include "IAEntity.h"
#include "TimeManager.h"


IAEntity::IAEntity():
m_nRecuperationTime( 1500 ),
m_bHitEnemy( false ),
m_eFightState( eNoFight )
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