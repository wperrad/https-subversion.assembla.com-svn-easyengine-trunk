#include "TestEntity.h"

CTestEntity::CTestEntity(
	string sFileName,
	IRessourceManager& oRessourceManager,
	IRenderer& oRenderer,
	IEntityManager* pEntityManager,
	IGeometryManager& oGeometryManager,
	ICollisionManager& oCollisionManager) :
	CEntity(sFileName, oRessourceManager, oRenderer, pEntityManager, oGeometryManager, oCollisionManager)
{

}