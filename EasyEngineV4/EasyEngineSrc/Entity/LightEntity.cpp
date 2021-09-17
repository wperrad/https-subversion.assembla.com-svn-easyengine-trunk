#include "LightEntity.h"


CLightEntity::CLightEntity(EEInterface& oInterface, IRessource* pLight):
CEntity(oInterface)
{	
	m_pRessource = pLight;
	SetEntityName("Light");
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

void CLightEntity::GetEntityInfos(ILoader::CObjectInfos*& pInfos)
{
	if(!pInfos)
		pInfos = new ILoader::CLightEntityInfos;
	CEntity::GetEntityInfos(pInfos);
	ILoader::CLightEntityInfos& lightInfos = static_cast< ILoader::CLightEntityInfos& >(*pInfos);
	lightInfos.m_fIntensity = GetIntensity();
	lightInfos.m_oColor = GetColor();
	ILoader::CLightInfos::TLight type;
	switch (GetType())
	{
	case IRessource::DIRECTIONAL:
		type = ILoader::CLightInfos::eDirectionnelle;
		break;
	case IRessource::OMNI:
		type = ILoader::CLightInfos::eOmni;
		break;
	case IRessource::SPOT:
		type = ILoader::CLightInfos::eTarget;
		break;
	}
	lightInfos.m_eType = type;
}

void CLightEntity::BuildFromInfos(const ILoader::CObjectInfos& infos, CEntity* pParent)
{
	CEntity::BuildFromInfos(infos, pParent);
	const ILoader::CLightEntityInfos* pLightEntityInfos = static_cast< const ILoader::CLightEntityInfos* >(&infos);
	IRessource::TLight type;
	switch (pLightEntityInfos->m_eType)
	{
	case ILoader::CLightInfos::eDirectionnelle:
		type = IRessource::DIRECTIONAL;
		break;
	case ILoader::CLightInfos::eOmni:
		type = IRessource::OMNI;
		break;
	case ILoader::CLightInfos::eTarget:
		type = IRessource::SPOT;
		break;
	default:
		throw 1;
	}
	IRessource* pLight = m_oRessourceManager.CreateLight(pLightEntityInfos->m_oColor, type, pLightEntityInfos->m_fIntensity);
	m_pRessource = pLight;
	SetLocalMatrix(infos.m_oXForm);
}