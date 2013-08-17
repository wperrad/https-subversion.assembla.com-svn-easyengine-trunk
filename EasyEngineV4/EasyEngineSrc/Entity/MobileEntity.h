#ifndef MOBILEENTITY_H
#define MOBILEENTITY_H

#include "Entity.h"
#include "FighterEntity.h"


class CMobileEntity : public CEntity, public virtual IFighterEntity
{

protected:
	typedef void (*TAction)( CMobileEntity*, bool );
	string						m_sFileNameWithoutExt;
	bool						m_bInitSkeletonOffset;
	CMatrix						m_oSkeletonOffset;
	TAnimation					m_eCurrentAnimationType;
	map< string, IBone* >		m_mPreloadedBones;
	IEntity*					m_pLeftEye;
	IEntity*					m_pRightEye;
	CNode*						m_pNeck;
	float						m_fEyesRotH;
	float						m_fEyesRotV;
	float						m_fNeckRotH;
	float						m_fNeckRotV;
	const float					m_fMaxEyeRotationH;
	const float					m_fMaxEyeRotationV;
	const float					m_fMaxNeckRotationH;
	const float					m_fMaxNeckRotationV;
	bool						m_bPerso;
	int							m_nLife;
	map< TAnimation, float >	m_mAnimationSpeedByType;
	

	static map< string, TAction >			s_mActions;
	static map< string, TAnimation >		s_mAnimationStringToType;
	static map< TAnimation, IAnimation* >	s_mAnimationTypeToAnimation;	
	static map< TAnimation, float >			s_mOrgAnimationSpeedByType;
	static vector< CMobileEntity* >			s_vHumans;

	void				SetPredefinedAnimation( string s, bool bLoop );
	void 				Walk( bool bLoop );
	void 				Stand( bool bLoop );
	void 				Run( bool bLoop );
	void				HitLeftFoot( bool bLoop );
	void				PlayReceiveHit( bool bLoop );
	void				ReceiveHit( IFighterEntity* pEnemy );
	void				TurnEyesH( float fValue );
	void				TurnNeckH( float f );
	IBone*				GetPreloadedBone( string sName );
	int					GetLife();
	void				SetLife( int nLife );
	void				IncreaseLife( int nLife );
	ISphere*			GetBoneSphere( string sBoneName );
	bool				HasAlreadyHitEnemy();
	void				GetPosition( CVector& oPosition ) const;
	IMesh*				GetMesh();
	IAnimation*			GetCurrentAnimation();
	IFighterEntity*		GetFirstEnemy();
	IFighterEntity*		GetNextEnemy();
	void				Stand();
	CMatrix&			GetWorldTM();
	float				GetBoundingSphereRadius(){ return m_fBoundingSphereRadius; }
	IBox*				GetBoundingBox();
	ICollisionManager&	GetCollisionManager(){ return m_oCollisionManager; }

	static void			OnWalkAnimationCallback( IAnimation::TEvent e, void* pEntity );
	static void 		Walk( CMobileEntity*, bool bLoop );
	static void 		Stand( CMobileEntity*, bool bLoop );
	static void 		Run( CMobileEntity*, bool bLoop );
	static void			HitLeftFoot( CMobileEntity* pHuman, bool bLoop );
	static void 		PlayReceiveHit( CMobileEntity* pHuman, bool bLoop );


public:
	CMobileEntity( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IFileSystem* pFileSystem, ICollisionManager& oCollisionManager, IGeometryManager& oGeometryManager );	

	void			SetAnimationSpeed( TAnimation eAnimationType, float fSpeed );
	TAnimation		GetCurrentAnimationType() const;
	void			RunAction( string sAction, bool bLoop );
	void			SetCurrentPerso( bool bPerso );
	
};

#endif // MOBILEENTITY_H