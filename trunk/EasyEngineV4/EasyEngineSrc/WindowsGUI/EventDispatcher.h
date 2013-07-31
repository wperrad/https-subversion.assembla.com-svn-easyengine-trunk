#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <vector>
#include "IEventDispatcher.h"

class CEventDispatcher : public IEventDispatcher
{
	std::vector< std::pair< CPlugin*, TKeyCallback > >		m_vKeyAbonnedPlugins;
	std::vector< std::pair< CPlugin*, TMouseCallback > >	m_vMouseAbonnedPlugins;
	std::vector< std::pair< CPlugin*, TWindowCallback > >	m_vWindowAbonnedPlugins;


public:

	CEventDispatcher( const IEventDispatcher::Desc& oDesc );

	void 			AbonneToKeyEvent( CPlugin* pPlugin, TKeyCallback pfnCallback );
	void 			AbonneToMouseEvent( CPlugin* pPlugin, TMouseCallback pfnCallback );
	void 			AbonneToWindowEvent( CPlugin* pPlugin, TWindowCallback pfnCallback );

	void 			DispatchKeyEvent( TKeyEvent, int nKeyCode );
	void 			DispatchMouseEvent( TMouseEvent, int x, int y );
	void			DispatchWindowEvent( TWindowEvent, int nWidth, int nHeight );
};

extern "C" _declspec(dllexport) IEventDispatcher* CreateEventDispatcher( const IEventDispatcher::Desc& );

#endif // EVENTDISPATCHER_H