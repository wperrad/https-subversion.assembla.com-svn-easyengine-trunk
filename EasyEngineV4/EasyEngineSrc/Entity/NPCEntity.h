#include "IAEntity.h"
#include "MobileEntity.h"

#pragma warning( disable:4250 )

class CNPCEntity : public CMobileEntity, public IAEntity
{

	void				Turn( float fAngle );

protected:
	static void 		OnCollision( IEntity* pEntity );

public:

	CNPCEntity( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IFileSystem* pFileSystem, ICollisionManager& oCollisionManager, IGeometryManager& oGeometryManager );
	int						GetLife();
	void					SetLife( int nLife );
	void					IncreaseLife( int nLife );
	float					GetDistanceTo2dPoint( const CVector& oPosition );
	void					Run();
	void					Attack( IFighterEntity* pEnemy );
	void					ReceiveHit( IAEntity* pEnemy );
	ICollisionManager&		GetCollisionManager(){ return m_oCollisionManager; }
	void					LookAt( float alpha );
	void					Update();
	IAnimation*				GetCurrentAnimation();
	CMatrix&				GetWorldTM();
	IFighterEntity*			GetFirstEnemy();
	IFighterEntity*			GetNextEnemy();
	void					GetPosition( CVector& v );
	void					ReceiveHit( IFighterEntity* pEnemy );
	void					Stand();
	void					Goto( const CVector& oPosition, float fSpeed );
	IBox*					GetFirstCollideBox();
	IBox*					GetNextCollideBox();

	// temporaire
	void	ComputePathFind2D( const CVector2D& oOrigin, const CVector2D& oDestination, vector< CVector2D >& vPoints );
};