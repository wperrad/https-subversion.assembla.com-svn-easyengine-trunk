#include "EEPlugin.h"

class IGUIManager;

class IHud : public CPlugin
{
public:

	struct Desc : public CPlugin::Desc
	{
		IGUIManager&	m_oGUIManager;
		Desc(IGUIManager& oGUIManager):
			CPlugin::Desc(NULL, "Hud"),
			m_oGUIManager(oGUIManager)
		{
		}
	};

	IHud(const Desc& oDesc) : CPlugin(oDesc.m_pParent, oDesc.m_sName) {}

};