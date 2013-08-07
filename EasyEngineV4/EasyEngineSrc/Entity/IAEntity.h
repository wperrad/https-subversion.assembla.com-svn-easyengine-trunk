#include "Math/vector.h"

class ICollisionManager;
class ISphere;
class IBone;
class IMesh;
class IAnimation;

class IAEntity
{
friend class CFightSystem;

private:
	enum TFightState
	{
		eNoFight = 0,
		eBeginHitReceived,
		eReceivingHit,
		eEndReceivingHit,
		eBeginGoToEnemy,
		eGoingToEnemy,
		eArrivedToEnemy,
		eBeginLaunchAttack,
		eLaunchingAttack,
		eEndLaunchAttack,
		eBeginWaitForNextAttack,
		eWaitingForNextAttack,
		eEndFight
	};	

	int							m_nRecuperationTime;
	IAEntity*					m_pCurrentEnemy;
	int							m_nBeginWaitTimeBeforeNextAttack;
	int							m_nCurrentWaitTimeBeforeNextAttack;
	TFightState					m_eFightState;
	bool						m_bHitEnemy;
	map< string, IBone* >		m_mPreloadedBones;
	string						m_sCurrentHitBoneName;

	void						UpdateFightState();

public:
	IAEntity();
	virtual int					GetLife() = 0;
	virtual void				SetLife( int nLife ) = 0;
	virtual void				IncreaseLife( int nLife ) = 0;
	virtual void				Goto( IAEntity* pEntityDestination, float fSpeed ) = 0;
	virtual float				GetDistanceTo2dPoint( const CVector& oPosition ) = 0;
	virtual void				SetDestination( const CVector& oDestination ) = 0;
	virtual void				TurnFaceToDestination() = 0;
	virtual void				Run() = 0;
	virtual void				Stand() = 0;
	virtual void				Attack( IAEntity* pFighter ) = 0;
	virtual void				ReceiveHit( IAEntity* pEnemy ) = 0;
	virtual void				GetPosition( CVector& oPosition ) = 0;
	virtual CMatrix&			GetWorldTM() = 0;
	virtual float				GetBoundingSphereRadius() = 0;
	virtual IMesh*				GetMesh() = 0;
	virtual ICollisionManager&	GetCollisionManager() = 0;
	virtual IAnimation*			GetCurrentAnimation() = 0;
	virtual bool				IsPerso() = 0;
	virtual ISphere*			GetBoneSphere( string sBoneName ) = 0;
};