#ifndef IEVENTDISPATCHER_H
#define IEVENTDISPATCHER_H

#include "EEPlugin.h"

class IEventDispatcher : public CPlugin
{
protected:
	IEventDispatcher( const Desc& oDesc ) : CPlugin( oDesc.m_pParent, oDesc.m_sName ){}

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

	typedef void( *TKeyCallback )( CPlugin*, TKeyEvent, int );
	typedef void( *TMouseCallback )( CPlugin*, TMouseEvent, int, int );
	typedef void( *TWindowCallback )( CPlugin*, TWindowEvent, int, int );

	virtual void 			AbonneToKeyEvent( CPlugin* pPlugin, TKeyCallback pfnCallback ) = 0;
	virtual void 			AbonneToMouseEvent( CPlugin* pPlugin, TMouseCallback pfnCallback ) = 0;
	virtual void 			AbonneToWindowEvent( CPlugin* pPlugin, TWindowCallback pfnCallback ) = 0;
	virtual void			DesabonneToWindowEvent(TWindowCallback pfnCallback) = 0;

	virtual void 			DispatchKeyEvent( TKeyEvent, int nKeyCode ) = 0;
	virtual void 			DispatchMouseEvent( TMouseEvent, int x, int y ) = 0;
	virtual void			DispatchWindowEvent( TWindowEvent, int nWidth, int nHeight ) = 0;

	virtual void			StopDispatcher() = 0;
	virtual void			StartDispatcher() = 0;
};

#endif // IEVENTDISPATCHER_H