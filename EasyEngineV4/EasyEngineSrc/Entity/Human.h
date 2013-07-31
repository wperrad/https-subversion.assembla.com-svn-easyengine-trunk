#include "Entity.h"

//using namespace std;

class CHuman : public CEntity
{
	typedef void (*TAction)( CHuman*, bool );
	string						m_sFileNameWithoutExt;
	bool						m_bInitSkeletonOffset;
	CMatrix						m_oSkeletonOffset;
	TAnimation					m_eCurrentAnimationType;
	map< string, IBone* >		m_mPreloadedBones;

	static map< string, TAction >	s_mActions;
	

	void		SetPredefinedAnimation( string s, bool bLoop );
	void 		Walk( bool bLoop );
	void 		Stand( bool bLoop );
	void 		Run( bool bLoop );
	void		HitLeftFoot( bool bLoop );
	void		HitReceived( bool bLoop );
	IBone*		GetPreloadedBone( string sName );
	void		ReceiveHit( CHuman* pHuman );	

	static map< string, TAnimation >	s_mAnimationStringToType;
	static map< TAnimation, IAnimation* > s_mAnimationTypeToAnimation;
	static map< TAnimation, float > s_mAnimationSpeedByType;
	static map< TAnimation, float > s_mOrgAnimationSpeedByType;
	static void	OnWalkAnimationCallback( IAnimation::TEvent e, void* pEntity );
	static void	OnHitLeftFootAnimationCallback( IAnimation::TEvent e, void* pEntity );
	static void OnHitReceivedCallback( IAnimation::TEvent e, void* pEntity );

	static void Walk( CHuman*, bool bLoop );
	static void Stand( CHuman*, bool bLoop );
	static void Run( CHuman*, bool bLoop );
	static void	HitLeftFoot( CHuman* pHuman, bool bLoop );
	static void HitReceived( CHuman* pHuman, bool bLoop );
	static vector< CHuman* >	s_vHumans;


public:
	CHuman( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IFileSystem* pFileSystem, ICollisionManager& oCollisionManager, IGeometryManager& oGeometryManager );
	

	void SetAnimationSpeed( TAnimation eAnimationType, float fSpeed );
	TAnimation	GetCurrentAnimationType() const;
	void		RunAction( string sAction, bool bLoop );
};