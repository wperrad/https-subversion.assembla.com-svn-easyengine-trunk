#include "FightSystem.h"
#include "IEntity.h"
#include "IGeometry.h"
#include "ICollisionManager.h"
#include "IAEntity.h"

CFightSystem* CFightSystem::s_pInstance = NULL;

CFightSystem::CFightSystem( IEntityManager& oEntityManager ):
m_oEntityManager( oEntityManager )
{
	s_pInstance = this;
}

void CFightSystem::OnHitAnimationCallback( IAnimation::TEvent e, void* pData )
{
	IAEntity* pThisFighter = reinterpret_cast< IAEntity* >( pData );
	bool bEndAnimation = false;
	bool bHitEnemy = false;
	if( e == IAnimation::eAfterUpdate && !pThisFighter->m_bHitEnemy )
	{
		IAEntity* pEnemy = s_pInstance->m_oEntityManager.GetFirstIAEntity();
		while( pEnemy )
		{
			if( pThisFighter == pEnemy )
			{
				pEnemy = s_pInstance->m_oEntityManager.GetNextIAEntity();
				continue;
			}

			ISphere* pBoneSphere = pThisFighter->GetBoneSphere( pThisFighter->m_sCurrentHitBoneName  ); 
			CVector oEnemyWorldPosition;
			pEnemy->GetPosition( oEnemyWorldPosition );
			float fFootDistance = ( pBoneSphere->GetCenter() - oEnemyWorldPosition ).Norm();
			if( fFootDistance < pBoneSphere->GetRadius() / 2.f + pEnemy->GetBoundingSphereRadius() / 2.f )
			{
				IMesh* pMesh = pEnemy->GetMesh();
				string sAnimationName;
				pEnemy->GetCurrentAnimation()->GetName( sAnimationName );
				IBox* pEnemyBox = pMesh->GetAnimationBBox( sAnimationName );
				pEnemyBox->SetTM( pEnemy->GetWorldTM() );
				if( pThisFighter->GetCollisionManager().IsIntersection( *pEnemyBox, *pBoneSphere ) )
				{
					pEnemy->ReceiveHit( pThisFighter );
					pThisFighter->m_bHitEnemy = true;
				}
			}
			pEnemy = s_pInstance->m_oEntityManager.GetNextIAEntity();
		}
	}
	else if( e == IAnimation::eBeginRewind )
	{
		pThisFighter->GetCurrentAnimation()->RemoveAllCallback();
		if( pThisFighter->m_eFightState == IAEntity::eLaunchingAttack )
			pThisFighter->m_eFightState = IAEntity::eEndLaunchAttack;
		pThisFighter->Stand();
		pThisFighter->m_bHitEnemy = false;
	}
}

void CFightSystem::OnHitReceivedCallback( IAnimation::TEvent e, void* pData )
{
	IAEntity* pThisFighter = reinterpret_cast< IAEntity* >( pData );
	switch( e )
	{
	case IAnimation::eBeginRewind:
		pThisFighter->GetCurrentAnimation()->RemoveCallback( OnHitReceivedCallback );
		if( pThisFighter->m_eFightState == IAEntity::eReceivingHit )
			pThisFighter->m_eFightState = IAEntity::eBeginPrepareForNextAttack;
		else
			pThisFighter->Stand();
		break;
	}
}

void CFightSystem::Update()
{
	IAEntity* pFighter = m_oEntityManager.GetFirstIAEntity();
	while( pFighter )
	{
		pFighter->UpdateFightState();
		pFighter = m_oEntityManager.GetNextIAEntity();
	}
}

void CFightSystem::OnHit( IAEntity* pAgressor, string sHitBoneName )
{
	pAgressor->m_sCurrentHitBoneName = sHitBoneName;
	pAgressor->GetCurrentAnimation()->AddCallback( OnHitAnimationCallback, pAgressor );
}

void CFightSystem::OnReceiveHit( IAEntity* pAssaulted, IAEntity* pAgressor )
{
	pAssaulted->m_pCurrentEnemy = pAgressor;
	if( pAssaulted->m_eFightState == IAEntity::eNoFight )
		pAssaulted->m_eFightState = IAEntity::eBeginHitReceived;
	else if( pAssaulted->m_eFightState == IAEntity::eLaunchingAttack )
		pAssaulted->m_eFightState = IAEntity::eEndLaunchAttack;
	pAssaulted->GetCurrentAnimation()->AddCallback( OnHitReceivedCallback, pAssaulted );
}