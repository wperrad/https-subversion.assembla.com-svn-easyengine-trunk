#include "LightEntity.h"


CLightEntity::CLightEntity( IRessource* pLight, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IGeometryManager& oGeometryManager, ICollisionManager& oCollisionManager ):
CEntity( oRessourceManager, oRenderer, NULL, oGeometryManager, oCollisionManager )
{	
	m_pRessource = pLight;
}

CLightEntity::~CLightEntity()
{
	m_oRessourceManager.DisableLight( m_pRessource );
}

void CLightEntity::SetIntensity( float fIntensity )
{
	m_oRessourceManager.SetLightIntensity( m_pRessource, fIntensity );
}

float CLightEntity::GetIntensity()
{
	return m_oRessourceManager.GetLightIntensity( m_pRessource );
}

CVector CLightEntity::GetColor()
{
	return m_oRessourceManager.GetLightColor( m_pRessource );
}

IRessource::TLight CLightEntity::GetType()
{
	return m_oRessourceManager.GetLightType( m_pRessource );
}

void CLightEntity::Unlink()
{
	m_oRessourceManager.DisableLight( m_pRessource );
	CEntity::Unlink();
}