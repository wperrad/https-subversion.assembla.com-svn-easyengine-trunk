#include "Button2.h"

using namespace std;

CButton2::Desc::Desc( int nWidth, int nHeight, CWidget* pParent, string sCaption  ) : 
CControl::Desc( nWidth, nHeight, pParent ),
m_sCaption( sCaption )
{
}



CButton2::CButton2( const CButton2::Desc& desc ) : 
CControl( desc ),
m_pfnCallback( NULL ),
m_pAdditionalArgCallback( NULL )
{
	CreateWnd( 0, "BUTTON", desc.m_sCaption, WS_CHILD | WS_VISIBLE, desc.m_nPosX, desc.m_nPosY, desc.m_nWidth, desc.m_nHeight, NULL );
}

CButton2::~CButton2(void)
{
}

void CButton2::SetCallback( BUTTONCALLBACK p, void* pArg )
{
	m_pfnCallback = p;
	m_pAdditionalArgCallback = pArg;
}

void CButton2::CallCallback( CallbackArgs& args )
{
	//MessageBoxA(NULL, "Appui d'un bouton", "", MB_OK );
	switch( args.m_msg )
	{
	case WM_COMMAND:
		{
			if ( GetHandle() == (HWND)args.m_lParam && m_pfnCallback )
				m_pfnCallback( this, BUTTON_EVENT_PRESS, m_pAdditionalArgCallback );
		}
		break;
	}
}