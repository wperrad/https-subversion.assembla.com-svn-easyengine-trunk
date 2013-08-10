#include "MobileEntity.h"
#include "IFileSystem.h"
#include "ISystems.h"
#include <algorithm>
#include "ICollisionManager.h"
#include "IGeometry.h"
#include "TimeManager.h"
#include "Scene.h"

map< string, IEntity::TAnimation >			CMobileEntity::s_mAnimationStringToType;
map< IEntity::TAnimation, IAnimation* >		CMobileEntity::s_mAnimationTypeToAnimation;
map< IEntity::TAnimation, float > 			CMobileEntity::s_mOrgAnimationSpeedByType;
map< string, CMobileEntity::TAction >				CMobileEntity::s_mActions;
vector< CMobileEntity* >							CMobileEntity::s_vHumans;

CMobileEntity::CMobileEntity( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager , IFileSystem* pFileSystem, ICollisionManager& oCollisionManager, IGeometryManager& oGeometryManager ):
CEntity( sFileName, oRessourceManager, oRenderer, pEntityManager, oGeometryManager, oCollisionManager ),
m_bInitSkeletonOffset( false ),
m_fMaxEyeRotationH( 15 ),
m_fMaxEyeRotationV( 15 ),
m_fMaxNeckRotationH( 45 ),
m_fMaxNeckRotationV( 15 ),
m_fEyesRotH( 0 ),
m_fEyesRotV( 0 ),
m_fNeckRotH( 0 ),
m_fNeckRotV( 0 ),
m_bPerso( false ),
m_nLife( 1000 )
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
		//s_mOrgAnimationSpeedByType[ eRun ] = -250.f;
		s_mOrgAnimationSpeedByType[ eRun ] = -4.6f;
		s_mOrgAnimationSpeedByType[ eHitLeftFoot ] = 0.f;
		s_mOrgAnimationSpeedByType[ eHitReceived ] = 0.f;		

		s_mActions[ "walk" ] = Walk;
		s_mActions[ "run" ] = Run;
		s_mActions[ "stand" ] = Stand;
		s_mActions[ "HitLeftFoot" ] = HitLeftFoot;
		s_mActions[ "PlayReceiveHit" ] = PlayReceiveHit;
	}
	for( int i = 0; i < eAnimationCount; i++ )
		m_mAnimationSpeedByType[ (TAnimation)i ] = s_mOrgAnimationSpeedByType[ (TAnimation)i ];

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
	m_pLeftEye = dynamic_cast< IEntity* >( m_pSkeletonRoot->GetChildBoneByName( "OeilG" ) );
	m_pRightEye = dynamic_cast< IEntity* >( m_pSkeletonRoot->GetChildBoneByName( "OeilD" ) );
	m_pNeck = m_pSkeletonRoot->GetChildBoneByName( "Cou" );
}

void CMobileEntity::SetCurrentPerso( bool bPerso )
{
	m_bPerso = bPerso;
}

void CMobileEntity::RunAction( string sAction, bool bLoop )
{
	s_mActions[ sAction ]( this, bLoop );
}

void CMobileEntity::SetPredefinedAnimation( string s, bool bLoop )
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

void CMobileEntity::Walk( bool bLoop )
{
	SetPredefinedAnimation( "walk", bLoop );
	if( !m_bUsePositionKeys )
		ConstantLocalTranslate( CVector( 0, m_mAnimationSpeedByType[ eWalk ], 0 ) );
	else
	{
		m_pCurrentAnimation->AddCallback( OnAnimationCallback, this );

		CKey oKey;
		m_pSkeletonRoot->GetKeyByIndex( m_pSkeletonRoot->GetKeyCount() - 1, oKey );
		m_oSkeletonOffset = m_oFirstAnimationFrameSkeletonMatrixInv * oKey.m_oLocalTM;
	}
}

void CMobileEntity::Stand( bool bLoop )
{
	SetPredefinedAnimation( "stand", bLoop );
	if( !m_bUsePositionKeys )
		ConstantLocalTranslate( CVector( 0.f, m_mAnimationSpeedByType[ eStand ], 0.f ) );
}

void CMobileEntity::Run( bool bLoop )
{
	if( m_eCurrentAnimationType != eRun )
	{
		SetPredefinedAnimation( "run", bLoop );
		if( !m_bUsePositionKeys )
			ConstantLocalTranslate( CVector( 0.f, m_mAnimationSpeedByType[ eRun ], 0.f ) );
	}
}

void CMobileEntity::HitLeftFoot( bool bLoop )
{
	SetPredefinedAnimation( "HitLeftFoot", bLoop );
	m_sCurrentHitBoneName = "OrteilsG";
	OnHit( this );
}

void CMobileEntity::PlayReceiveHit( bool bLoop )
{
	SetPredefinedAnimation( "HitReceived", bLoop );
	if( !m_bUsePositionKeys )
		ConstantLocalTranslate( CVector( 0.f, m_mAnimationSpeedByType[ eStand ], 0.f ) );
}

void CMobileEntity::PlayReceiveHit( CMobileEntity* pEntity, bool bLoop )
{
	pEntity->PlayReceiveHit( bLoop );
}

void CMobileEntity::ReceiveHit( IFighterEntity* pEnemy )
{	
	RunAction( "PlayReceiveHit", false );
	m_nLife -= 20;
}

void CMobileEntity::OnWalkAnimationCallback( IAnimation::TEvent e, void* pData )
{
	if( e == IAnimation::eBeginRewind )
	{
		CMobileEntity* pHuman = reinterpret_cast< CMobileEntity* >( pData );
		pHuman->LocalTranslate( 0, -pHuman->m_oSkeletonOffset.m_23, 0 );
	}
}

void CMobileEntity::Walk( CMobileEntity* pHuman, bool bLoop  )
{
	pHuman->Walk( bLoop );
}

void CMobileEntity::Stand( CMobileEntity* pHuman, bool bLoop  )
{
	pHuman->Stand( bLoop );
}

void CMobileEntity::Run( CMobileEntity* pHuman, bool bLoop  )
{
	pHuman->Run( bLoop );
}

void CMobileEntity::HitLeftFoot( CMobileEntity* pHuman, bool bLoop  )
{
	pHuman->HitLeftFoot( bLoop );
}

void CMobileEntity::SetAnimationSpeed( IEntity::TAnimation eAnimationType, float fSpeed )
{
	s_mAnimationTypeToAnimation[ eAnimationType ]->SetSpeed( fSpeed );
	m_mAnimationSpeedByType[ eAnimationType ] = s_mOrgAnimationSpeedByType[ eAnimationType ] * fSpeed;
}

IEntity::TAnimation CMobileEntity::GetCurrentAnimationType() const
{
	return m_eCurrentAnimationType;
}

void CMobileEntity::TurnEyesH( float fValue )
{
	m_pRightEye->Roll( fValue );
	m_pLeftEye->Roll( fValue );
}

void CMobileEntity::TurnNeckH( float fNeckRotH )
{
	m_pNeck->Pitch( fNeckRotH );
}

ISphere* CMobileEntity::GetBoneSphere( string sBoneName )
{
	IBone* pBone = GetPreloadedBone ( sBoneName );
	float fBoneRadius = pBone->GetBoundingBox()->GetBoundingSphereRadius();
	CVector oBoneLocalPosition, oBoneWorldPosition;
	pBone->GetWorldPosition( oBoneLocalPosition );
	oBoneWorldPosition = m_oWorldMatrix * oBoneLocalPosition;
	return m_oGeometryManager.CreateSphere( oBoneWorldPosition, fBoneRadius / 2.f );
}

IBone* CMobileEntity::GetPreloadedBone( string sName )
{
	IBone* pBone = m_mPreloadedBones[ sName ];
	if( pBone )
		return pBone;
	m_mPreloadedBones[ sName ] = static_cast< IBone* >( GetSkeletonRoot()->GetChildBoneByName( sName ) );
	return m_mPreloadedBones[ sName ];
}

int CMobileEntity::GetLife()
{
	return m_nLife;
}

void CMobileEntity::SetLife( int nLife )
{
	m_nLife = nLife;
}

void CMobileEntity::IncreaseLife( int nLife )
{
	m_nLife += nLife;
}

bool CMobileEntity::IsHitIntersectEnemySphere( IFighterEntity* pEnemy )
{
	CMobileEntity* pEnemyMobileEntity = dynamic_cast< CMobileEntity* >( pEnemy );
	ISphere* pBoneSphere = GetBoneSphere( m_sCurrentHitBoneName ); 
	CVector oEnemyWorldPosition;
	pEnemy->GetPosition( oEnemyWorldPosition );
	float fBoneDistance = ( pBoneSphere->GetCenter() - oEnemyWorldPosition ).Norm();
	return fBoneDistance < ( pBoneSphere->GetRadius() / 2.f + pEnemyMobileEntity->GetBoundingSphereRadius() / 2.f );
}

bool CMobileEntity::IsHitIntersectEnemyBox( IFighterEntity* pEnemy )
{
	CMobileEntity* pEnemyMobileEntity = dynamic_cast< CMobileEntity* >( pEnemy );
	IMesh* pMesh = pEnemyMobileEntity->GetMesh();
	string sAnimationName;
	pEnemyMobileEntity->GetCurrentAnimation()->GetName( sAnimationName );
	IBox* pEnemyBox = pMesh->GetAnimationBBox( sAnimationName );
	pEnemyBox->SetWorldMatrix( pEnemy->GetWorldTM() );
	ISphere* pBoneSphere = GetBoneSphere( m_sCurrentHitBoneName );
	return m_oCollisionManager.IsIntersection( *pEnemyBox, *pBoneSphere );
}

void CMobileEntity::GetPosition( CVector& oPosition )
{ 
	GetWorldPosition( oPosition ); 
}

IMesh* CMobileEntity::GetMesh()
{ 
	return dynamic_cast< IMesh* >( m_pRessource ); 
}

IAnimation*	CMobileEntity::GetCurrentAnimation()
{ 
	return m_pCurrentAnimation; 
}

IFighterEntity* CMobileEntity::GetFirstEnemy()
{
	IFighterEntity* pEntity = m_pEntityManager->GetFirstFighterEntity();
	if( pEntity == this )
		pEntity = m_pEntityManager->GetNextFighterEntity();
	return pEntity;
}

IFighterEntity* CMobileEntity::GetNextEnemy()
{
	IFighterEntity* pEntity = static_cast< IFighterEntity* >( m_pEntityManager->GetNextFighterEntity() );
	if( pEntity == this )
		pEntity = static_cast< IFighterEntity* >( m_pEntityManager->GetNextFighterEntity() );
	return pEntity;
}

void CMobileEntity::Stand()
{ 
	Stand( true ); 
}

CMatrix& CMobileEntity::GetWorldTM()
{ 
	return m_oWorldMatrix; 
}