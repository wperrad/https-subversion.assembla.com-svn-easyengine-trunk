#ifndef OGL_WINDOW_H
#define OGL_WINDOW_H

#include "../WindowsGUI/window2.h"

typedef void(*PFNVOID)();
typedef void(*PFNWPARAM)( WPARAM );
typedef void(*PFNINTINT)( int, int );

class COGLWindow : public CWindow2
{
	HDC							m_hdc;
	void						OnPaint();
	void						OnSize( int nWidth, int nHeight )const;
	//WNDPROC						m_pfnGLWndProc;
	void						( *m_pfnOnPaint )();
	void						( *m_pfnOnKeyDown )( WPARAM );
	void						( *m_pfnOnKeyUp )( WPARAM );
	void						( *m_pfnOnMouseMove )( int, int );
	void						( *m_pfnOnUpdate)();
	static LRESULT 				OGLWndProc( CWidget* pParent, UINT msg, WPARAM wParam, LPARAM lParam );
	static void					OnUpdate( const CWindow2* );
public:


	struct Desc : public CWindow2::Desc
	{
	private:
		void					( *m_pfnOnPaint )();
		void					( *m_pfnOnKeyDown )( WPARAM );
		void					( *m_pfnOnKeyUp )( WPARAM );
		void					( *m_pfnOnMouseMove )( int, int );
		WINDOW2CALLBACK			m_pfnGLWndProc;
	public:
								Desc( int width, int height );
		void					SetPaintCallback( void(*)() );
		void					SetKeyDownCallback( void(*)( WPARAM ) );
		void					SetKeyUpCallback( void(*)( WPARAM ) );
		void					SetMouseMoveCallback( void(*)( int, int ) );
		PFNVOID					GetPaintCallback()const;
		PFNWPARAM				GetKeyUpCallback()const;
		PFNWPARAM				GetKeyDownCallback()const;
		PFNINTINT				GetMouseMoveCallback()const;
		void					(*m_pfnOnUpdate)();
	};
	COGLWindow( const COGLWindow::Desc& desc );
	~COGLWindow(void);

	void CallCallback(){}
	
};

#endif // OGL_WINDOW_H