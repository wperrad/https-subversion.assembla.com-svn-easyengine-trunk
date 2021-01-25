#define GUIWINDOW_CPP

#include "guiwindow.h"
#include "Utils2/Rectangle.h"
#include "IRessource.h"

CGUIWindow::CGUIWindow(string fileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, const CDimension& windowSize) :
	CGUIWidget(oRenderer, oRessourceManager, fileName, windowSize.GetWidth(), windowSize.GetHeight()),
	m_bVisible(false),
	m_bGUIMode(false)
{
}

CGUIWindow::CGUIWindow(IRessourceManager& oRessourceManager, IRenderer& oRenderer, const CDimension& windowSize, const CRectangle& skin):
	CGUIWidget(oRenderer, oRessourceManager, windowSize, skin),
	m_bVisible(false),
	m_bGUIMode(false)
{
}

CGUIWindow::~CGUIWindow(void)
{
}

void CGUIWindow::SetGUIMode(bool bGUIMode)
{
	m_bGUIMode = bGUIMode;
}

void CGUIWindow::AddWidget(CGUIWidget* pWidget)
{
	m_vWidget.push_back(pWidget);
	pWidget->SetPosition(pWidget->GetPosition().GetX() + _Position.GetX(), pWidget->GetPosition().GetY() + _Position.GetY());
	pWidget->SetParent(this);
}

void CGUIWindow::SetPosition(float fPosX, float fPosY)
{
	CGUIWidget::SetPosition(fPosX, fPosY);
	for (int i = 0; i < m_vWidget.size(); i++) {
		CGUIWidget* pWidget = m_vWidget[i];
		pWidget->SetPosition(pWidget->GetPosition().GetX() + fPosX, pWidget->GetPosition().GetY() + fPosY);
	}
}

bool CGUIWindow::IsGUIMode()
{
	return m_bGUIMode;
}

size_t CGUIWindow::GetWidgetCount()const
{
	return m_vWidget.size();
}

CGUIWidget* CGUIWindow::GetWidget( unsigned int nIndex )
{
	return m_vWidget[nIndex];
}


void CGUIWindow::SetVisibility(bool bVisible)
{
	m_bVisible = bVisible;
}


bool CGUIWindow::IsVisible()
{
	return m_bVisible;
}


void CGUIWindow::Clear()
{
	/*for (int i=0 ; i<_vWidget.size() ; i++)
	{		
		CGUIWidget* pWidget = _vWidget[i];
		delete(_vWidget[i]);
		_vWidget[i] = NULL;
	}*/
	m_vWidget.clear();
}

void CGUIWindow::Display()
{
	CGUIWidget::Display();
	for (int i = 0; i < m_vWidget.size(); i++) {
		CGUIWidget* pWidget = m_vWidget[i];
		pWidget->Display();
	}
}

void CGUIWindow::UpdateCallback(int nCursorXPos, int nCursorYPos, IInputManager::TMouseButtonState eButtonState)
{
	size_t nWidgetCount = GetWidgetCount();
	for (size_t i = 0; i<nWidgetCount; i++)
	{
		CGUIWidget* pWidget = GetWidget((unsigned int)i);
		pWidget->UpdateCallback(nCursorXPos, nCursorYPos, eButtonState);
	}
}