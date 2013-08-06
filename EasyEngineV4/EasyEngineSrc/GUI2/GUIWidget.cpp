#define GUIWIDGET_CPP

#include "GUIWidget.h"
#include "listener.h"
#include "IRessource.h"
#include "Exception.h"
#include "IShader.h"

using namespace std;

int CGUIWidget::s_nScreenResWidth = 0;
int CGUIWidget::s_nScreenResHeight = 0;
IShader* CGUIWidget::s_pShader = NULL;


void CGUIWidget::Init( int nResX, int nResY, IShader* pShader )
{
	s_nScreenResWidth = nResX;
	s_nScreenResHeight = nResY;
	s_pShader = pShader;
}

CGUIWidget::CGUIWidget( int nWidth, int nHeight ):
_pListener(NULL),
_bIsCursorInWidget( NULL ),
m_pMesh( NULL )
{
	if( s_pShader == NULL )
	{
		CWidgetNotInitialized e( "" );
		throw e;
	}
	_Dimension.SetDimension( (float) nWidth, (float)nHeight);
}


CGUIWidget::~CGUIWidget(void)
{
}

bool CGUIWidget::operator==( const CGUIWidget& w )
{
	return *m_pMesh == *w.m_pMesh;
}

void CGUIWidget::SetRect( IRessource* pMesh )
{
	m_pMesh = static_cast< IMesh* >( pMesh );
}

void CGUIWidget::Display()
{
	float fWidgetLogicalPosx, fWidgetLogicalPosy;
	GetLogicalPosition( fWidgetLogicalPosx, fWidgetLogicalPosy, s_nScreenResWidth, s_nScreenResHeight );
	vector< float > vPos;
	vPos.push_back( fWidgetLogicalPosx );
	vPos.push_back( fWidgetLogicalPosy );
	s_pShader->SendUniformVec2Array( "vImagePosition", vPos );
	m_pMesh->Update();
}


void CGUIWidget::SetPosition( float fPosX, float fPosY )
{
	_Position.SetX( fPosX );
	_Position.SetY( fPosY );
}

void CGUIWidget::SetY( float fY )
{
	_Position.SetY( fY );
}


CPosition CGUIWidget::GetPosition() const
{
	return _Position;
}

void CGUIWidget::GetLogicalPosition( float& x, float& y, int nResWidth, int nResHeight ) const
{
	x = 2.f*(float)_Position.GetX() / nResWidth;
	y = - 2.f*(float)_Position.GetY() / nResHeight;
}

void CGUIWidget::GetLogicalDimension( float& x, float& y, int nResWidth, int nResHeight ) const
{
	x = 2.f*(float)_Dimension.GetWidth() / nResWidth;
	y = 2.f*(float)_Dimension.GetHeight() / nResHeight;
}

CDimension CGUIWidget::GetDimension() const
{
	return _Dimension;
}

void CGUIWidget::SetListener(CListener* pListener)
{
	_pListener = pListener;
}

void CGUIWidget::UpdateCallback( int nCursorXPos, int nCursorYPos, const unsigned int nButtonState )
{
	bool bIsCursorInWidget = false;
	if (_pListener)
	{
		if (nCursorXPos > _Position.GetX() && nCursorXPos < _Position.GetX() + _Dimension.GetWidth())
		{
			if (nCursorYPos > _Position.GetY() && nCursorYPos < _Position.GetY() + _Dimension.GetHeight())
			{
				bIsCursorInWidget = true;
				if (!_bIsCursorInWidget)
				{
					_pListener->ExecuteCallBack( IGUIManager::EVENT_MOUSEENTERED );
					_bIsCursorInWidget = true;
					return;
				}
				if ( nButtonState == WM_LBUTTONDOWN )
				{
					_pListener->ExecuteCallBack( IGUIManager::EVENT_LMOUSECLICK );
					return;
				}
				if ( nButtonState == WM_LBUTTONUP)
				{
					_pListener->ExecuteCallBack( IGUIManager::EVENT_LMOUSERELEASED );
					return;
				}
				if ( _NextCursorPos.GetX() != nCursorXPos || _NextCursorPos.GetY() != nCursorYPos )
				{
					_pListener->ExecuteCallBack( IGUIManager::EVENT_MOUSEMOVE);
					_NextCursorPos.SetPosition(static_cast<float> (nCursorXPos), static_cast<float> (nCursorYPos) );
					return;
				}				
			}
		}	
		if ( !bIsCursorInWidget && _bIsCursorInWidget )
		{
			_pListener->ExecuteCallBack( IGUIManager::EVENT_MOUSEEXITED );
			_bIsCursorInWidget = false;
			return;
		}
		_pListener->ExecuteCallBack( IGUIManager::EVENT_NONE );
	}
}


void CGUIWidget::SetSkinName(const string& szSkinName)
{
	_strSkinName = string (szSkinName);
}


string CGUIWidget::GetSkinName()
{
	return _strSkinName;
}

void CGUIWidget::SetPosition(CPosition p)
{
	_Position = p;
}