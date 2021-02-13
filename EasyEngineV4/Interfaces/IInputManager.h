#ifndef IINPUTMANAGER_H
#define IINPUTMANAGER_H

#include "EEPlugin.h"
#include "IEventDispatcher.h"

class IWidget;
class IEventDispatcher;

class IInputManager : public CPlugin
{
protected:
	IInputManager( const Desc& oDesc ) : CPlugin( oDesc.m_pParent, oDesc.m_sName ){}

public:
	struct Desc : public CPlugin::Desc
	{
		IEventDispatcher&	m_oEventDispatcher;
		int					m_nResX;
		int					m_nResY;
		Desc( IEventDispatcher& oEventDispatcher ) : 
			CPlugin::Desc( NULL, "InputManager" ),
			m_oEventDispatcher( oEventDispatcher ), 
			m_nResX(0), 
			m_nResY(0){}
	};

	enum KEY_STATE
	{
		NONE = -1,
		RELEASED = 0,
		PRESSED,
		JUST_PRESSED,
		JUST_RELEASED
	};

	enum TMouseButton
	{
		eMouseButtonNone = 0,
		eMouseButtonLeft,
		eMouseButtonRight,
		eMouseWheel
	};

	enum TMouseButtonState
	{
		eMouseButtonStateNone = 0,
		eMouseButtonStateJustUp,
		eMouseButtonStateDown,
		eMouseButtonStateJustDown,
		eMouseWheelUp,
		eMouseWheelDown,
		eMouseWheelNone
	};

	typedef void( *TKeyCallback )( CPlugin*, unsigned int, KEY_STATE );

	virtual void				GetPhysicalCursorPos(int& x, int& y) = 0;
	virtual void				GetVirtualCursorPos( int& x, int& y ) = 0;
	virtual void				SetMouseCursorXPos( int x ) = 0;
	virtual void				SetMouseCursorYPos( int y ) = 0;
	virtual TMouseButtonState	GetMouseButtonState( TMouseButton b ) = 0;
	virtual void				AbonneToKeyEvent( CPlugin*, TKeyCallback ) = 0;
	virtual void				GetOffsetMouse( int& x, int& y ) = 0;
	virtual void				SetMouseCursorPos( int nx, int ny ) = 0;
	virtual KEY_STATE			GetKeyState( unsigned char key ) = 0;
	virtual void				OnKeyPress( unsigned int key ) = 0;
	virtual void				OnKeyRelease( unsigned int key ) = 0;
	virtual void				OnMouseMove( int x, int y ) = 0;
	virtual void				OnMouseClick( TMouseButton b, TMouseButtonState s ) = 0;
	virtual void				SetMouseButtonState( unsigned int ButtonState ) = 0;
	virtual void				SetMouseCorner( int left, int right, int top, int bottom ) = 0;
	virtual float				GetMouseSensitivity() = 0;
	virtual void				SetMouseSensitivity( float s ) = 0;
	virtual void				SetEditionMode( bool bEdition ) = 0;
	virtual void				OnUpdate() = 0;
};

#endif // IINPUTMANAGER_H