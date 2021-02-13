#include "MobileEntity.h"
#include "IFileSystem.h"
#include "ISystems.h"
#include <algorithm>
#include "ICollisionManager.h"
#include "IGeometry.h"
#include "Utils2/TimeManager.h"
#include "Scene.h"
#include "EntityManager.h"
#include "IGUIManager.h"

map< string, IEntity::TAnimation >			CMobileEntity::s_mAnimationStringToType;
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
m_bFirstUpdate(true)
{
	m_sTypeName = "Human";
	if( s_mAnimationStringToType.size() == 0 )
	{
		s_mAnimationStringToType[ "walk" ] = eWalk;
		s_mAnimationStringToType[ "run" ] = eRun;
		s_mAnimationStringToType[ "stand" ] = eStand;
		s_mAnimationStringToType[ "HitLeftFoot" ] = eHitLeftFoot;
		s_mAnimationStringToType["jump"] = eJump;
		s_mAnimationStringToType["dying"] = eDying;
		s_mAnimationStringToType["guard"] = eGuard;
		s_mOrgAnimationSpeedByType[ eWalk ] = -1.6f;
		s_mOrgAnimationSpeedByType[ eStand ] = 0.f;
		s_mOrgAnimationSpeedByType[eRun] = -7.f;
		s_mOrgAnimationSpeedByType[ eHitLeftFoot ] = 0.f;
		s_mOrgAnimationSpeedByType[ eHitReceived ] = 0.f;
		s_mOrgAnimationSpeedByType[eDying] = 0.f;
		s_mOrgAnimationSpeedByType[eGuard] = 0.f;

		s_mActions[ "walk" ] = Walk;
		s_mActions[ "run" ] = Run;
		s_mActions[ "stand" ] = Stand;
		//s_mActions[ "HitLeftFoot" ] = HitLeftFoot;
		s_mActions[ "PlayReceiveHit" ] = PlayReceiveHit;
		s_mActions[ "jump"] = Jump;
		s_mActions["dying"] = Dying;
		s_mActions["guard"] = Guard;
	}
	for( int i = 0; i < eAnimationCount; i++ )
		m_mAnimationSpeedByType[ (TAnimation)i ] = s_mOrgAnimationSpeedByType[ (TAnimation)i ];

	m_oBody.m_fWeight = 1.f;
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
			m_mAnimations[ s_mAnimationStringToType[ sAnimationType ] ] = m_mAnimation[ sFileNameLow ] ;
		}
		while( FindNextFileA( hFirst, &oData ) );
	}
	s_vHumans.push_back( this );
	m_pLeftEye = dynamic_cast< IEntity* >( m_pSkeletonRoot->GetChildBoneByName( "OeilG" ) );
	m_pRightEye = dynamic_cast< IEntity* >( m_pSkeletonRoot->GetChildBoneByName( "OeilD" ) );
	m_pNeck = m_pSkeletonRoot->GetChildBoneByName( "Cou" );

	m_pfnCollisionCallback = OnCollision;
	m_sAttackBoneName = "OrteilsG";
}


void CMobileEntity::OnCollision(CEntity* pThis, vector<CEntity*> entities)
{
	for (int i = 0; i < entities.size(); i++) {
		CEntity* pEntity = entities[i];
		IMesh* pMesh = static_cast<IMesh*>(pThis->GetRessource());
		ICollisionMesh* pCollisionMesh = pEntity ? pEntity->GetCollisionMesh() : NULL;
		if (pCollisionMesh)
			pThis->LinkAndUpdateMatrices(pEntity);
	}
}

void CMobileEntity::ManageGravity()
{
	m_oBody.Update();
	if (m_oBody.m_fWeight > 0.f)
	{
		float x, y, z;
		m_oLocalMatrix.GetPosition(x, y, z);
		int nDelta = CTimeManager::Instance()->GetTimeElapsedSinceLastUpdate();
		m_vNextLocalTranslate.m_y += m_oBody.m_oSpeed.m_y * (float)nDelta / 1000.f;
	}
}

void CMobileEntity::DisplayPlayerPosition()
{
	string sEntityName;
	GetEntityName(sEntityName);
	if (sEntityName.find("perso") != -1) {
		CVector pos;
		GetWorldPosition(pos);
		ostringstream oss;
		oss << "Perso position = (" << pos.m_x << ", " << pos.m_y << ", " << pos.m_z << ")";
		m_pEntityManager->GetGUIManager()->Print(oss.str(), 1000, 10);
	}
}

void CMobileEntity::Update()
{
	ManageGravity();
	UpdateCollision();

	if (m_pCurrentAnimation)
		m_pCurrentAnimation->Update();

	UpdateWorldMatrix();
	UpdateChildren();
	SendBonesToShader();

	m_oWorldMatrix *= m_oScaleMatrix;
	m_oRenderer.SetModelMatrix(m_oWorldMatrix);
	UpdateRessource();

	if (m_bDrawBoundingBox && m_pBoundingGeometry)
		UpdateBoundingBox();

	m_vNextLocalTranslate.Fill(0, 0, 0, 1);
	DisplayPlayerPosition();
}

void CMobileEntity::UpdateCollision()
{
	float h = GetHeight();

	m_vNextLocalTranslate += m_vConstantLocalTranslate;

	CMatrix backupLocal = m_oLocalMatrix;
	backupLocal.m_13 -= h / 2.f - m_fMaxStepHeight;

	CNode::LocalTranslate(m_vNextLocalTranslate.m_x, m_vNextLocalTranslate.m_y, m_vNextLocalTranslate.m_z);
	CNode::UpdateWorldMatrix();
	CMatrix oLocalMatrix = m_oLocalMatrix;
	oLocalMatrix.m_13 -= h / 2.f - m_fMaxStepHeight;

	vector<CEntity*> entities;
	if (!m_bFirstUpdate)
		GetEntitiesCollision(entities);
	else
		m_bFirstUpdate = false;

	CVector localPos;
	oLocalMatrix.GetPosition(localPos);
	
	CVector directriceLine = m_vNextLocalTranslate;
	CVector first = backupLocal.GetPosition();
	CVector last = localPos;
	CVector firstBottom = first;
	CVector lastBottom = last;
	CVector R = last;
	bool bCollision = false;
	float fMaxHeight = -999999.f;
	for (int i = 0; i < entities.size(); i++) {
		CEntity* pEntity = entities[i];
		pEntity->GetBoundingGeometry()->SetTM(pEntity->GetLocalMatrix());
		IGeometry* firstBox = GetBoundingGeometry()->Duplicate();
		firstBox->SetTM(backupLocal);
		IGeometry* lastBox = GetBoundingGeometry();
		lastBox->SetTM(oLocalMatrix);
		IGeometry::TFace collisionFace = IGeometry::eNone;
		collisionFace = pEntity->GetBoundingGeometry()->GetReactionYAlignedBox(*firstBox, *lastBox, R);
		if (collisionFace != IBox::eNone) {
			lastBottom = R;
			last = R;
			last.m_y += h / 2.f;
			bCollision = true;
			if (collisionFace == IBox::eYPositive) {
				if (fMaxHeight < last.m_y)
					fMaxHeight = last.m_y;
				else
					last.m_y = fMaxHeight;
				m_oBody.m_oSpeed.m_y = 0;
			}
			else {
				last.m_y -= m_fMaxStepHeight;
			}
		}
	}
	// Ground collision
	const float margin = 7.f;
	float fGroundHeight = static_cast<CEntity*>(m_pParent)->GetGroundHeight(localPos.m_x, localPos.m_z) + margin;
	float fEntityY = last.m_y - h / 2.f;
	if (fEntityY <= fGroundHeight + CBody::GetEpsilonError()) {
		m_oBody.m_oSpeed.m_x = 0;
		m_oBody.m_oSpeed.m_y = 0;
		m_oBody.m_oSpeed.m_z = 0;
		float newY = fGroundHeight + h / 2.f;
		last.m_y = newY;
	}
	SetLocalPosition(last);

	// Still into parent ?
	CEntity* pParentEntity = static_cast<CEntity*>(m_pParent);
	if (!TestEntityCollision(pParentEntity)) {
		CEntity* pEntity = static_cast<CEntity*>(m_pParent->GetParent());
		if (pEntity)
			LinkAndUpdateMatrices(pEntity);
	}

	if (bCollision && m_pfnCollisionCallback) {
		m_pfnCollisionCallback(this, entities);
	}
}

const string& CMobileEntity::GetAttackBoneName()
{
	return m_sAttackBoneName;
}

void CMobileEntity::WearArmor(string armorName)
{
	string path = "Armors\\" + armorName + "\\";
	const int count = 8;
	string arrayPiece[count] = {"cuirasse", "jupe", "JambiereD", "JambiereG", "BrassiereD", "BrassiereG", "EpauletteD", "EpauletteG" };
	string arrayBone[count] = { "Cervicales", "Bassin", "TibiasD", "TibiasG", "AvantBrasD", "AvantBrasG", "EpauleD", "EpauleG" };

	for (int i = 0; i < count; i++) {
		CEntity* pArmorPiece = dynamic_cast<CEntity*>(m_pEntityManager->CreateEntity(path + arrayPiece[i] + ".bme", "", m_oRenderer));
		IBone* pBone = dynamic_cast<IBone*>(m_pSkeletonRoot->GetChildBoneByName(arrayBone[i]));
		if (pBone)
			LinkEntityToBone(pArmorPiece, pBone, ePreserveChildRelativeTM);
	}
}

void CMobileEntity::RunAction( string sAction, bool bLoop )
{
	map<string, CMobileEntity::TAction>::iterator itAction = s_mActions.find(sAction);
	if (itAction != s_mActions.end())
		itAction->second(this, bLoop);
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
	if (m_eCurrentAnimationType != eWalk)
	{
		SetPredefinedAnimation("walk", bLoop);
		if (!m_bUsePositionKeys)
			ConstantLocalTranslate(CVector(0.f, 0.f, -m_mAnimationSpeedByType[eWalk]));
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
			ConstantLocalTranslate( CVector( 0.f, 0.f, -m_mAnimationSpeedByType[eRun]) );
	}
}

void CMobileEntity::Jump(bool bLoop)
{
	if (m_eCurrentAnimationType != eJump)
	{
		//SetPredefinedAnimation("jump", bLoop);
		//if (!m_bUsePositionKeys)
		//ConstantLocalTranslate(CVector(0.f, m_mAnimationSpeedByType[eJump], 0.f));
		m_oBody.m_oSpeed.m_y = 2000;
	}
}

void CMobileEntity::Die()
{
	if (m_eCurrentAnimationType != eDying) {
		SetPredefinedAnimation("dying", false);
		m_pCurrentAnimation->AddCallback(OnDyingCallback, this);
	}
}

void CMobileEntity::Yaw(float fAngle)
{
	if(GetLife() > 0)
		CNode::Yaw(fAngle);
}

void CMobileEntity::Pitch(float fAngle)
{
	if (GetLife() > 0)
		CNode::Pitch(fAngle);
}

void CMobileEntity::Roll(float fAngle)
{
	if (GetLife() > 0)
		CNode::Roll(fAngle);
}

void CMobileEntity::OnDyingCallback(IAnimation::TEvent e, void* pEntity)
{
	CMobileEntity* pMobileEntity = (CMobileEntity*)pEntity;
	pMobileEntity->m_eCurrentAnimationType = eNone;
}

void CMobileEntity::PlayHitAnimation()
{
	SetPredefinedAnimation("HitLeftFoot", false);
}

void CMobileEntity::PlayReceiveHit( bool bLoop )
{
	SetPredefinedAnimation( "HitReceived", bLoop );
	if( !m_bUsePositionKeys )
		ConstantLocalTranslate( CVector( 0.f, m_mAnimationSpeedByType[ eHitReceived ], 0.f ) );
}

void CMobileEntity::PlayReceiveHit( CMobileEntity* pEntity, bool bLoop )
{
	pEntity->PlayReceiveHit( bLoop );
}

void CMobileEntity::PlayReceiveHit()
{
	RunAction("PlayReceiveHit", false);
}

void CMobileEntity::Guard()
{
	//RunAction("guard", false);
	if (m_eCurrentAnimationType != eGuard) {
		SetPredefinedAnimation("guard", false);
	}
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

void CMobileEntity::Jump(CMobileEntity* pHuman, bool bLoop)
{
	pHuman->Jump(bLoop);
}

void CMobileEntity::Dying(CMobileEntity* pHuman, bool bLoop)
{
	pHuman->Die();
}

void CMobileEntity::Guard(CMobileEntity* pHuman, bool bLoop)
{
	pHuman->Guard();
}

void CMobileEntity::SetAnimationSpeed( IEntity::TAnimation eAnimationType, float fSpeed )
{
	m_mAnimations[ eAnimationType ]->SetSpeed(fSpeed);
	m_mAnimationSpeedByType[ eAnimationType ] = s_mOrgAnimationSpeedByType[ eAnimationType ] * fSpeed;
}

float CMobileEntity::GetAnimationSpeed(IEntity::TAnimation eAnimationType)
{
	return m_mAnimations[eAnimationType]->GetSpeed();
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
	CVector oBoneWorldPosition;
	pBone->GetWorldPosition(oBoneWorldPosition);	
	return m_oGeometryManager.CreateSphere( oBoneWorldPosition, fBoneRadius / 2.f );
}

void CMobileEntity::AddSpeed(float x, float y, float z)
{
	m_oBody.m_oSpeed.m_x += x;
	m_oBody.m_oSpeed.m_y += y;
	m_oBody.m_oSpeed.m_z += z;
}

IBone* CMobileEntity::GetPreloadedBone( string sName )
{
	IBone* pBone = m_mPreloadedBones[ sName ];
	if( pBone )
		return pBone;
	m_mPreloadedBones[ sName ] = static_cast< IBone* >( GetSkeletonRoot()->GetChildBoneByName( sName ) );
	return m_mPreloadedBones[ sName ];
}


void CMobileEntity::GetPosition( CVector& oPosition ) const
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

IBox* CMobileEntity::GetBoundingBox()
{
	IMesh* pMesh = GetMesh();
	string sAnimationName;
	m_pCurrentAnimation->GetName( sAnimationName );
	return pMesh->GetAnimationBBox( sAnimationName );
}
