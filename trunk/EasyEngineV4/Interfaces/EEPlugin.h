#ifndef PLUGIN_H
#define PLUGIN_H

// Systeme
#include <windows.h>

// stl
#include <vector>
#include <map>
#include <string>

class CPlugin
{
	CPlugin*									m_pParent;
	std::map< std::string, CPlugin* >			m_mChild;

	static std::map<std::string, CPlugin*>		s_mPlugins;

public:
	void AddChildPlugin( const std::string& sName, CPlugin* pChild )
	{
		m_mChild[ sName ] = pChild;
	}



	struct Desc
	{
		CPlugin*		m_pParent;
		std::string		m_sName;
		Desc( CPlugin* pParent, std::string sName ) : m_pParent( pParent ), m_sName( sName ){}
	};

	CPlugin( CPlugin* pParent, const std::string& sName ):
	m_pParent( pParent )
	{
		if ( pParent )
		{
			m_pParent = pParent;
			m_pParent->AddChildPlugin( sName, this );
		}
	}

	virtual	~CPlugin(){}

	static CPlugin* GetPlugin(std::string name)
	{
		std::map<std::string, CPlugin*>::iterator itPlugin = s_mPlugins.find(name);
		if (itPlugin != s_mPlugins.end())
			return itPlugin->second;
		return NULL;
	}

	virtual void UpdateChildPlugins()
	{
		for ( std::map< std::string, CPlugin* >::iterator itPlugin = m_mChild.begin(); itPlugin != m_mChild.end(); itPlugin++ )
		{
			CPlugin* pPlugin = itPlugin->second;
			pPlugin->UpdateChildPlugins ();
		}
	}

	CPlugin* GetChildPlugin( const std::string& sPluginName )
	{
		CPlugin* pPlugin = NULL;
		std::map< std::string, CPlugin* >::iterator itPlugin = m_mChild.find( sPluginName );
		if ( itPlugin != m_mChild.end() )
			pPlugin = itPlugin->second;
		return pPlugin;
	}

	static CPlugin* Create( const CPlugin::Desc& oDesc, std::string sDllPath, const std::string& sFuncName )
	{
		HMODULE hDll = LoadLibraryA(sDllPath.c_str());
		if ( !hDll )
		{
			std::string sMessage = sDllPath + " introuvable";
			std::exception e( sMessage.c_str() );
			throw e;
		}
		CPlugin* ( *pCreate )( const CPlugin::Desc& ) = reinterpret_cast< CPlugin* ( * )( const CPlugin::Desc& ) > ( GetProcAddress( hDll, sFuncName.c_str() ) );
		if ( !pCreate )
		{
			std::string sMessage = std::string( "Impossible de charger la fonction \"" ) + sFuncName + "\"  dans " + sDllPath;
			std::exception e( sMessage.c_str() );
			throw e;
		}
		CPlugin* plugin = pCreate(oDesc);
		s_mPlugins[oDesc.m_sName] = plugin;
		return plugin;
	}

	virtual void EnableRenderEvent(bool enable) {}
	
};

#endif // PLUGIN_H