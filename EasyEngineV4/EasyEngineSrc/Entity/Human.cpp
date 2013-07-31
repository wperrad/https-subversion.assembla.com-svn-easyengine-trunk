#include "Human.h"
#include "IFileSystem.h"
#include "ISystems.h"
#include <algorithm>
#include "ICollisionManager.h"
#include "IGeometry.h"

map< string, IEntity::TAnimation >			CHuman::s_mAnimationStringToType;
map< IEntity::TAnimation, IAnimation* >		CHuman::s_mAnimationTypeToAnimation;
map< IEntity::TAnimation, float > 			CHuman::s_mAnimationSpeedByType;
map< IEntity::TAnimation, float > 			CHuman::s_mOrgAnimationSpeedByType;
map< string, CHuman::TAction >				CHuman::s_mActions;
vector< CHuman* >							CHuman::s_vHumans;

CHuman::CHuman( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager , IFileSystem* pFileSystem, ICollisionManager& oCollisionManager, IGeometryManager& oGeometryManager ):
CEntity( sFileName, oRessourceManager, oRenderer, pEntityManager, oGeometryManager, oCollisionManager ),
m_bInitSkeletonOffset( false )
{
	m_sTypeName = "Human";
	if( s_mAnimationStringToType.size() == 0 )
	{
		s_mAnimationStringToType[ "walk" ] = eWalk;
		s_mAnimationStringToType[ "run" ] = eRun;
		s_mAnimationStringToType[ "stand" ] = eStand;
		s_mAnimationStringToType[ "HitLeftFoot" ] = eHitLeftFoot;
		s_mOrgAnimationSpeedByType[ eWalk ] = -1.6f;
		s_mOrgAnimationSpeedByType[ eStand ] = 0.f;
		s_mOrgAnimationSpeedByType[ eRun ] = -3.5f;
		s_mOrgAnimationSpeedByType[ eHitLeftFoot ] = 0.f;
		s_mOrgAnimationSpeedByType[ eHitReceived ] = 0.f;

		for( int i = 0; i < eAnimationCount; i++ )
			s_mAnimationSpeedByType[ (TAnimation)i ] = s_mOrgAnimationSpeedByType[ (TAnimation)i ];

		s_mActions[ "walk" ] = Walk;
		s_mActions[ "run" ] = Run;
		s_mActions[ "stand" ] = Stand;
		s_mActions[ "HitLeftFoot" ] = HitLeftFoot;
		s_mActions[ "HitReceived" ] = HitReceived;
	}

	m_oBody.m_fWeight = 1.f;
	Pitch( -90.f );
	int nDotPos = (int)sFileName.find( "." );
	m_sFileNameWithoutExt = sFileName.substr( 0, nDotPos );
	string sMask = m_sFileNameWithoutExt + "*" + ".bke";
	WIN32_FIND_DATAA oData;
	HANDLE hFirst = pFileSystem->FindFirstFile_EE( sMask, oData );
	
	if( hFirst != INVALID_HANDLE_VALUE )
	{
		do
		{
			string sFileNameFound = oData.cFileName;
			string sFileNameLow = oData.cFileName;
			transform( sFileNameFound.begin(), sFileNameFound.end(), sFileNameLow.begin(), tolower );
			AddAnimation( sFileNameLow );
			string sAnimationType = sFileNameLow.substr( m_sFileNameWithoutExt.size() + 1, sFileNameLow.size() - m_sFileNameWithoutExt.size() - 5 );
			s_mAnimationTypeToAnimation[ s_mAnimationStringToType[ sAnimationType ] ] = m_mAnimation[ sFileNameLow ] ;
		}
		while( FindNextFileA( hFirst, &oData ) );
	}
	s_vHumans.push_back( this );
}

void CHuman::RunAction( string sAction, bool bLoop )
{
	s_mActions[ sAction ]( this, bLoop );
}

void CHuman::SetPredefinedAnimation( string s, bool bLoop )
{
	IMesh* pMesh = static_cast< IMesh* >( m_pRessource );
	string sAnimationName = m_sFileNameWithoutExt + "_" + s + ".bke";
	string sAnimationNameLow = sAnimationName;
	transform( sAnimationName.begin(), sAnimationName.end(), sAnimationNameLow.begin(), tolower );
	SetCurrentAnimation( sAnimationNameLow );
	if( !m_pCurrentAnimation )
	{
		string sMessage = string( "Erreur : fichier \"" ) + sAnimationNameLow + "\" manquant";
		CFileNotFoundException e( sMessage );
		e.m_sFileName = sAnimationNameLow;
		throw e;
	}
	m_pCurrentAnimation->Play( bLoop );
	m_eCurrentAnimationType = s_mAnimationStringToType[ s ];
}

void CHuman::Walk( bool bLoop )
{
	SetPredefinedAnimation( "walk", bLoop );
	if( !m_bUsePositionKeys )
		ConstantLocalTranslate( CVector( 0, s_mAnimationSpeedByType[ eWalk ], 0 ) );
	else
	{
		m_pCurrentAnimation->AddCallback( OnAnimationCallback, this );

		CKey oKey;
		m_pSkeletonRoot->GetKeyByIndex( m_pSkeletonRoot->GetKeyCount() - 1, oKey );
		m_oSkeletonOffset = m_oFirstAnimationFrameSkeletonMatrixInv * oKey.m_oLocalTM;
	}
}

void CHuman::Stand( bool bLoop )
{
	SetPredefinedAnimation( "stand", bLoop );
	if( !m_bUsePositionKeys )
		ConstantLocalTranslate( CVector( 0.f, s_mAnimationSpeedByType[ eStand ], 0.f ) );
}

void CHuman::Run( bool bLoop )
{
	SetPredefinedAnimation( "run", bLoop );
	if( !m_bUsePositionKeys )
		ConstantLocalTranslate( CVector( 0.f, s_mAnimationSpeedByType[ eRun ], 0.f ) );
}

void CHuman::HitLeftFoot( bool bLoop )
{
	SetPredefinedAnimation( "HitLeftFoot", bLoop );
	m_pCurrentAnimation->AddCallback( OnHitLeftFootAnimationCallback, this );
}

void CHuman::HitReceived( bool bLoop )
{
	SetPredefinedAnimation( "HitReceived", bLoop );
	if( !m_bUsePositionKeys )
		ConstantLocalTranslate( CVector( 0.f, s_mAnimationSpeedByType[ eStand ], 0.f ) );
	m_pCurrentAnimation->AddCallback( OnHitReceivedCallback, this );
}

IBone* CHuman::GetPreloadedBone( string sName )
{
	IBone* pBone = m_mPreloadedBones[ sName ];
	if( pBone )
		return pBone;
	m_mPreloadedBones[ sName ] = static_cast< IBone* >( m_pSkeletonRoot->GetChildBoneByName( sName ) );
	return m_mPreloadedBones[ sName ];
}

void CHuman::OnHitLeftFootAnimationCallback( IAnimation::TEvent e, void* pData )
{
	CHuman* pCurrentHuman = reinterpret_cast< CHuman* >( pData );
	if( e == IAnimation::eAfterUpdate )
	{
		for( unsigned int i = 0; i < s_vHumans.size(); i++ )
		{
			CHuman* pEnnemy = s_vHumans[ i ];
			if( pCurrentHuman == pEnnemy )
				continue;

			IBone* pLeftFoot = pCurrentHuman->GetPreloadedBone ( "OrteilsG"  );
			float fFootRadius = pLeftFoot->GetBoundingBox()->GetBoundingSphereRadius();
			CVector oFootLocalPosition, oFootWorldPosition, oEnemyWorldPosition;
			pLeftFoot->GetWorldPosition( oFootLocalPosition );
			oFootWorldPosition = pCurrentHuman->m_oWorldMatrix * oFootLocalPosition;

			pEnnemy->GetWorldPosition( oEnemyWorldPosition );
			float fFootDistance = ( oFootWorldPosition - oEnemyWorldPosition ).Norm();
			if( fFootDistance < fFootRadius / 2.f + pEnnemy->m_fBoundingSphereRadius / 2.f )
			{
				IMesh* pMesh = static_cast< IMesh* >( pEnnemy->GetRessource() );
				string sAnimationName;
				pEnnemy->GetCurrentAnimation()->GetName( sAnimationName );
				int nExtPos = sAnimationName.find( ".bke" );
				string sAnimationNameLowWithoutExt = sAnimationName.substr( 0, nExtPos );
				IBox* pEnemyBox = pMesh->GetAnimationBBox( sAnimationNameLowWithoutExt );
				pEnemyBox->SetWorldMatrix( pEnnemy->m_oWorldMatrix );
				ISphere* pSphere = pCurrentHuman->m_oGeometryManager.CreateSphere( oFootWorldPosition, fFootRadius / 2.f );				
				if( pCurrentHuman->m_oCollisionManager.IsIntersection( *pEnemyBox, *pSphere ) )
				{
					pEnnemy->ReceiveHit( pCurrentHuman );
					pCurrentHuman->m_pCurrentAnimation->RemoveCallback( OnHitLeftFootAnimationCallback );
				}
			}
		}
		
	}
	else if( e == IAnimation::eBeginRewind )
	{
		pCurrentHuman->m_pCurrentAnimation->RemoveCallback( OnHitLeftFootAnimationCallback );
		pCurrentHuman->Stand( true );
	}
}

void CHuman::OnHitReceivedCallback( IAnimation::TEvent e, void* pData )
{
	CHuman* pCurrentHuman = reinterpret_cast< CHuman* >( pData );
	if( e == IAnimation::eBeginRewind )
	{
		pCurrentHuman->m_pCurrentAnimation->RemoveCallback( OnHitReceivedCallback );
		pCurrentHuman->Stand( true );
	}
}

void CHuman::OnWalkAnimationCallback( IAnimation::TEvent e, void* pData )
{
	if( e == IAnimation::eBeginRewind )
	{
		CHuman* pHuman = reinterpret_cast< CHuman* >( pData );
		pHuman->LocalTranslate( 0, -pHuman->m_oSkeletonOffset.m_23, 0 );
	}
}

void CHuman::ReceiveHit( CHuman* pHuman )
{
	RunAction( "HitReceived", false );
}

void CHuman::Walk( CHuman* pHuman, bool bLoop  )
{
	pHuman->Walk( bLoop );
}

void CHuman::Stand( CHuman* pHuman, bool bLoop  )
{
	pHuman->Stand( bLoop );
}

void CHuman::Run( CHuman* pHuman, bool bLoop  )
{
	pHuman->Run( bLoop );
}

void CHuman::HitLeftFoot( CHuman* pHuman, bool bLoop  )
{
	pHuman->HitLeftFoot( bLoop );
}

void CHuman::HitReceived( CHuman* pHuman, bool bLoop )
{
	pHuman->HitReceived( bLoop );
}

void CHuman::SetAnimationSpeed( IEntity::TAnimation eAnimationType, float fSpeed )
{
	s_mAnimationTypeToAnimation[ eAnimationType ]->SetSpeed( fSpeed );
	//ConstantLocalTranslate( CVector( 0.f, s_mAnimationSpeedByType[ eAnimationType ] * fSpeed, 0.f ) );
	s_mAnimationSpeedByType[ eAnimationType ] = s_mOrgAnimationSpeedByType[ eAnimationType ] * fSpeed;
}

IEntity::TAnimation CHuman::GetCurrentAnimationType() const
{
	return m_eCurrentAnimationType;
}