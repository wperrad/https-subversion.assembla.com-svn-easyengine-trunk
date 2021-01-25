#include "PlayerWindow.h"
#include "GUIManager.h"
#include "Utils2/rectangle.h"
#include "IRessource.h"

CPlayerWindow::CPlayerWindow(IGUIManager* pGUIManager, IRessourceManager& oRessourceManager, IRenderer& oRenderer, const CDimension& windowSize):
	CGUIWindow(oRessourceManager, oRenderer, windowSize, CRectangle(0, 0, windowSize.GetWidth(), windowSize.GetHeight())),
	m_pGUIManager((CGUIManager*)pGUIManager),
	m_pWindowBackground(NULL)
{
	m_pArmorWindow = new CGUIWidget(windowSize.GetWidth(), windowSize.GetHeight());
	SetPosition(100, 100);

	ITexture* pTexture = static_cast< ITexture* > (oRessourceManager.GetRessource("Gui/PlayerWindow.bmp"));
	m_pMesh->SetTexture(pTexture);
	SetGUIMode(true);
}

CPlayerWindow::~CPlayerWindow()
{
}

void CPlayerWindow::SetVisibility(bool bVisible)
{
	CGUIWindow::SetVisibility(bVisible);
}
