#include "Human.h"
#include "IFileSystem.h"
#include "ISystems.h"
#include <algorithm>
#include "ICollisionManager.h"
#include "IGeometry.h"
#include "TimeManager.h"
#include "Scene.h"

map< string, IEntity::TAnimation >			CHuman::s_mAnimationStringToType;
map< IEntity::TAnimation, IAnimation* >		CHuman::s_mAnimationTypeToAnimation;
map< IEntity::TAnimation, float > 			CHuman::s_mOrgAnimationSpeedByType;
map< string, CHuman::TAction >				CHuman::s_mActions;
vector< CHuman* >							CHuman::s_vHumans;

CHuman::CHuman( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager , IFileSystem* pFileSystem, ICollisionManager& oCollisionManager, IGeometryManager& oGeometryManager ):
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
m_fAngleRemaining( 0.f ),
m_bArriveAtDestination( true ),
m_bPerso( false ),
m_nLife( 1000 ),
m_bHitEnemy( false )
{
	if( !m_pfnCollisionCallback )
		m_pfnCollisionCallback = OnCollision;
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

		s_mActions[ "walk" ] = Walk;
		s_mActions[ "run" ] = Run;
		s_mActions[ "stand" ] = Stand;
		s_mActions[ "HitLeftFoot" ] = HitLeftFoot;
		s_mActions[ "HitReceived" ] = HitReceived;
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

void CHuman::SetCurrentPerso( bool bPerso )
{
	m_bPerso = bPerso;
}

void CHuman::OnCollision( IEntity* pEntity )
{
	CHuman* pHuman = static_cast< CHuman* >( pEntity );
	pHuman->m_bArriveAtDestination = true;
	if( pHuman->m_eCurrentAnimationType != eStand )
		pHuman->Stand( true );
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
		ConstantLocalTranslate( CVector( 0, m_mAnimationSpeedByType[ eWalk ], 0 ) );
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
		ConstantLocalTranslate( CVector( 0.f, m_mAnimationSpeedByType[ eStand ], 0.f ) );
}

void CHuman::Run( bool bLoop )
{
	if( m_eCurrentAnimationType != eRun )
	{
		SetPredefinedAnimation( "run", bLoop );
		if( !m_bUsePositionKeys )
			ConstantLocalTranslate( CVector( 0.f, m_mAnimationSpeedByType[ eRun ], 0.f ) );
	}
}

void CHuman::HitLeftFoot( bool bLoop )
{
	SetPredefinedAnimation( "HitLeftFoot", bLoop );
	m_pScene->GetFightSystem().OnHit( this, "OrteilsG" );
}

void CHuman::HitReceived( bool bLoop )
{
	SetPredefinedAnimation( "HitReceived", bLoop );
	if( !m_bUsePositionKeys )
		ConstantLocalTranslate( CVector( 0.f, m_mAnimationSpeedByType[ eStand ], 0.f ) );
}


void CHuman::OnWalkAnimationCallback( IAnimation::TEvent e, void* pData )
{
	if( e == IAnimation::eBeginRewind )
	{
		CHuman* pHuman = reinterpret_cast< CHuman* >( pData );
		pHuman->LocalTranslate( 0, -pHuman->m_oSkeletonOffset.m_23, 0 );
	}
}

void CHuman::ReceiveHit( IFighter* pEnemy )
{	
	RunAction( "HitReceived", false );
	if( m_bPerso )
		m_nLife -= 20;
	else
		m_pScene->GetFightSystem().OnReceiveHit( this, pEnemy );
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

void CHuman::Attack( IFighter* pEnemy )
{
	HitLeftFoot( false );
}

void CHuman::SetAnimationSpeed( IEntity::TAnimation eAnimationType, float fSpeed )
{
	s_mAnimationTypeToAnimation[ eAnimationType ]->SetSpeed( fSpeed );
	m_mAnimationSpeedByType[ eAnimationType ] = s_mOrgAnimationSpeedByType[ eAnimationType ] * fSpeed;
}

IEntity::TAnimation CHuman::GetCurrentAnimationType() const
{
	return m_eCurrentAnimationType;
}

void CHuman::TurnEyesH( float fValue )
{
	m_pRightEye->Roll( fValue );
	m_pLeftEye->Roll( fValue );
}

void CHuman::TurnNeckH( float fNeckRotH )
{
	m_pNeck->Pitch( fNeckRotH );
}

ISphere* CHuman::GetBoneSphere( string sBoneName )
{
	IBone* pBone = GetPreloadedBone ( sBoneName );
	float fBoneRadius = pBone->GetBoundingBox()->GetBoundingSphereRadius();
	CVector oBoneLocalPosition, oBoneWorldPosition;
	pBone->GetWorldPosition( oBoneLocalPosition );
	oBoneWorldPosition = m_oWorldMatrix * oBoneLocalPosition;
	return m_oGeometryManager.CreateSphere( oBoneWorldPosition, fBoneRadius / 2.f );
}

IBone* CHuman::GetPreloadedBone( string sName )
{
	IBone* pBone = m_mPreloadedBones[ sName ];
	if( pBone )
		return pBone;
	m_mPreloadedBones[ sName ] = static_cast< IBone* >( GetSkeletonRoot()->GetChildBoneByName( sName ) );
	return m_mPreloadedBones[ sName ];
}

void CHuman::LookAt( float alpha )
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

float CHuman::GetDestinationAngleRemaining()
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

void CHuman::Goto( const CVector& oPosition, float fSpeed )
{
	m_oDestination = oPosition;
	m_fAngleRemaining = GetDestinationAngleRemaining();
	//LookAt( m_fAngleRemaining );
	Run( true );
	m_bArriveAtDestination = false;
}

void CHuman::Goto( IEntity* pEntity, float fSpeed )
{
	CVector oPosition;
	pEntity->GetWorldPosition( oPosition );
	Goto( oPosition, fSpeed );
}

void CHuman::Goto( IFighter* pFighter, float fSpeed )
{
	CVector oPosition;
	pFighter->GetPosition( oPosition );
	Goto( oPosition, fSpeed );
}

float CHuman::GetDistanceTo2dPoint( const CVector& oPosition )
{
	CVector oThisPosition;
	GetWorldPosition( oThisPosition );
	CVector o2DThisPosition = CVector( oThisPosition.m_x, 0, oThisPosition.m_z );
	CVector o2DPosition = CVector( oPosition.m_x, 0, oPosition.m_z );
	return ( o2DPosition - o2DThisPosition ).Norm();
}

void CHuman::UpdateGoto()
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
				Stand( true );
		}
	}
}

void CHuman::SetDestination( const CVector& oDestination )
{
	m_oDestination = oDestination;
	m_bArriveAtDestination = false;
}

int CHuman::GetLife()
{
	return m_nLife;
}

void CHuman::SetLife( int nLife )
{
	m_nLife = nLife;
}

void CHuman::IncreaseLife( int nLife )
{
	m_nLife += nLife;
}

void CHuman::Update()
{
	CEntity::Update();
	UpdateGoto();
}
