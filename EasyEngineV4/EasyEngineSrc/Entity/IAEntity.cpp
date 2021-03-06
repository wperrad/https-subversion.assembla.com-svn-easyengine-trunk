#include "IAEntity.h"
#include "Utils2/TimeManager.h"
#include "ICollisionManager.h"
#include "IGeometry.h"


const float fRotateSpeed = 10.f;

IAEntity::IAEntity():
m_nRecoveryTime( 1000 ),
m_bHitEnemy( false ),
m_eFightState( eNoFight ),
m_fAngleRemaining( 0.f ),
m_bArriveAtDestination( true ),
m_fDestinationDeltaRadius( 100.f ),
m_nCurrentPathPointNumber( 0 ),
m_pCurrentEnemy(NULL),
m_bFaceToTarget(false)
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
		if( GetLife() <= 0 )
			m_eFightState = eNoFight;
		else
			m_eFightState = eReceivingHit;
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
		m_eFightState = ePreparingForNextAttack;
		TurnFaceToDestination();
		Stand();
		break;
	case eBeginLaunchAttack:
		Hit();
		m_eFightState = eLaunchingAttack;
		break;
	case eLaunchingAttack:
		break;
	case eEndLaunchAttack:
		if( m_pCurrentEnemy->GetLife() <= 0 )
			m_eFightState = eEndFight;
		else
			m_eFightState = eBeginPrepareForNextAttack;
		break;
	case eBeginPrepareForNextAttack:
		Guard();
		m_nBeginWaitTimeBeforeNextAttack = CTimeManager::Instance()->GetCurrentTimeInMillisecond();
		m_eFightState = ePreparingForNextAttack;
		break;
	case ePreparingForNextAttack:
		m_pCurrentEnemy->GetPosition( oEnemyPos );
		if( GetDistanceTo2dPoint( oEnemyPos ) > 100.f )
			m_eFightState = eBeginGoToEnemy;
		else
		{
			FaceTo(oEnemyPos);
			//Guard();
			m_nCurrentWaitTimeBeforeNextAttack = CTimeManager::Instance()->GetCurrentTimeInMillisecond() - m_nBeginWaitTimeBeforeNextAttack;
			if( m_nCurrentWaitTimeBeforeNextAttack > m_nRecoveryTime )
				m_eFightState = eBeginLaunchAttack;
		}
		break;
	case eEndFight:
		if(m_pCurrentEnemy->GetLife() > 0)
			Stand();
		m_eFightState = eNoFight;
		break;
	default:
		break;
	}
}

void IAEntity::OnReceiveHit( IFighterEntity* pAgressor )
{
	m_pCurrentEnemy = pAgressor;
	m_eFightState = IAEntity::eBeginHitReceived;
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
			pThisFighter->m_eFightState = IAEntity::eBeginPrepareForNextAttack;
		else if(pThisFighter->GetLife() > 0)
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

void IAEntity::FaceTo(const CVector& target)
{
	m_oDestination = CVector(target.m_x, 0.f, target.m_z);
	m_fAngleRemaining = GetDestinationAngleRemaining();
	m_bFaceToTarget = false;
}

void IAEntity::UpdateFaceTo()
{
	if (!m_bFaceToTarget)
	{
		if (m_fAngleRemaining > 1)
		{
			if (m_fAngleRemaining > fRotateSpeed || m_fAngleRemaining < -fRotateSpeed)
			{
				float fDelta = m_fAngleRemaining > 0 ? -fRotateSpeed : fRotateSpeed;
				m_fAngleRemaining = m_fAngleRemaining + fDelta;
				Turn(-fDelta);
			}
			else
				m_fAngleRemaining = GetDestinationAngleRemaining();
		}
		else {
			Turn(GetDestinationAngleRemaining());
			m_bFaceToTarget = true;
		}
	}
}

void IAEntity::Goto( const CVector& oDestination, float fSpeed )
{
	m_vCurrentPath.clear();
	CVector oPos;
	GetPosition( oPos );
	CVector2D oDestination2D( oDestination.m_x, oDestination.m_z ), oPos2D( oPos.m_x, oPos.m_z );
	ComputePathFind2D( oPos2D, oDestination2D, m_vCurrentPath );
	if (!m_vCurrentPath.empty()) {
		if (m_vCurrentPath.size() > 1)
			m_fDestinationDeltaRadius = 50.f;
		else
			m_fDestinationDeltaRadius = 100.f;
		m_nCurrentPathPointNumber = 0;
		oDestination2D = m_vCurrentPath[m_nCurrentPathPointNumber];
		m_oDestination = CVector(oDestination2D.m_x, 0.f, oDestination2D.m_y);
		m_fAngleRemaining = GetDestinationAngleRemaining();

		//LookAt( m_fAngleRemaining );
		Run();
		m_bArriveAtDestination = false;
	}
}


void IAEntity::UpdateGoto()
{
	if( !m_bArriveAtDestination )
	{
		float fDistance = GetDistanceTo2dPoint( m_oDestination );
		if( fDistance > 200 )
		{			
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
	if ( (m_eFightState != eNoFight) && (GetLife() > 0) ) {
		UpdateFaceTo();
		UpdateGoto();
		UpdateFightState();
	}
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
	CVector v( 0, 0, 1, 1 ), oThisPosition, oTempPosition( m_oDestination.m_x, 0, m_oDestination.m_z );
	CVector oBefore = GetWorldTM().GetRotation() * v;
	GetPosition( oThisPosition );
	oThisPosition = CVector( oThisPosition.m_x, 0, oThisPosition.m_z );
	CVector oDirection = oTempPosition - oThisPosition;
	if (oDirection == CVector(0, 0, 0)) {
		return 0.f;
	}
	return GetAngleBetween2Vectors(oBefore, oDirection);
}

float IAEntity::GetAngleBetween2Vectors(CVector& v1, CVector& v2)
{
	float n = (v1.Norm() * v2.Norm());
	float cosAlpha = 0;
	if (n != 0)
		cosAlpha = (v1 * v2) / n;
	if (cosAlpha > 1.f) cosAlpha = 1.f;
	else if (cosAlpha < -1.f) cosAlpha = -1.f;
	float alpha = acosf(cosAlpha) * 180.f / 3.1415927f;
	CVector up = (v1 ^ v2) / n;
	if (up.m_y < 0)
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

void IAEntity::Attack(IFighterEntityInterface* pEntity)
{
	IFighterEntity* pEnemy = dynamic_cast<IFighterEntity*>(pEntity);
	if (pEnemy)
		Attack(pEnemy);
}

void IAEntity::Attack(IFighterEntity* pEntity)
{
	m_pCurrentEnemy = pEntity;
	m_eFightState = eBeginGoToEnemy;
}