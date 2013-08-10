#include "IAEntity.h"
#include "MobileEntity.h"

class CNPCEntity : public CMobileEntity, public IAEntity
{

public:

	CNPCEntity( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IFileSystem* pFileSystem, ICollisionManager& oCollisionManager, IGeometryManager& oGeometryManager );
	int						GetLife();
	void					SetLife( int nLife );
	void					IncreaseLife( int nLife );
	float					GetDistanceTo2dPoint( const CVector& oPosition );
	void					SetDestination( const CVector& oDestination );
	void					Run();
	void					TurnFaceToDestination();
	void					Attack( IFighterEntity* pEnemy );
	void					ReceiveHit( IAEntity* pEnemy );
	float					GetBoundingSphereRadius(){ return m_fBoundingSphereRadius; }
	ICollisionManager&		GetCollisionManager(){ return m_oCollisionManager; }
	ISphere*				GetBoneSphere( string sBoneName );
	void					UpdateGoto();
	void					Goto( IAEntity* pEntity, float fSpeed );
	void					Goto( const CVector& oPosition, float fSpeed );
	void					Goto( IEntity* pEntity, float fSpeed );
	void					LookAt( float alpha );
	float					GetDestinationAngleRemaining();
	void					Update();
	IAnimation*				GetCurrentAnimation();
	CMatrix&				GetWorldTM();
	bool					IsHitIntersectEnemySphere( IFighterEntity* pEnemy );
	IFighterEntity*			GetFirstEnemy();
	IFighterEntity*			GetNextEnemy();
	void					GetPosition( CVector& v );
	void					ReceiveHit( IFighterEntity* pEnemy );
	bool					IsHitIntersectEnemyBox( IFighterEntity* pEnemy );
	void					Stand();
};