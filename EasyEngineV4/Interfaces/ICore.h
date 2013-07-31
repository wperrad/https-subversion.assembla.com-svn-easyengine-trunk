#ifndef ICORE_H
#define ICORE_H

#include "EEPlugin.h"
#include "IInputManager.h"


class CNode;
class IGUIManager;
class IRenderer;
class IShaderManager;
class IFileSystem;
class IWindow;
class IRessourceManager;
class ICameraManager;
class ISceneManager;
class IActionManager;

class ICore : public CPlugin
{
	
protected:
	ICore( const Desc& oDesc ) : CPlugin( oDesc.m_pParent, oDesc.m_sName ){}

public:
	struct Desc : public CPlugin::Desc
	{
		bool							m_bHUDEnabled;
		bool							m_bGUIEnabled;
		std::string 					m_sDefaultShader;
		IRenderer&						m_oRenderer;
		IFileSystem&					m_oFileSystem;
		IInputManager&					m_oInputManager;
		IWindow&						m_oWindow;
		IEventDispatcher&				m_oEventDispatcher;
		IRessourceManager&				m_oRessourceManager;
		IGUIManager*					m_pGUIManager;
		ICameraManager&					m_oCameraManager;
		ISceneManager&					m_oSceneManager;
		IActionManager&					m_oActionManager;

		Desc( IRenderer& oRenderer, IFileSystem& oFileSystem, IInputManager& oInputManager, 
			IWindow& oWindow, IEventDispatcher& oEventDispatcher, IRessourceManager& oRessourceManager, ICameraManager& oCameraManager,
			ISceneManager& oSceneManager, IActionManager& oActionManager ):
				CPlugin::Desc( NULL, "" ),
				m_bHUDEnabled( true ),
				m_bGUIEnabled( true ),
				m_oRenderer( oRenderer ),
				m_oFileSystem( oFileSystem ),
				m_oInputManager( oInputManager ),
				m_oWindow( oWindow ),
				m_oEventDispatcher( oEventDispatcher ),
				m_oRessourceManager( oRessourceManager ),
				m_pGUIManager( NULL ),
				m_oCameraManager( oCameraManager ),
				m_oSceneManager( oSceneManager ),
				m_oActionManager( oActionManager )
			{}
	};

	//virtual bool			GetCurrentMode() = 0;
	//virtual void			SetGUIMode( bool bGUI ) = 0;
	//virtual int				Run() = 0;
};




#endif // ICORE_H