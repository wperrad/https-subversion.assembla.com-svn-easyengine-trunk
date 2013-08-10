#include "FighterEntity.h"
#include "IGeometry.h"

IFighterEntity::IFighterEntity():
m_bHitEnemy( false )
{
}

void IFighterEntity::OnHitAnimationCallback( IAnimation::TEvent e, void* pData )
{
	IFighterEntity* pThisFighter = reinterpret_cast< IFighterEntity* >( pData );
	bool bEndAnimation = false;
	if( e == IAnimation::eAfterUpdate && !pThisFighter->m_bHitEnemy )
	{
		IFighterEntity* pEnemy = pThisFighter->GetFirstEnemy();
		while( pEnemy )
		{
			if( pThisFighter->IsHitIntersectEnemySphere( pEnemy ) )
			{
				if( pThisFighter->IsHitIntersectEnemyBox( pEnemy ) )
				{
					pEnemy->ReceiveHit( pThisFighter );
					pEnemy->OnReceiveHit( pThisFighter );
					pThisFighter->m_bHitEnemy = true;
				}
			}
			pEnemy = pThisFighter->GetNextEnemy();
		}
	}
	else if( e == IAnimation::eBeginRewind )
	{
		pThisFighter->GetCurrentAnimation()->RemoveAllCallback();
		pThisFighter->m_bHitEnemy = false;
		pThisFighter->OnEndHitAnimation();
	}
}

void IFighterEntity::OnHit( IFighterEntity* pAgressor )
{
	pAgressor->GetCurrentAnimation()->AddCallback( OnHitAnimationCallback, pAgressor );
}

void IFighterEntity::OnEndHitAnimation()
{
	Stand();
}

void IFighterEntity::OnReceiveHit( IFighterEntity* pEnemy )
{
	GetCurrentAnimation()->AddCallback( OnHitReceivedAnimationCallback, this );
}

void IFighterEntity::OnHitReceivedAnimationCallback( IAnimation::TEvent e, void* pData )
{
	IFighterEntity* pThisEntity = reinterpret_cast< IFighterEntity* >( pData );
	switch( e )
	{
	case IAnimation::eBeginRewind:
		pThisEntity->GetCurrentAnimation()->RemoveAllCallback();
		pThisEntity->Stand();
		break;
	}
}