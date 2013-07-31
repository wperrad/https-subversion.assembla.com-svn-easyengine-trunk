#include "Widget.h"
#include "Menu2.h"

using namespace std;

CWidget::Desc::Desc( int nWidth, int nHeight, CWidget* pParent ):
m_nPosX(0),
m_nPosY(0),
m_nWidth( nWidth ),
m_nHeight( nHeight ),
m_pParent( pParent )
{
}

CWidget::CallbackArgs::CallbackArgs( CWidget* pParent, UINT msg, WPARAM wParam, LPARAM lParam ):
m_pParent( pParent ),
m_msg( msg ),
m_wParam( wParam ),
m_lParam( lParam )
{
}

CWidget::CWidget( const Desc& desc ) :
m_pParent( desc.m_pParent )
{
}

CWidget::~CWidget(void)
{	
}

HWND CWidget::GetHandle()const
{
	return m_hWnd;
}

void CWidget::SetPosition( int x, int y )
{
	SetWindowPos( GetHandle(), NULL, x, y, 0, 0, SWP_NOSIZE );
}

void CWidget::GetPosition( int& x, int& y )const
{
	RECT rect;
	GetWindowRect( m_hWnd, &rect );
	x = rect.left;
	y = rect.top;
}

void CWidget::GetDimension( int& nWidth, int& nHeight )const
{
	RECT rect;
	GetWindowRect( m_hWnd, &rect );
	nWidth = rect.right - rect.left;
	nHeight = rect.bottom - rect.top;
}

void CWidget::GetClientDimension( int& nWidth, int& nHeight )const
{
	RECT rect;
	GetClientRect( m_hWnd, &rect );
	nWidth = rect.right - rect.left;
	nHeight = rect.bottom - rect.top;
}

void CWidget::SetCaption( std::string sCaption )
{
	SetWindowTextA( GetHandle(), sCaption.c_str() );
}


void CWidget::CreateWnd( DWORD dwExStyle, string sClassName, string sWindowName, DWORD dwStyle, int nPosX, int nPosY, int nWidth, int nHeight, const CMenu2* pMenu )
{
	HWND hParent = 0;
	if ( m_pParent )
	{
		hParent = m_pParent->GetHandle();
		m_pParent->AddChild( this );
	}

	HMENU hMenu = NULL;
	if ( pMenu )
		hMenu = pMenu->m_hHandle;
	m_hWnd = CreateWindowExA( dwExStyle, sClassName.c_str(), sWindowName.c_str() , dwStyle, nPosX, nPosY, nWidth, nHeight, hParent, hMenu, GetModuleHandleA(NULL), 0 );
}


int CWidget::GetChildCount()const
{
	return (int)m_vChild.size();
}

CWidget* CWidget::GetChild( int iIndex )const
{
	return m_vChild[ iIndex ];
}

void CWidget::AddChild( CWidget* pChild )
{
	m_vChild.push_back( pChild );
}

HDC CWidget::GetDeviceContext()const
{
	return GetDC( GetHandle() );
}

void CWidget::ForcePaint()
{
	SendMessageA( m_hWnd, WM_GETMINMAXINFO, SIZE_MAXSHOW, 0x02000100 );
	SendMessageA( m_hWnd, WM_SIZE, 0, 0 );
	SendMessageA( m_hWnd, WM_PAINT, 0, 0 );
}

void CWidget::SendMessage2( CallbackArgs arg )
{
	SendMessageA( m_hWnd, arg.m_msg, arg.m_wParam, arg.m_lParam );
}

void CWidget::GetClientPosition( int& x, int& y )const
{
	RECT wRect, cRect;
	GetWindowRect( m_hWnd, &wRect );
	GetClientRect( m_hWnd, &cRect );
	x = wRect.left - cRect.left;
	y = wRect.top - cRect.top;
}

void CWidget::SetHandle( HWND hWnd )
{
	m_hWnd = hWnd;
}

void CWidget::UnlinkChild( int iIndex )
{
	m_vChild[ iIndex ] = m_vChild[ m_vChild.size() - 1 ];
	m_vChild.resize( m_vChild.size() - 1 );
}