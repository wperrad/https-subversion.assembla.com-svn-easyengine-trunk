#include "Control.h"
#include <exception>

using namespace std;

CControl::Desc::Desc( int nWidth, int nHeight, CWidget* pParent ) : 
CWidget::Desc( nWidth, nHeight, pParent )
{
	
}

CControl::CControl( const Desc& desc ) : CWidget( desc )
{
	if ( desc.m_pParent == NULL )
	{
		exception e( "Un controle doit obligatoirement avoir un parent" );
		throw e;
	}
}

CControl::~CControl(void)
{
}


//void CControl::CreateWnd( DWORD dwExStyle, std::string sClassName, std::string sWindowName, DWORD dwStyle, int nPosX, int nPosY, int nWidth, int nHeight, const CMenu2* pMenu )
//{
//
//	CWidget::CreateWnd( dwExStyle, sClassName, sWindowName, dwStyle, nPosX, nPosY, nWidth, nHeight, pMenu );
//}