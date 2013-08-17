#include "Entity.h"

class ICylinder;


class CCylinderEntity : public CEntity
{
	ICylinder*	m_pCylinder;
public:

	CCylinderEntity( IGeometryManager& gm, IRenderer& r, IRessourceManager& rm, ICollisionManager& cm, float fRadius, float fHeight );
	void	Update();
};