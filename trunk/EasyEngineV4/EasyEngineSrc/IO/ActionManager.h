#ifndef CONTROLMANAGER_H
#define CONTROLMANAGER_H

// System
#include <windows.h>
#include <string>
#include <map>

// Engine
#include "IInputManager.h"
#include "IActionManager.h"
#include "IEventDispatcher.h"


class IInputManager;

typedef map< string, WPARAM >	Map_string_WPARAM;
typedef map< string, TMouse >	Map_string_AXIS;
typedef map< string, pair< IInputManager::TMouseButton, IInputManager::TMouseButtonState > >	TStringMouseButton;

class CActionManager : public IActionManager
 {
	Map_string_WPARAM						m_MapKeyActionState;
	Map_string_WPARAM						m_MapGUIKeyActionState;	
	Map_string_AXIS							m_MapMouseActionState;
	TStringMouseButton						m_mMouseActionButtonState;
	Map_string_AXIS							m_MapGUIMouseActionState;
	float									m_fMouseSensibility;
	float									m_fGUIMouseSensibility;
	IInputManager&							m_oInputManager;
	static void								OnKeyAction( CPlugin*, unsigned int key, IInputManager::KEY_STATE );
	//static void							OnMouseAction( CPlugin*, IEventDispatcher::TMouseEvent, int, int );

	map< unsigned int, IInputManager::KEY_STATE >	m_mKeyboardState;

public:

										CActionManager( IActionManager::Desc& oDesc );
	virtual								~CActionManager();

	IInputManager::KEY_STATE			GetKeyActionState( string sActionName );
	IInputManager::TMouseButtonState	GetMouseActionState( string sActionName );

	IInputManager::KEY_STATE			GetGUIActionState( const char* szActionName);
	void								AddKeyAction( string sActionName, WPARAM key);
	void								AddMouseAction( string sActionName, IInputManager::TMouseButton b, IInputManager::TMouseButtonState s );
	void								AddGUIAction( const char* szActionName, WPARAM key );
	void								AddGameAction( const char* szActionName, TMouse axis );	
	void								AddGUIAction( const char* szActionName, TMouse axis );
	void								GetGameMousePos( int& x, int& y );
	void								SetGameMousePos( int x, int y );
	WPARAM								GetGameKeyPress();
	WPARAM								GetGUIKeyPress();	
	void								SetGameMouseSensibility(float fSensibility);
	void								SetGUIMouseSensibility(float fSensibility);	
	void								ForceActionState( string sActionName, IInputManager::KEY_STATE eState );
};

extern "C" _declspec(dllexport) IActionManager* CreateActionManager( IActionManager::Desc& oDesc );


#endif //CONTROLMANAGER_H