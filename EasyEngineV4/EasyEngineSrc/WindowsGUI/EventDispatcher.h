#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <vector>
#include "IEventDispatcher.h"

using namespace std;

class IEntity;

class CEventDispatcher : public IEventDispatcher
{
	std::vector< pair< CPlugin*, TKeyCallback > >		m_vKeyAbonnedPlugins;
	std::vector< pair< CPlugin*, TMouseCallback > >		m_vMouseAbonnedPlugins;
	std::vector< pair< CPlugin*, TWindowCallback > >	m_vWindowAbonnedPlugins;
	std::vector< pair< CPlugin*, TEntityCallback>>		m_vEntityAbonnedPlugins;

	bool m_bDispatcherWorking;

public:

	CEventDispatcher(EEInterface& oInterface);

	void 			AbonneToKeyEvent( CPlugin* pPlugin, TKeyCallback pfnCallback ) override;
	void 			AbonneToMouseEvent( CPlugin* pPlugin, TMouseCallback pfnCallback ) override;
	void 			AbonneToWindowEvent( CPlugin* pPlugin, TWindowCallback pfnCallback ) override;
	void			AbonneToEntityEvent(CPlugin* pPlugin, TEntityCallback pfnEntityCallback) override;
	void			DesabonneToMouseEvent(TMouseCallback pfnCallback);
	void			DesabonneToWindowEvent(TWindowCallback pfnCallback);	

	void 			DispatchKeyEvent( TKeyEvent, int nKeyCode ) override;
	void 			DispatchMouseEvent( TMouseEvent, int x, int y ) override;
	void			DispatchWindowEvent( TWindowEvent, int nWidth, int nHeight ) override;
	void			DispatchEntityEvent(TEntityEvent e, IEntity* pEntity) override;

	void			StopDispatcher();
	void			StartDispatcher();
	string			GetName() override;
};

extern "C" _declspec(dllexport) IEventDispatcher* CreateEventDispatcher(EEInterface& oInterface);

#endif // EVENTDISPATCHER_H