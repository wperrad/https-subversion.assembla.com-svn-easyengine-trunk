#include "Math/vector.h"
#include "FighterEntity.h"

class ICollisionManager;
class ISphere;
class IBone;
class IMesh;
class IAnimation;

class IAEntity : public virtual IFighterEntity
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

	TFightState					m_eFightState;

	int							m_nRecuperationTime;
	int							m_nBeginWaitTimeBeforeNextAttack;
	int							m_nCurrentWaitTimeBeforeNextAttack;
	bool						m_bHitEnemy;
	map< string, IBone* >		m_mPreloadedBones;
	string						m_sCurrentHitBoneName;

	
	static void					OnHitReceivedCallback( IAnimation::TEvent e, void* pData );

protected:
	IFighterEntity*				m_pCurrentEnemy;
	void						UpdateFightState();

public:
	IAEntity();
	virtual void				Goto( IAEntity* pEntityDestination, float fSpeed ) = 0;
	virtual void				Goto( const CVector& oPosition, float fSpeed ) = 0;
	virtual float				GetDistanceTo2dPoint( const CVector& oPosition ) = 0;
	virtual void				SetDestination( const CVector& oDestination ) = 0;
	virtual void				TurnFaceToDestination() = 0;
	virtual void				Run() = 0;
	virtual void				Attack( IFighterEntity* pFighter ) = 0;
	virtual IAnimation*			GetCurrentAnimation() = 0;
	void						OnReceiveHit( IFighterEntity* pAgressor );
	void						OnEndHitAnimation();
};