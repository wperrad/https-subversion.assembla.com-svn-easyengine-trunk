#ifndef EASYENGINE_H
#define EASYENGINE_H

// stl
#include <vector>
#include <map>

// Engine
#include "EEPlugin.h"
#include "IInputManager.h"
#include "ICore.h"
#include "IEventDispatcher.h"

#include "../Utils2/Position.h"

class CNode;
class IActionManager;
class CScene;
class CEntity;
class ICameraManager;
class IFileSystem;
class ICamera;
class IScene;

struct PositionnedMessage
{
	int x;
	int y;
	std::string m_sMsg;
};


class CCore : public ICore 
{
	IInputManager&					m_oInputManager;
	IActionManager&					m_oActionManager;
	IRessourceManager&				m_oRessourceManager;
	IGUIManager*					m_pGUIManager;
	IRenderer&						m_oRenderer;
	IFileSystem&					m_oFileSystem;
	IWindow&						m_oWindow;
	IEventDispatcher&				m_oEventDispatcher;
	ICameraManager&					m_oCameraManager;
  	ISceneManager&        			m_oSceneManager;

  	unsigned int  					m_nFrameCount;
  	time_t          				m_nLastFrameTime;
  	unsigned int  					m_nLastFps;

	std::vector< std::string >		m_vUnPositionnedMessage;
	PositionnedMessage*				m_vPositionnedMessage[10];
	unsigned int					m_nMsgCount;
	bool							m_bGUIMode;
	int								m_nLastGameMousePosx;
	int								m_nLastGameMousePosy;
	std::string						m_strText;
	CPosition						m_TextPosition;
	bool							m_bHUDEnabled;

	std::map< std::string, CPlugin* >	m_mPlugin;
	static void						OnUpdateWindow( CPlugin* pCore, IEventDispatcher::TWindowEvent e, int nWindowWidth, int nWindowHeight );
	int								CalculFramerate();
	void            				Close();

	
	static LRESULT CALLBACK			WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		
public:

							CCore( const Desc& desc );
							~CCore();
	//void					Print( std::string szText, int nWidth,int nHeight);
	//void					Print( std::string szText );
	//void					DisplayNodePosition( CNode* pNode );
	//void					DisplayNodeInfos( CNode* pNode );
	//void					DisableLight(int nLight);
	void					End();
};

extern "C" _declspec(dllexport) ICore* CreateCore( const ICore::Desc& oDesc );



#endif