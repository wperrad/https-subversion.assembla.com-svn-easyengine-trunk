#include "Entity.h"

class ICylinder;


class CCylinderEntity : public CEntity
{
	ICylinder*	m_pCylinder;
public:

	CCylinderEntity(EEInterface& oInterface, float fRadius, float fHeight );
	void	Update();
};