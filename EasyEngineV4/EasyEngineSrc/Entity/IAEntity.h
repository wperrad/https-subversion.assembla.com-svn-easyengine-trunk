#include "Math/vector.h"
#include "FighterEntity.h"

class ICollisionManager;
class ISphere;
class IBone;
class IMesh;
class IAnimation;
class IGeometryManager;

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
	CVector						m_oDestination;
	float						m_fAngleRemaining;
	bool						m_bArriveAtDestination;
	IFighterEntity*				m_pCurrentEnemy;
	vector< CVector2D >			m_vCurrentPath;
	int							m_nCurrentPathPointNumber;
	float						m_fDestinationDeltaRadius;

	void						UpdateGoto();
	void						UpdateFightState();
	float						GetDestinationAngleRemaining();
	void						TurnFaceToDestination();
	void						OnReceiveHit( IFighterEntity* pAgressor );
	void						OnEndHitAnimation();
	virtual void				ComputePathFind2D( const CVector2D& oOrigin, const CVector2D& oDestination, vector< CVector2D >& vPoints ) = 0;
	virtual void				SetDestination( const CVector& oDestination );
	virtual void				Turn( float fAngle ) = 0;
	virtual void				Attack( IFighterEntity* pFighter ) = 0;
	virtual IAnimation*			GetCurrentAnimation() = 0;
	virtual float				GetDistanceTo2dPoint( const CVector& oPosition ) = 0;
	virtual void				Run() = 0;
	virtual IBox*				GetFirstCollideBox() = 0;
	virtual IBox*				GetNextCollideBox() = 0;

	static void					OnHitReceivedCallback( IAnimation::TEvent e, void* pData );

protected:

	static void 				OnCollision( IAEntity* pEntity );

public:
	IAEntity();
	virtual void				Goto( const CVector& oPosition, float fSpeed );

	void						Update();
};