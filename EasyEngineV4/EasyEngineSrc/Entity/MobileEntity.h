#ifndef MOBILEENTITY_H
#define MOBILEENTITY_H

#include "Entity.h"
#include "FighterEntity.h"


class CMobileEntity : public CEntity, public virtual IFighterEntity
{

protected:
	typedef void (*TAction)( CMobileEntity*, bool );
	string										m_sFileNameWithoutExt;
	bool										m_bInitSkeletonOffset;
	CMatrix										m_oSkeletonOffset;
	TAnimation									m_eCurrentAnimationType;
	map< string, IBone* >						m_mPreloadedBones;
	IEntity*									m_pLeftEye;
	IEntity*									m_pRightEye;
	CNode*										m_pNeck;
	float										m_fEyesRotH;
	float										m_fEyesRotV;
	float										m_fNeckRotH;
	float										m_fNeckRotV;
	const float									m_fMaxEyeRotationH;
	const float									m_fMaxEyeRotationV;
	const float									m_fMaxNeckRotationH;
	const float									m_fMaxNeckRotationV;
	bool										m_bPerso;
	int											m_nLife;
	map< IEntity::TAnimation, IAnimation* >		m_mAnimations;
	map< TAnimation, float >					m_mAnimationSpeedByType;
	

	static map< string, TAction >				s_mActions;
	static map< string, TAnimation >			s_mAnimationStringToType;
	static map< TAnimation, float >				s_mOrgAnimationSpeedByType;
	static vector< CMobileEntity* >				s_vHumans;

	void					SetPredefinedAnimation( string s, bool bLoop );
	void 					Walk( bool bLoop );
	void 					Stand( bool bLoop );
	void 					Run( bool bLoop );
	void 					Jump(bool bLoop);
	void					HitLeftFoot( bool bLoop );
	void					PlayReceiveHit( bool bLoop );
	void					Stand();
	void					ReceiveHit( IFighterEntity* pEnemy );	
	void					TurnEyesH( float fValue );
	void					TurnNeckH( float f );
	IBone*					GetPreloadedBone( string sName );
	int						GetLife();
	void					SetLife( int nLife );
	void					IncreaseLife( int nLife );	
	bool					HasAlreadyHitEnemy();
	void					GetPosition( CVector& oPosition ) const;
	IMesh*					GetMesh();
	IAnimation*				GetCurrentAnimation();
	IFighterEntity*			GetFirstEnemy();
	IFighterEntity*			GetNextEnemy();
	CMatrix&				GetWorldTM();
	float					GetBoundingSphereRadius(){ return m_fBoundingSphereRadius; }
	IBox*					GetBoundingBox();
	ICollisionManager&		GetCollisionManager(){ return m_oCollisionManager; }
	ISphere*				GetBoneSphere( string sBoneName );
	void					AddSpeed(float x, float y, float z);
	void					UpdateCollision();

	static void				OnWalkAnimationCallback( IAnimation::TEvent e, void* pEntity );
	static void 			Walk( CMobileEntity*, bool bLoop );
	static void 			Stand( CMobileEntity*, bool bLoop );
	static void 			Run( CMobileEntity*, bool bLoop );
	static void				Jump(CMobileEntity* pHuman, bool bLoop);
	static void				HitLeftFoot( CMobileEntity* pHuman, bool bLoop );
	static void				Dying(CMobileEntity* pHuman, bool bLoop);
	static void 			PlayReceiveHit( CMobileEntity* pHuman, bool bLoop );
	static void 			OnCollision(CEntity* pThis, CEntity* pEntity);
	static void				OnDyingCallback(IAnimation::TEvent e, void* data);


public:
	CMobileEntity( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IFileSystem* pFileSystem, ICollisionManager& oCollisionManager, IGeometryManager& oGeometryManager );	

	void					SetAnimationSpeed( TAnimation eAnimationType, float fSpeed );
	TAnimation				GetCurrentAnimationType() const;
	void					RunAction( string sAction, bool bLoop );
	void					SetCurrentPerso( bool bPerso );
	void					Die();
};

#endif // MOBILEENTITY_H