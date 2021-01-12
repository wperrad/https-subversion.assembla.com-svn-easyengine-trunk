#include "PlayerWindow.h"
#include "GUIManager.h"
#include "Utils2/rectangle.h"
#include "IRessource.h"

CPlayerWindow::CPlayerWindow(IGUIManager* pGUIManager, int nWidth, int nHeight):
	CGUIWindow(nWidth, nHeight),
	m_pGUIManager((CGUIManager*)pGUIManager),
	m_pWindowBackground(NULL)
{
	m_pArmorWindow = new CGUIWidget(nWidth, nHeight);
	
	CRectangle rect;
	rect.SetPosition(0, 200);
	rect.SetDimension(nWidth, nHeight);
	IMesh* pRect = m_pGUIManager->CreateImageFromFile("PlayerWindow.bmp", rect, _Dimension);
	SetRect(pRect);
}


CPlayerWindow::~CPlayerWindow()
{
}

void CPlayerWindow::SetVisibility(bool bVisible)
{
	CGUIWindow::SetVisibility(bVisible);
}