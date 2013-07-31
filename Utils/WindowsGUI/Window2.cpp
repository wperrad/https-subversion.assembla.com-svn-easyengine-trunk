#include "Window2.h"
#include <exception>
#include <sstream>
#include "Menu2.h"

using namespace std;

map< HWND, CWindow2* > CWindow2::s_mWindow;
map< int, string > CWindow2::s_mErrorString;

CWindow2* CWindow2::s_pThis = NULL;

CWindow2::Desc::Desc( int nWidth, int nHeight, string sClassName, WINDOW2CALLBACK pfnCallback  ): 
CWidget::Desc( nWidth, nHeight, NULL ),
m_pfnWindowCallback( pfnCallback ),
m_sClassName( sClassName ),
m_ColorBrush( WHITE_BRUSH ),
m_dwClassStyle( -1 ),
m_dwExStyle( 0 ),
m_dwStyle( -1 ),
m_pMenu( NULL ),
m_bCentered(false),
m_bFullscreen(false),
m_pfnOnUpdate(NULL),
m_nBits( 32 )
{
}

CWindow2::CallbackArgs::CallbackArgs()
{
}

CWindow2::CWindow2( const Desc& desc ):
CWidget( desc ),
m_pfnWindowCallback( NULL ),
m_bDestroy(false),
m_pfnOnUpdate( desc.m_pfnOnUpdate ),
m_bFullscreen( desc.m_bFullscreen ),
m_nBits( desc.m_nBits )
{
	s_pThis = this;
	InitCommonControls();
	InitErrorString();
	if ( TestDesc( desc ) == false )
	{
		exception e( "Desc non valide" );
		throw e;
	}
	m_pfnWindowCallback = desc.m_pfnWindowCallback;
	m_sClassName = desc.m_sClassName;
	m_pMenu = desc.m_pMenu;

	WNDCLASSA WndClass;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	if ( desc.m_ColorBrush != -1 )
		WndClass.hbrBackground = (HBRUSH)GetStockObject( desc.m_ColorBrush );// desc.m_hbrBackGround;
	else
		WndClass.hbrBackground = NULL;
	WndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	WndClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	WndClass.hInstance = GetModuleHandleA( NULL );
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = desc.m_sClassName.c_str();
	WndClass.lpszMenuName = "MainMenu";
	if ( desc.m_dwClassStyle == -1 )
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
	else
		WndClass.style = desc.m_dwClassStyle;
	LRESULT lRegister = RegisterClassA( &WndClass );
	if( lRegister == 0 )
	{
		ostringstream ssErrorMessage;
		ssErrorMessage << "Impossible d'enregistrer la classe " << WndClass.lpszClassName << ", code d'erreur " << GetLastError();
		exception e( ssErrorMessage.str().c_str() );
		throw e;
	}
	DWORD dwStyle;
	if ( desc.m_dwStyle == -1 )
		dwStyle = WS_OVERLAPPEDWINDOW;
	else
		dwStyle = desc.m_dwStyle;
	
	DWORD dwExStyle = desc.m_dwExStyle;
	if ( desc.m_bFullscreen )
	{
		DEVMODEA dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= desc.m_nWidth;
		dmScreenSettings.dmPelsHeight	= desc.m_nHeight;
		dmScreenSettings.dmBitsPerPel	= desc.m_nBits;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		
		if ( ChangeDisplaySettingsA(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL )
		{
			exception e( "The Requested Fullscreen Mode Is Not Supported By Video Card" );
			throw e;
		}
		dwExStyle = WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);
		RECT WindowRect;
		WindowRect.top = 0;
		WindowRect.left = 0;
		WindowRect.bottom = desc.m_nHeight;
		WindowRect.right = desc.m_nWidth;
		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size
	}
	CreateWnd( dwExStyle, WndClass.lpszClassName, desc.m_sClassName, dwStyle, desc.m_nPosX, desc.m_nPosY, desc.m_nWidth, desc.m_nHeight, m_pMenu );
	if ( desc.m_bCentered )
	{
		RECT WindowRect, oClientRect;
		GetWindowRect( GetHandle(), &WindowRect );
		GetClientRect( GetHandle(), &oClientRect );
		DEVMODEA oDevMode;
		memset( &oDevMode, 0, sizeof( DEVMODEA ) );
		oDevMode.dmSize = sizeof( DEVMODEA );
		EnumDisplaySettingsA( NULL, ENUM_CURRENT_SETTINGS, &oDevMode );
		oDevMode.dmPelsWidth;
		int xPos = ( oDevMode.dmPelsWidth - WindowRect.right )/ 2;
		int yPos = ( oDevMode.dmPelsHeight - WindowRect.bottom )/ 2;
		int xClientPos = ( oDevMode.dmPelsWidth - oClientRect.right )/ 2;
		int yClientPos = ( oDevMode.dmPelsHeight - oClientRect.bottom )/ 2;
		SetWindowPos( GetHandle(), NULL, xPos, yPos, xClientPos, yClientPos, SWP_NOSIZE );
	}
	if ( GetHandle() == NULL )
	{
		ostringstream ssErrorMessage;
		string sErrorString;
		GetStringByErrorCode( GetLastError(), sErrorString );
		if ( sErrorString.size() > 0 )
			ssErrorMessage << "Impossible de créer la fenêtre " << desc.m_sClassName << " : " << sErrorString ;
		else
			ssErrorMessage << "Impossible de créer la fenêtre " << desc.m_sClassName << " : code d'erreur " << GetLastError();
		exception e( ssErrorMessage.str().c_str() );
		throw e;
	}
}

LRESULT CALLBACK CWindow2::WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	CWindow2* pCurrentWindow = NULL;
	map< HWND, CWindow2* >::iterator itWindow = s_mWindow.find( hWnd );
	if ( itWindow != s_mWindow.end() )
		pCurrentWindow = itWindow->second;

	switch( msg )
	{
	case WM_CREATE:
		s_mWindow[ hWnd ] = s_pThis;
		pCurrentWindow = s_pThis;
		pCurrentWindow->SetHandle( hWnd );
		break;
	case WM_DESTROY:
		pCurrentWindow->m_bDestroy = true;
		PostQuitMessage( 0 );
		break;
	case WM_COMMAND:
		if ( pCurrentWindow->m_pMenu )
		{
			if ( pCurrentWindow->m_pMenu->m_pfnCallback )
				pCurrentWindow->m_pMenu->m_pfnCallback( (int)wParam );
			int end=0;
		}
		break;
	}

	if ( pCurrentWindow )
	{
		pCurrentWindow->CallCallback( CallbackArgs( pCurrentWindow, msg, wParam, lParam ) );
		for ( int i = 0; i < pCurrentWindow->GetChildCount(); i++ )
		{
			CWidget* pWidget = pCurrentWindow->GetChild( i );
			CallbackArgs args( pCurrentWindow, msg, wParam, lParam );
			pWidget->CallCallback( args );
			//pCurrentWindow->GetChild( i )->CallCallback( CallbackArgs( pCurrentWindow, msg, wParam, lParam ) );
		}
	}

	return DefWindowProcA( hWnd, msg, wParam, lParam );
}

CWindow2::~CWindow2(void)
{
}


void CWindow2::Show()const
{
	ShowWindow( GetHandle(), SW_SHOW );
	if ( UpdateWindow( GetHandle() ) == FALSE )
	{
		ostringstream ssErrorMessage;
		ssErrorMessage << "Erreur à l'update de la fenêtre " << m_sClassName << ", code d'erreur " << GetLastError();
		exception e( ssErrorMessage.str().c_str() );
		throw e;
	}
}

void CWindow2::ShowModal()const
{
	Show();
	MSG msg;
	while( !m_bDestroy )
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	
		{			
			TranslateMessage(&msg);				
			DispatchMessage(&msg);							
		}
		else	
		{
			if ( m_pfnOnUpdate )
				m_pfnOnUpdate( this );
		}
	}
}


void CWindow2::ShowModalWithoutUpdate()const
{
	Show();
	MSG msg;
	while ( GetMessageA( &msg, 0, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessageA( &msg );
	}
}

bool CWindow2::TestDesc( const Desc& desc)const
{
	return ( desc.m_sClassName != "" );
}

void CWindow2::InitErrorString()
{
	s_mErrorString[ ERROR_RESOURCE_TYPE_NOT_FOUND ] = "The specified resource type cannot be found in the image file";
}

void CWindow2::GetStringByErrorCode( int nErrorCode, string& sString )
{
	map< int, string >::iterator itError = s_mErrorString.find( nErrorCode );
	if ( itError != s_mErrorString.end() )
		sString = s_mErrorString[ nErrorCode ];
}

CWindow2* CWindow2::GetWindow( HWND hWnd )
{
	return s_mWindow[ hWnd ];
}

void CWindow2::Close()const
{
	if ( m_bFullscreen )
	{
		ChangeDisplaySettingsA( NULL, 0 );
		ShowCursor( TRUE );
	}
	DestroyWindow( GetHandle() );
	UnregisterClassA( m_sClassName.c_str(), GetModuleHandleA(NULL) );
}

void CWindow2::GetClientDimension( int& nWidth, int& nHeight )const
{
	RECT rect;
	GetClientRect( GetHandle(), &rect );
	nWidth = rect.right - rect.left;
	nHeight = rect.bottom - rect.top;
}

void CWindow2::CallCallback( CallbackArgs& args )
{
	if ( m_pfnWindowCallback )
		m_pfnWindowCallback( args.m_pParent, args.m_msg, args.m_wParam, args.m_lParam );
}

bool CWindow2::IsFullscreenMode()
{
	return m_bFullscreen;
}

void CWindow2::Setfocus()
{
	SetFocus( GetHandle() );
}

void CWindow2::SetForeground()
{
	SetForegroundWindow( GetHandle() );
}

int CWindow2::GetBits()
{
	return m_nBits;
}