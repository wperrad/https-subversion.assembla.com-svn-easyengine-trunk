#include "Entity.h"
#include "FightSystem.h"


class CHuman : public CEntity, public IFighter
{

	typedef void (*TAction)( CHuman*, bool );
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
	float						m_fAngleRemaining;
	CVector						m_oDestination;
	bool						m_bArriveAtDestination;
	bool						m_bPerso;
	int							m_nLife;
	bool						m_bHitEnemy;
	map< TAnimation, float >	m_mAnimationSpeedByType;
	

	static map< string, TAction >	s_mActions;
	

	void		SetPredefinedAnimation( string s, bool bLoop );
	void 		Walk( bool bLoop );
	void 		Stand( bool bLoop );
	void 		Run( bool bLoop );
	void		HitLeftFoot( bool bLoop );
	void		HitReceived( bool bLoop );
	
	void		ReceiveHit( IFighter* pEnemy );
	void		TurnEyesH( float fValue );
	void		TurnNeckH( float f );
	void		LookAt( float alpha );
	void		UpdateGoto();
	float		GetDestinationAngleRemaining();
	IBone*		GetPreloadedBone( string sName );
	
	// IFighter implementation
	float				GetDistanceTo2dPoint( const CVector& oPosition );
	void				UpdateFight();
	int					GetLife();
	void				SetLife( int nLife );
	void				IncreaseLife( int nLife );
	void				TurnH( float fAngle ){Roll( fAngle );}
	void				Goto( IFighter* pFighter, float fSpeed );
	void				Run(){ Run( true ); }
	void				Stand(){ Stand( true ); }
	void				GetPosition( CVector& oPosition ){ GetWorldPosition( oPosition ); }
	void				TurnFaceToDestination(){ Roll( GetDestinationAngleRemaining() ); }
	void				Attack( IFighter* pEnemy );
	CMatrix&			GetWorldTM(){ return m_oWorldMatrix; }
	float				GetBoundingSphereRadius(){ return m_fBoundingSphereRadius; }
	IMesh*				GetMesh(){ return dynamic_cast< IMesh* >( m_pRessource ); }
	ICollisionManager&	GetCollisionManager(){ return m_oCollisionManager; }
	IAnimation*			GetCurrentAnimation(){ return m_pCurrentAnimation; }
	bool				IsPerso(){ return m_bPerso; }
	ISphere*			GetBoneSphere( string sBoneName );


	static map< string, TAnimation >		s_mAnimationStringToType;
	static map< TAnimation, IAnimation* >	s_mAnimationTypeToAnimation;	
	static map< TAnimation, float >			s_mOrgAnimationSpeedByType;

	static void		OnWalkAnimationCallback( IAnimation::TEvent e, void* pEntity );
	static void 	OnCollision( IEntity* pEntity );
	static void 	Walk( CHuman*, bool bLoop );
	static void 	Stand( CHuman*, bool bLoop );
	static void 	Run( CHuman*, bool bLoop );
	static void		HitLeftFoot( CHuman* pHuman, bool bLoop );
	static void 	HitReceived( CHuman* pHuman, bool bLoop );
	static vector< CHuman* >	s_vHumans;


public:
	CHuman( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IFileSystem* pFileSystem, ICollisionManager& oCollisionManager, IGeometryManager& oGeometryManager );
	

	void SetAnimationSpeed( TAnimation eAnimationType, float fSpeed );
	TAnimation	GetCurrentAnimationType() const;
	void		RunAction( string sAction, bool bLoop );
	void		Goto( const CVector& oPosition, float fSpeed );
	void		Goto( IEntity* pEntity, float fSpeed );
	
	void		Update();
	void		SetCurrentPerso( bool bPerso );
	void		SetDestination( const CVector& oDestination );
};
