#ifndef MOBILEENTITY_H
#define MOBILEENTITY_H

#include "Entity.h"
#include "FighterEntity.h"

class CMobileEntity : public CEntity, public virtual IFighterEntity
{

public:
	CMobileEntity(EEInterface& oInterface, string sFileName, string sID);

	float						GetAnimationSpeed(IEntity::TAnimation eAnimationType);
	void						GetEntityInfos(ILoader::CObjectInfos*& pInfos) override;
	void						BuildFromInfos(const ILoader::CObjectInfos& infos, CEntity* pParent) override;
	void						SetAnimationSpeed(TAnimation eAnimationType, float fSpeed);
	TAnimation					GetCurrentAnimationType() const;
	void						RunAction(string sAction, bool bLoop);
	void						Die();
	void						WearArmorToDummy(string armorName);
	void						WearShoes(string shoesName);
	void						Yaw(float fAngle);
	void						Pitch(float fAngle);
	void						Roll(float fAngle);
	void						ManageGravity();
	void						Update();
	void						UpdateCollision();
	IAnimation*					GetCurrentAnimation();
	static void					InitStatics();

	static map<string, IEntity::TAnimation>	s_mStringToAnimation;

protected:
	typedef void (*TAction)( CMobileEntity*, bool );

	EEInterface&								m_oInterface;
	string										m_sFileNameWithoutExt;
	bool										m_bInitSkeletonOffset;
	CMatrix										m_oSkeletonOffset;
	TAnimation									m_eCurrentAnimationType;
	map< string, IBone* >						m_mPreloadedBones;
	IEntity*									m_pLeftEye;
	IEntity*									m_pRightEye;
	IBone*										m_pNeck;
	float										m_fEyesRotH;
	float										m_fEyesRotV;
	float										m_fNeckRotH;
	float										m_fNeckRotV;
	const float									m_fMaxEyeRotationH;
	const float									m_fMaxEyeRotationV;
	const float									m_fMaxNeckRotationH;
	const float									m_fMaxNeckRotationV;
	map< IEntity::TAnimation, IAnimation* >		m_mAnimations;
	map< TAnimation, float >					m_mAnimationSpeedByType;
	string										m_sAttackBoneName;
	CVector										m_vNextLocalTranslate;
	bool										m_bFirstUpdate;
	string										m_sStandAnimation;
	

	static map< string, TAction >				s_mActions;
	static map< string, TAnimation >			s_mAnimationStringToType;
	static map< TAnimation, float >				s_mOrgAnimationSpeedByType;
	static vector< CMobileEntity* >				s_vHumans;

	void					SetPredefinedAnimation( string s, bool bLoop );
	void 					Walk( bool bLoop );
	void 					Stand( bool bLoop );
	void 					Run( bool bLoop );
	void 					Jump(bool bLoop);
	void					PlayReceiveHit( bool bLoop );
	void					Stand();
	void					PlayReceiveHit();
	void					PlayHitAnimation();
	void					Guard();
	void					TurnEyesH( float fValue );
	void					TurnNeckH( float f );
	IBone*					GetPreloadedBone( string sName );
	void					GetPosition( CVector& oPosition ) const;
	IMesh*					GetMesh();
	IFighterEntity*			GetFirstEnemy();
	IFighterEntity*			GetNextEnemy();
	CMatrix&				GetWorldTM();
	float					GetBoundingSphereRadius(){ return m_fBoundingSphereRadius; }
	IBox*					GetBoundingBox();
	ICollisionManager&		GetCollisionManager(){ return m_oCollisionManager; }
	ISphere*				GetBoneSphere( string sBoneName );
	void					AddSpeed(float x, float y, float z);
	const string&			GetAttackBoneName();
	IGeometry*				GetBoundingGeometry() override;

	static void				OnWalkAnimationCallback( IAnimation::TEvent e, void* pEntity );
	static void 			Walk( CMobileEntity*, bool bLoop );
	static void 			Stand( CMobileEntity*, bool bLoop );
	static void 			Run( CMobileEntity*, bool bLoop );
	static void				Jump(CMobileEntity* pHuman, bool bLoop);
	static void				Dying(CMobileEntity* pHuman, bool bLoop);
	static void				Guard(CMobileEntity* pHuman, bool bLoop);
	static void 			PlayReceiveHit( CMobileEntity* pHuman, bool bLoop );
	static void 			OnCollision(CEntity* pThis, vector<INode*> entities);
	static void				OnDyingCallback(IAnimation::TEvent e, void* data);
};

#endif // MOBILEENTITY_H