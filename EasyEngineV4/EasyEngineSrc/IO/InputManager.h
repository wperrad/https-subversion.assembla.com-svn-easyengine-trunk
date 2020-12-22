#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#pragma warning(disable:4275)

// System
#include <windows.h>
#include <map>
#include <vector>

// Engine
#include "EEPlugin.h"
#include "IInputManager.h"
#include "IEventDispatcher.h"

#define CONTROL_COUNT 256
 
using namespace std;

class CInputManager : public IInputManager
{
private:	
	vector< pair< CPlugin*, TKeyCallback > >				m_vKeyCallback;
	//vector< TMouseCallback >								m_vMouseCallback;
	POINT													m_OldMousePos;
	POINT													m_OffsetMousePos;
	int														m_nCursorPosx;
	int														m_nCursorPosy;
	unsigned int											m_ButtonState;
	std::map< unsigned int, KEY_STATE > 					m_mKeyboardState;
	map< TMouseButton, TMouseButtonState >					m_mMouseButtonState;
	IEventDispatcher&										m_oEventDispatcher;

	int														m_xCorner;
	int														m_yCorner;

	int														m_Left;
	int														m_Right;
	int														m_Top;
	int														m_Bottom;
	float													m_fMouseSensitivity;
	bool													m_bEditionMode;
	vector< int >											m_vLastKeyEvent;
	map< int, bool >										m_mKeyPressed;
	bool													m_bWheelConsumed;




	void													OnKeyPress( unsigned int key );
	void													OnKeyRelease( unsigned int key );	
	void													OnMouseMove( int x, int y );
	void													OnMouseClick( TMouseButton b, TMouseButtonState s );
	void													OnMouseWheel(int value);

	static void					OnKeyEventCallback( CPlugin*, IEventDispatcher::TKeyEvent, int nKeyCode );
	static void					OnMouseEventCallback( CPlugin*, IEventDispatcher::TMouseEvent e, int x, int y );

public:

	CInputManager( const IInputManager::Desc& );
	virtual									~CInputManager(void);
	

	void									GetOffsetMouse( int& x, int& y );
	void									GetCursorPos_EE( int& x, int& y );
	void									ShowMouseCursor(bool bShow);
	void									SetMouseCursorPos(int nx, int ny);
	void									SetMouseCursorXPos(int nx);
	void									SetMouseCursorYPos(int ny);
	void									SetMouseButtonState(unsigned int state);
	TMouseButtonState						GetMouseButtonState( TMouseButton b );
	void									AbonneToKeyEvent( CPlugin*, TKeyCallback Callback );
	KEY_STATE								GetKeyState( unsigned char key );
	void									SetMouseCorner( int left, int right, int top, int bottom );	
	float									GetMouseSensitivity();
	void									SetMouseSensitivity( float s );
	void									SetEditionMode( bool bEdition );
	void									OnUpdate();
};

extern "C" _declspec(dllexport) IInputManager* CreateInputManager( const IInputManager::Desc& );

#endif // INPUTMANAGER_H