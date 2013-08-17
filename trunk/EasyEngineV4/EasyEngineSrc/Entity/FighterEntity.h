#ifndef IFIGHTERENTITY_H
#define IFIGHTERENTITY_H

#include <vector>

#include "IRessource.h"

class ISphere;
class CVector;
class IMesh;
class IAnimation;
class IBbox;
class ICollisionManager;


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
	virtual bool				IsHitIntersectEnemySphere( IFighterEntity* pEnemy );
	virtual bool				IsHitIntersectEnemyBox( IFighterEntity* pEnemy );
	virtual void				SetLife( int nLife ) = 0;
	virtual ISphere*			GetBoneSphere( string sBoneName ) = 0;
	virtual void				OnEndHitAnimation();
	virtual float				GetBoundingSphereRadius() = 0;
	virtual IBox*				GetBoundingBox() = 0;

	static void			OnHitAnimationCallback( IAnimation::TEvent e, void* pData );
	static void			OnHitReceivedAnimationCallback( IAnimation::TEvent e, void* pData );

protected:
	IFighterEntity();
	virtual void				OnHit( IFighterEntity* pAgressor, string sHitBoneName );
	virtual void				Stand() = 0;
	virtual void				IncreaseLife( int nLife ) = 0;
	virtual ICollisionManager&	GetCollisionManager() = 0;

public:
	virtual int					GetLife() = 0;
	virtual void				GetPosition( CVector& v ) const = 0;

public: // temporaire, à mettre en privé ou protected par la suite
	
	virtual CMatrix&			GetWorldTM() = 0;
};

#endif // IFIGHTERENTITY_H