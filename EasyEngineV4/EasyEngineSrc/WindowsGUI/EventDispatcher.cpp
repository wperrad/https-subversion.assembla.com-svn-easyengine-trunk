#include "EventDispatcher.h"

using namespace std;

CEventDispatcher::CEventDispatcher( const IEventDispatcher::Desc& oDesc ) :
IEventDispatcher( oDesc )
{
}

void CEventDispatcher::AbonneToKeyEvent( CPlugin* pPlugin, TKeyCallback pfnCallback )
{
	m_vKeyAbonnedPlugins.push_back( pair< CPlugin*, TKeyCallback >::pair( pPlugin, pfnCallback ) );
}

void CEventDispatcher::AbonneToMouseEvent( CPlugin* pPlugin, TMouseCallback pfnCallback )
{
	m_vMouseAbonnedPlugins.push_back( pair< CPlugin*, TMouseCallback >::pair( pPlugin, pfnCallback ) );
}

void CEventDispatcher::AbonneToWindowEvent( CPlugin* pPlugin, TWindowCallback pfnCallback )
{
	m_vWindowAbonnedPlugins.push_back( pair< CPlugin*, TWindowCallback >::pair( pPlugin, pfnCallback ) );
}

void CEventDispatcher::DesabonneToWindowEvent(TWindowCallback pfnCallback)
{
	for (vector< pair< CPlugin*, TWindowCallback > >::iterator it = m_vWindowAbonnedPlugins.begin(); it != m_vWindowAbonnedPlugins.end(); it++) {
		if (it->second == pfnCallback) {
			m_vWindowAbonnedPlugins.erase(it);
			break;
		}
	}
}

void CEventDispatcher::DispatchKeyEvent( TKeyEvent e, int nKeyCode )
{
	for ( unsigned int i = 0; i < m_vKeyAbonnedPlugins.size(); i++ )
	{
		CPlugin* pPlugin = m_vKeyAbonnedPlugins[ i ].first;
		m_vKeyAbonnedPlugins[ i ].second( pPlugin, e, nKeyCode );
	}
}

void CEventDispatcher::DispatchMouseEvent( TMouseEvent e, int x, int y )
{
	for ( unsigned int i = 0; i < m_vMouseAbonnedPlugins.size(); i++ )
	{
		CPlugin* pPlugin = m_vMouseAbonnedPlugins[ i ].first;
		m_vMouseAbonnedPlugins[ i ].second( pPlugin, e, x, y );
	}
}

void CEventDispatcher::DispatchWindowEvent( TWindowEvent e, int nWidth, int nHeight )
{
	for ( unsigned int i = 0; i < m_vWindowAbonnedPlugins.size(); i++ )
	{
		CPlugin* pPlugin = m_vWindowAbonnedPlugins[ i ].first;
		IEventDispatcher::TWindowCallback callback = m_vWindowAbonnedPlugins[ i ].second;
		callback( pPlugin,e, nWidth, nHeight );
	}
}

extern "C" _declspec(dllexport) IEventDispatcher* CreateEventDispatcher( const IEventDispatcher::Desc& oDesc )
{
	return new CEventDispatcher( oDesc );
}