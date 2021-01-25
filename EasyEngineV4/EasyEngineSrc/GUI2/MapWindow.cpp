#include "MapWindow.h"
#include "Utils2/Rectangle.h"
#include "IRessource.h"
#include "IEntity.h"
#include "GUIManager.h"

CMapWindow::CMapWindow(IGUIManager* pGUIManager, IScene& oScene, IRessourceManager& oRessourceManager, IRenderer& oRenderer, int nWidth, int nHeight):
	CGUIWindow("Gui/map-bkg.bmp", oRessourceManager, oRenderer, CDimension(520, 296)),
	m_pGUIManager((CGUIManager*)pGUIManager),
	m_oScene(oScene),
	m_oRenderer(oRenderer),
	m_oMap(512, 288)
{
	int screenWidth, screenHeight;
	m_oRenderer.GetResolution(screenWidth, screenHeight);
	SetPosition(screenWidth - GetDimension().GetWidth(), 0);
	CRectangle skin;
#if 0
	skin.SetPosition(m_oMap.GetDimension().GetWidth(), 0);
	skin.SetDimension(-m_oMap.GetDimension().GetWidth(), m_oMap.GetDimension().GetHeight());
#else
	skin.SetPosition(0, 0);
	skin.SetDimension(m_oMap.GetDimension());
#endif // 0
	IMesh* pQuadMap = CreateQuad(m_oRenderer, oRessourceManager, m_oMap.GetDimension(), skin);
	pQuadMap->SetTexture(m_oScene.GetMapTexture());
	pQuadMap->SetShader(m_pShader);
	m_oMap.SetQuad(pQuadMap);
	AddWidget(&m_oMap);
	m_oMap.Translate(4, 4);
}

CMapWindow::~CMapWindow()
{
}

void CMapWindow::Display()
{
	CGUIWindow::Display();
}