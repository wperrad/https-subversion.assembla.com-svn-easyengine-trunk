#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include <string>
#include "OGLWindow.h"
#include "WinError2.h"

using namespace std;

void LaunchException()
{
	CWinError we;
	string sError;
	we.GetErrorString( GetLastError(), sError );
	exception e( sError.c_str() );
	throw e;
}

COGLWindow::Desc::Desc( int nWidth, int nHeight ) : 
CWindow2::Desc( nWidth, nHeight, "OpenGL", OGLWndProc ),
m_pfnOnPaint( NULL ),
m_pfnOnKeyUp( NULL ),
m_pfnOnKeyDown( NULL ),
m_pfnOnMouseMove( NULL ),
m_pfnGLWndProc( NULL ),
m_pfnOnUpdate( NULL )
{
	CWindow2::Desc::m_pfnOnUpdate = COGLWindow::OnUpdate;
	m_ColorBrush = -1;
}

PFNVOID COGLWindow::Desc::GetPaintCallback() const
{
	return m_pfnOnPaint;
}

PFNWPARAM COGLWindow::Desc::GetKeyUpCallback()const
{
	return m_pfnOnKeyUp;
}


PFNWPARAM COGLWindow::Desc::GetKeyDownCallback()const
{
	return m_pfnOnKeyDown;
}

PFNINTINT COGLWindow::Desc::GetMouseMoveCallback()const
{
	return m_pfnOnMouseMove;
}

void COGLWindow::Desc::SetKeyDownCallback( void(*OnKeyDown)(WPARAM) )
{
	m_pfnOnKeyDown = OnKeyDown;
}

void COGLWindow::Desc::SetKeyUpCallback( void(*OnKeyUp)(WPARAM) )
{
	m_pfnOnKeyUp = OnKeyUp;
}

void COGLWindow::Desc::SetPaintCallback( void(*OnPaint)() )
{
	m_pfnOnPaint = OnPaint;
}

COGLWindow::COGLWindow( const COGLWindow::Desc& desc ) : 
CWindow2( (const CWindow2::Desc&)desc ),
m_pfnOnKeyUp( desc.GetKeyUpCallback() ),
m_pfnOnKeyDown( desc.GetKeyDownCallback() ),
m_pfnOnPaint( desc.GetPaintCallback() ),
m_pfnOnUpdate( desc.m_pfnOnUpdate )
{

	m_pfnOnMouseMove = desc.GetMouseMoveCallback();
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.cColorBits = desc.m_nBits;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
	pfd.nVersion = 1;
	m_hdc = GetDC( GetHandle() );
	int format = ChoosePixelFormat( m_hdc, &pfd );
	if ( SetPixelFormat( m_hdc, format, &pfd ) == false ) LaunchException();
	HGLRC hrc = wglCreateContext( m_hdc );
	if ( hrc == NULL ) LaunchException();
	if ( wglMakeCurrent( m_hdc, hrc ) == FALSE ) LaunchException();
}

COGLWindow::~COGLWindow(void)
{
}

void COGLWindow::OnPaint()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	if ( m_pfnOnPaint )
		m_pfnOnPaint();
	SwapBuffers( m_hdc );
}

void COGLWindow::OnSize( int nWidth, int nHeight )const
{
	glViewport( 0, 0, nWidth, nHeight );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45.f, (float)nWidth / (float)nHeight, 1, 400000 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

LRESULT COGLWindow::OGLWndProc( CWidget* pWindow, UINT msg, WPARAM wParam, LPARAM lParam )
{
	COGLWindow* pOGLWindow = static_cast< COGLWindow* > ( pWindow );
	switch ( msg )
	{
	case WM_PAINT:
		pOGLWindow->OnPaint();
		break;
	case WM_SIZE:
		pOGLWindow->OnSize( LOWORD( lParam), HIWORD( lParam ) );
		break;
	case WM_KEYUP:
		if ( pOGLWindow->m_pfnOnKeyUp )
			pOGLWindow->m_pfnOnKeyUp( wParam );
		break;
	case WM_KEYDOWN:
		if ( pOGLWindow->m_pfnOnKeyDown )
			pOGLWindow->m_pfnOnKeyDown( wParam );
		break;
	case WM_MOUSEMOVE:
		if ( pOGLWindow->m_pfnOnMouseMove )
			pOGLWindow->m_pfnOnMouseMove( LOWORD( lParam ), HIWORD( lParam ) );
		break;
	}
	return 0;
}

void COGLWindow::OnUpdate( const CWindow2* pWindow )
{
	const COGLWindow* pOglWindow = static_cast< const COGLWindow* > ( pWindow );
	if ( pOglWindow->m_pfnOnUpdate )
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		pOglWindow->m_pfnOnUpdate();
		SwapBuffers( pOglWindow->m_hdc );
	}
}