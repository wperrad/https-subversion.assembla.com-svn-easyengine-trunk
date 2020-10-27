#ifndef _ICONSOLE_H_
#define _ICONSOLE_H_

#include "EEPlugin.h"
#include <string>

class IActionManager;
class IScriptManager;
class IGUIManager;

using namespace std;

class IConsole : public CPlugin
{
protected:
	IConsole( const Desc& oDesc ) : CPlugin( oDesc.m_pParent, oDesc.m_sName ){}

public:
	struct Desc : public CPlugin::Desc
	{		
		IInputManager&		m_oInputManager;
		IScriptManager&		m_oScriptManager;
		IGUIManager&		m_oGUIManager;
		int					xPos;
		int 				yPos;
		int					m_nWidth;
		int					m_nHeight;
		Desc( IInputManager& oInputManager, IGUIManager& oGUIManager, IScriptManager& oScriptManager ) :
			CPlugin::Desc( NULL, "" ),
			m_oInputManager( oInputManager ),
			m_oGUIManager( oGUIManager ),
			m_oScriptManager( oScriptManager ),
			m_nWidth( 420 ),
			m_nHeight( 300 ){}
	};
	virtual void	Update() = 0;
	virtual bool	IsOpen() = 0;
	virtual void	Open( bool bOpen ) = 0;
	virtual void	Cls() = 0;
	virtual void	Print( string s ) = 0;
	virtual int		GetConsoleShortCut() = 0;
	virtual void	SetConsoleShortCut(int key) = 0;
};

#endif // _ICONSOLE_H_