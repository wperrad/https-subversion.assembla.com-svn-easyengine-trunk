#ifndef EE_INTERFACE_H
#define EE_INTERFACE_H

#include "EEPlugin.h"

#include <map>

using namespace std;


class EEInterface
{
public:
	void RegisterPlugin(CPlugin* plugin)
	{
		m_mPlugins.insert(map<string, CPlugin*>::value_type(plugin->GetName(), plugin));
	}

	CPlugin* GetPlugin(string sName)
	{
		return m_mPlugins[sName];
	}

private:
	map<string, CPlugin*>	m_mPlugins;
};

#endif // EE_INTERFACE_H