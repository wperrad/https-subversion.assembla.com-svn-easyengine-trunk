#include <windows.h>
#include "OGLAPI/OGLAPI.h"
#include <gl/gl.h>

#include "../RendererClean/OglRenderer.h"

int g_nCycle = 0;

void OnPaint( const COglRenderer& r )
{
	glBegin( GL_TRIANGLES );
	glVertex3d( 0, 0, -2 );
	glVertex3d( 0, 1, -2 );
	glVertex3d( 1, 0, -2 );
	glEnd();
	if ( g_nCycle > 1000 )
		r.KillWindow();
	g_nCycle++;
}

void OnUpdate()
{
	//glBegin( GL_TRIANGLES );
	//glVertex3d( 0, 0, -2 );
	//glVertex3d( 0, 1, -2 );
	//glVertex3d( 1, 0, -2 );
	//glEnd();
	//if ( g_nCycle > 1000 )
	//	r.KillWindow();
	//g_nCycle++;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	COglRenderer::Desc desc( 640, 480, 32 );
	desc.m_pfnOnPaint = OnPaint;
	COglRenderer r( desc );
	r.DisplayWindow();

	return TRUE;
}