#define GUIWINDOW_CPP

#include "guiwindow.h"



CGUIWindow::CGUIWindow( int nWidth, int nHeight ):
CGUIWidget( nWidth, nHeight ),
_bVisible(false)
{
}

CGUIWindow::~CGUIWindow(void)
{
}


void CGUIWindow::AddWidget(CGUIWidget* pWidget)
{
	_vWidget.push_back(pWidget);
	pWidget->SetPosition(pWidget->GetPosition().GetX() + _Position.GetX(), pWidget->GetPosition().GetY() + _Position.GetY());
	pWidget->SetParent(this);
}

void CGUIWindow::SetPosition(float fPosX, float fPosY)
{
	CGUIWidget::SetPosition(fPosX, fPosY);
	for (int i = 0; i < _vWidget.size(); i++) {
		CGUIWidget* pWidget = _vWidget[i];
		pWidget->SetPosition(pWidget->GetPosition().GetX() + fPosX, pWidget->GetPosition().GetY() + fPosY);
	}
}


size_t CGUIWindow::GetWidgetCount()const
{
	return _vWidget.size();
}

CGUIWidget* CGUIWindow::GetWidget( unsigned int nIndex )
{
	return _vWidget[nIndex];
}


void CGUIWindow::SetVisibility(bool bVisible)
{
	_bVisible = bVisible;
}


bool CGUIWindow::IsVisible()
{
	return _bVisible;
}


void CGUIWindow::Clear()
{
	/*for (int i=0 ; i<_vWidget.size() ; i++)
	{		
		CGUIWidget* pWidget = _vWidget[i];
		delete(_vWidget[i]);
		_vWidget[i] = NULL;
	}*/
	_vWidget.clear();
}

void CGUIWindow::Display()
{
	CGUIWidget::Display();
	for (int i = 0; i < _vWidget.size(); i++) {
		CGUIWidget* pWidget = _vWidget[i];
		pWidget->Display();
	}
}