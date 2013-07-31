#include "OGLAPI.h"

// System
#include <windows.h>

// stl
#include <map>

#include "OGLWindow.h"

using namespace std;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

map< int, COGLWindow* >	g_mOGLWindow;

COGLAPI::COGLAPI()
{
	return;
}

LRESULT CALLBACK GLWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
	case WM_KEYDOWN:
		switch ( wParam )
		{
		case VK_ESCAPE:
			DestroyWindow( hWnd );
			break;
		}
	case WM_DESTROY:
		PostQuitMessage( 0 );
	}
	return 0;
}

int	COGLAPI::CreateOGLWindow( const GLPARAM& param )
{
	COGLWindow::Desc desc( param.m_nWidth, param.m_nHeight );
	desc.m_nBits = param.m_nBits;
	desc.SetPaintCallback( param.OnPaint );
	desc.m_bFullscreen = param.m_bFullscreen;
	desc.m_pfnOnUpdate = param.OnUpdate;
	COGLWindow* pOGLWindow = new COGLWindow( desc );
	int nNumber = (int)g_mOGLWindow.size() + 1;
	g_mOGLWindow.insert( map< int, COGLWindow* >::value_type( nNumber ,pOGLWindow ) );
	return nNumber;	
}

void COGLAPI::Display( int nResID )
{
	map< int, COGLWindow* >::iterator itWindow = g_mOGLWindow.find( nResID );
	itWindow->second->ShowModal();
}

void COGLAPI::KillWindow( int nResID ) const
{
	map< int, COGLWindow* >::iterator itWindow = g_mOGLWindow.find( nResID );
	itWindow->second->Close();
}