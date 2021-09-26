#ifndef PLUGIN_H
#define PLUGIN_H

// Systeme
#include <windows.h>

// stl
#include <vector>
#include <map>
#include <string>

using namespace std;

class EEInterface;
class CPlugin;

typedef void(*ProcPluginCreation)(CPlugin* plugin, void* pData);



class CPlugin
{
public:

	struct Desc
	{
		CPlugin*		m_pParent;
		std::string		m_sName;
		Desc(CPlugin* pParent, std::string sName) : m_pParent(pParent), m_sName(sName) {}
	};

	CPlugin(CPlugin* pParent, const std::string& sName);
	virtual					~CPlugin();
	virtual string GetName() = 0;
	void					AddChildPlugin(const std::string& sName, CPlugin* pChild);
	virtual void			EnableRenderEvent(bool enable);
	virtual void			UpdateChildPlugins();
	CPlugin*				GetChildPlugin(const std::string& sPluginName);
	virtual void			OnPluginRegistered() {}
	static void				HandlePluginCreation(string pluginName, ProcPluginCreation callback, void* pData);

	static CPlugin*			Create(const CPlugin::Desc& oDesc, std::string sDllPath, const std::string& sFuncName);
	static CPlugin*			Create(EEInterface& oInterface, std::string sDllPath, const std::string& sFuncName);
	static void				SetEngineInterface(EEInterface* pInterface);
	static CPlugin*			GetPlugin(std::string name);
	

protected:
	static EEInterface*							s_pEngineInterface;

private:
	CPlugin*												m_pParent;
	std::map< std::string, CPlugin* >						m_mChild;
	static std::map<std::string, CPlugin*>					s_mPlugins;
	static map<string, pair<ProcPluginCreation, void*>>		s_vPluginCreationCallback;
	
};

#endif // PLUGIN_H