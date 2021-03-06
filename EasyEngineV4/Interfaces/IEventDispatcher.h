#ifndef IEVENTDISPATCHER_H
#define IEVENTDISPATCHER_H

#include "EEPlugin.h"

class IEntity;

class IEventDispatcher : public CPlugin
{
protected:
	IEventDispatcher() : CPlugin( nullptr, ""){}

public:
	enum TKeyEvent
	{
		T_KEYUP = 1,
		T_KEYDOWN,
	};
	enum TMouseEvent
	{
		T_MOVE = 1,
		T_LBUTTONDOWN,
		T_LBUTTONUP,
		T_RBUTTONDOWN,
		T_RBUTTONUP,
		T_WHEEL
	};
	enum TWindowEvent
	{
		T_WINDOWCLOSE,
		T_WINDOWRESIZE,
		T_WINDOWUPDATE,
		T_WINDOWCREATE,
		T_WINDOWSHOW,
		T_WINDOWDESTROY
	};

	enum TEntityEvent
	{
		T_UPDATE = 0,
		T_LOAD_RESSOURCE
	};

	typedef void( *TKeyCallback )( CPlugin*, TKeyEvent, int );
	typedef void( *TMouseCallback )( CPlugin*, TMouseEvent, int, int );
	typedef void( *TWindowCallback )( CPlugin*, TWindowEvent, int, int );
	typedef void( *TEntityCallback)( CPlugin*, TEntityEvent, IEntity*);

	virtual void 			AbonneToKeyEvent(CPlugin* pPlugin, TKeyCallback pfnCallback) = 0;
	virtual void 			AbonneToMouseEvent(CPlugin* pPlugin, TMouseCallback pfnCallback) = 0;
	virtual void 			AbonneToWindowEvent(CPlugin* pPlugin, TWindowCallback pfnCallback) = 0;
	virtual void 			AbonneToEntityEvent(CPlugin* plugin, TEntityCallback pfnCallback) = 0;
	virtual void			DesabonneToKeyEvent(CPlugin* pPlugin, TKeyCallback pfnCallback) = 0;
	virtual void			DesabonneToMouseEvent(TMouseCallback pfnCallback) = 0;
	virtual void			DesabonneToWindowEvent(TWindowCallback pfnCallback) = 0;

	virtual void 			DispatchKeyEvent( TKeyEvent, int nKeyCode ) = 0;
	virtual void 			DispatchMouseEvent( TMouseEvent, int x, int y ) = 0;
	virtual void			DispatchWindowEvent( TWindowEvent, int nWidth, int nHeight ) = 0;
	virtual void			DispatchEntityEvent(TEntityEvent e, IEntity* pEntity) = 0;

	virtual void			StopDispatcher() = 0;
	virtual void			StartDispatcher() = 0;
};

#endif // IEVENTDISPATCHER_H