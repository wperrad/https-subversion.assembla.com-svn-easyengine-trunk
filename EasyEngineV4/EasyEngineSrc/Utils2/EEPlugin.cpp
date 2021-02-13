#include "EEPlugin.h"
#include "Interface.h"


EEInterface* CPlugin::s_pEngineInterface = NULL;
std::map<std::string, CPlugin*>	CPlugin::s_mPlugins;

CPlugin::CPlugin(CPlugin* pParent, const std::string& sName) :
	m_pParent(pParent)
{
	if (pParent)
	{
		m_pParent = pParent;
		m_pParent->AddChildPlugin(sName, this);
	}
}

CPlugin::~CPlugin() 
{
}

string CPlugin::GetName()
{
	return m_sName;
}

void CPlugin::AddChildPlugin(const std::string& sName, CPlugin* pChild)
{
	m_mChild[sName] = pChild;
}

CPlugin* CPlugin::GetPlugin(std::string name)
{
	std::map<std::string, CPlugin*>::iterator itPlugin = s_mPlugins.find(name);
	if (itPlugin != s_mPlugins.end())
		return itPlugin->second;
	return NULL;
}

void CPlugin::UpdateChildPlugins()
{
	for (std::map< std::string, CPlugin* >::iterator itPlugin = m_mChild.begin(); itPlugin != m_mChild.end(); itPlugin++)
	{
		CPlugin* pPlugin = itPlugin->second;
		pPlugin->UpdateChildPlugins();
	}
}

CPlugin* CPlugin::GetChildPlugin(const std::string& sPluginName)
{
	CPlugin* pPlugin = NULL;
	std::map< std::string, CPlugin* >::iterator itPlugin = m_mChild.find(sPluginName);
	if (itPlugin != m_mChild.end())
		pPlugin = itPlugin->second;
	return pPlugin;
}

CPlugin* CPlugin::Create(const CPlugin::Desc& oDesc, std::string sDllPath, const std::string& sFuncName)
{

	HMODULE hDll = LoadLibraryA(sDllPath.c_str());
	if (!hDll)
	{
		std::string sMessage = sDllPath + " introuvable";
		std::exception e(sMessage.c_str());
		throw e;
	}
	CPlugin* (*pCreate)(const CPlugin::Desc&) = reinterpret_cast< CPlugin* (*)(const CPlugin::Desc&) > (GetProcAddress(hDll, sFuncName.c_str()));
	if (!pCreate)
	{
		std::string sMessage = std::string("Impossible de charger la fonction \"") + sFuncName + "\"  dans " + sDllPath;
		std::exception e(sMessage.c_str());
		throw e;
	}
	CPlugin* plugin = pCreate(oDesc);
	s_mPlugins[oDesc.m_sName] = plugin;
	plugin->m_sName = oDesc.m_sName;
	s_pEngineInterface->RegisterPlugin(plugin);
	return plugin;
}

void CPlugin::SetEngineInterface(EEInterface* pInterface)
{
	s_pEngineInterface = pInterface;
}

void CPlugin::EnableRenderEvent(bool enable) {}
