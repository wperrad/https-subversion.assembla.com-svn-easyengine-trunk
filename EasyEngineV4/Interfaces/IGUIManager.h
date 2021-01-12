#ifndef IGUIManager_H
#define IGUIManager_H

#include <string>
#include "EEPlugin.h"

class IRenderer;
class IRessourceManager;
class IXMLParser;
class IInputManager;
class ILoaderManager;
class CGUIWidget;

using namespace std;

class IGUIWindow
{
public:
	virtual void	Display() = 0;
	virtual void	SetVisibility(bool bVisible) = 0;
};

class IGUIManager : public CPlugin
{
protected:
	IGUIManager( const Desc& oDesc ) : CPlugin( oDesc.m_pParent,  oDesc.m_sName ){}

public:
	enum ENUM_EVENT
	{
		EVENT_NONE = 0,
		EVENT_MOUSEMOVE,
		EVENT_LMOUSECLICK,
		EVENT_LMOUSERELEASED,
		EVENT_RMOUSECLICK,
		EVENT_MOUSEENTERED,
		EVENT_MOUSEEXITED,
		EVENT_OUTSIDE
	};

	enum TFontColor 
	{
		eWhite = 0,
		eBlue,
		eTurquoise
	};

	typedef void (*EVENT_CALLBACK)( ENUM_EVENT nEvent, CGUIWidget*, int, int) ;

	struct Desc : public CPlugin::Desc
	{
		IRenderer&				m_oRenderer;
		IRessourceManager&		m_oRessourceManager;
		IXMLParser&				m_oXMLParser;
		std::string				m_sShaderName;
		IInputManager&			m_oInputManager;
		Desc( IRenderer& oRenderer, IRessourceManager& oRessourceManager, IXMLParser& oXMLParser, IInputManager& oInputManager ):
			CPlugin::Desc( NULL, "" ),
			m_oRenderer( oRenderer ),
			m_oRessourceManager( oRessourceManager ),
			m_oXMLParser( oXMLParser ),
			m_oInputManager( oInputManager ){}
	};

	virtual int				CreateImage( const std::string& sFileName, unsigned int nWidth = 0, unsigned int nHeight = 0 ) = 0;
	virtual void			SetPosition( int hWidget, int x, int y ) = 0;
	virtual int				CreateGUIWindow( int nx, int ny , int nWidth, int nHeight ) = 0;
	virtual void			AddWidget( int hWindow, int hWidget ) = 0;
	virtual int				CreateListener( EVENT_CALLBACK pfnCallback ) = 0;
	virtual void			AddEventListener( int hWidget, int hListener ) = 0;
	virtual void			SetCurrentWindow( IGUIWindow* pWindow ) = 0;
	virtual void			SetVisibility( int hWindow, bool bVisible ) = 0;
	virtual void			OnRender() = 0;
	virtual void			Print( std::string sText, int x, int y, TFontColor color = eWhite) = 0;
	virtual void			Print( char c, int x, int y ) = 0;
	virtual void			SetActive( bool bActivate ) = 0;
	virtual bool			GetActive() = 0;
	virtual unsigned int	GetCurrentFontHeight() const = 0;
	virtual unsigned int	GetCurrentFontWidth( char c ) const = 0;
	virtual unsigned int	GetCharSpace() = 0;
	virtual int				CreateStaticText( vector< string >& vText, int nPosX = 0, int nPosY = 0 ) = 0;
	virtual void			DestroyStaticTest( int nID ) = 0;
	virtual void			PrintStaticText( int nTextID ) = 0;
	virtual void			EnableStaticText( int nTextID, bool bEnable ) = 0;
	virtual IGUIWindow*		GetTopicsWindow() = 0;
	virtual IGUIWindow*		CreatePlayerWindow(int nWidth, int nHeight) = 0;
	virtual void			SetGUIMode(bool bGUIMode) = 0;
	virtual bool			GetGUIMode() = 0;
};

#endif // IGUIManager_H