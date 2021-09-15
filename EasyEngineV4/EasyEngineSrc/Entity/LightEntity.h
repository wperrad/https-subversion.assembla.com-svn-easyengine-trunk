#ifndef LIGHTENTITY_H
#define LIGHTENTITY_H

#include "Entity.h"

class IRessource;
class IRessourceManager;
class IRenderer;

class CLightEntity : public CEntity
{
public:
	CLightEntity(EEInterface& oInterface, IRessource* pLight);
	~CLightEntity();
	void				SetIntensity( float fInstensity );
	float				GetIntensity();
	CVector				GetColor();
	IRessource::TLight	GetType();
	void				Unlink();
	void				GetEntityInfos(ILoader::CObjectInfos*& pInfos) override;

};

#endif // LIGHTENTITY_H