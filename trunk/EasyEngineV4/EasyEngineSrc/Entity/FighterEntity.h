#ifndef IFIGHTERENTITY_H
#define IFIGHTERENTITY_H

#include <vector>

#include "IRessource.h"

class ISphere;
class CVector;
class IMesh;
class IAnimation;


using namespace std;

class IFighterEntity
{
	string						m_sCurrentHitBoneName;
	bool						m_bHitEnemy;

	virtual IAnimation*			GetCurrentAnimation() = 0;
	virtual void				OnReceiveHit( IFighterEntity* pEnemy );
	virtual void				ReceiveHit( IFighterEntity* pEnemy ) = 0;
	virtual IFighterEntity*		GetFirstEnemy() = 0;
	virtual IFighterEntity*		GetNextEnemy() = 0;
	virtual bool				IsHitIntersectEnemySphere( IFighterEntity* pEnemy ) = 0;
	virtual bool				IsHitIntersectEnemyBox( IFighterEntity* pEnemy ) = 0;
	virtual void				SetLife( int nLife ) = 0;
	virtual void				OnEndHitAnimation();

	static void			OnHitAnimationCallback( IAnimation::TEvent e, void* pData );
	static void			OnHitReceivedAnimationCallback( IAnimation::TEvent e, void* pData );

protected:
	IFighterEntity();
	virtual void				IncreaseLife( int nLife ) = 0;
	virtual void				OnHit( IFighterEntity* pAgressor );
	virtual void				Stand() = 0;

public:
	virtual int					GetLife() = 0;

public: // temporaire, à mettre en privé ou protected par la suite
	virtual void				GetPosition( CVector& v ) = 0;
	virtual CMatrix&			GetWorldTM() = 0;
};

#endif // IFIGHTERENTITY_H