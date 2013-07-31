#ifndef LIGHTENTITY_H
#define LIGHTENTITY_H

#include "Entity.h"

class IRessource;
class IRessourceManager;
class IRenderer;

class CLightEntity : public CEntity
{
public:
	CLightEntity( IRessource* pLight, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IGeometryManager& oGeometryManager, ICollisionManager& oCollisionManager );
	~CLightEntity();
	void				SetIntensity( float fInstensity );
	float				GetIntensity();
	CVector				GetColor();
	IRessource::TLight	GetType();
	void				Unlink();

};

#endif // LIGHTENTITY_H