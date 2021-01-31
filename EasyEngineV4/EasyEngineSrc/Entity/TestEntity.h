#include "Entity.h"

class CTestEntity : public CEntity
{
public:
	CTestEntity(
		string sFileName, 
		IRessourceManager& oRessourceManager, 
		IRenderer& oRenderer, 
		IEntityManager* pEntityManager,
		IGeometryManager& oGeometryManager, 
		ICollisionManager& oCollisionManager);
};