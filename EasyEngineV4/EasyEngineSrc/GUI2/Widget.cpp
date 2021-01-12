#include "widget.h"
#include "../ressource2/Mesh.h"
#include "listener.h"
#include "../Renderer/Renderer.h"
#include "IShader.h"

using namespace std;


CWidget::CWidget( int nWidth, int nHeight ):
_pListener(NULL),
_bIsCursorInWidget( NULL )
{
	_Dimension.SetDimension( (float) nWidth, (float)nHeight);
}


CWidget::~CWidget(void)
{
}


void CWidget::SetRect(CMesh* pRect)
{
	_pRect = pRect;
}

void CWidget::Display()
{
	//float fXPos = 2.f*(float)_Position.GetX() / (float)Res.GetWidth();
	//float fYPos = 2.f*(float)_Position.GetY() / (float)Res.GetHeight();
	////s_pRenderer->Translate2D(fXPos,fYPos);
	//vector< float > vPos;
	//vPos.push_back( fXPos );
	//vPos.push_back( fYPos );
	//oShaderManager.SendUniformVec2Array( "vImagePosition", vPos );
	_pRect->Update();
}


void CWidget::SetPosition( float fPosX, float fPosY )
{
	_Position.SetX( fPosX );
	_Position.SetY( fPosY );
}

void CWidget::SetY( float fY )
{
	_Position.SetY( fY );
}


CPosition CWidget::GetPosition() const
{
	return _Position;
}

void CWidget::GetLogicalPosition( float& x, float& y, int nResWidth, int nResHeight ) const
{
	x = 2.f*(float)_Position.GetX() / nResWidth;
	x = 2.f*(float)_Position.GetY() / nResHeight;
}

CDimension CWidget::GetDimension()
{
	return _Dimension;
}

void CWidget::SetListener(CListener* pListener)
{
	_pListener = pListener;
}

void CWidget::UpdateCallback( int nCursorXPos, int nCursorYPos, const unsigned int nButtonState )
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
					_pListener->ExecuteCallBack( IGUIManager::EVENT_LMOUSERELEASED, this, nCursorXPos, nCursorYPos);
					return;
				}
				if ( _NextCursorPos.GetX() != nCursorXPos || _NextCursorPos.GetY() != nCursorYPos )
				{
					_pListener->ExecuteCallBack( IGUIManager::EVENT_MOUSEMOVE, this, nCursorXPos, nCursorYPos);
					_NextCursorPos.SetPosition(static_cast<float> (nCursorXPos), static_cast<float> (nCursorYPos) );
					return;
				}				
			}
		}	
		if ( !bIsCursorInWidget && _bIsCursorInWidget )
		{
			_pListener->ExecuteCallBack( IGUIManager::EVENT_MOUSEEXITED, this, nCursorXPos, nCursorYPos);
			_bIsCursorInWidget = false;
			return;
		}
		_pListener->ExecuteCallBack( IGUIManager::EVENT_NONE, this, nCursorXPos, nCursorYPos);
	}
}


void CWidget::SetSkinName(const string& szSkinName)
{
	_strSkinName = string (szSkinName);
}


string CWidget::GetSkinName()
{
	return _strSkinName;
}

void CWidget::SetPosition(CPosition p)
{
	_Position = p;
}