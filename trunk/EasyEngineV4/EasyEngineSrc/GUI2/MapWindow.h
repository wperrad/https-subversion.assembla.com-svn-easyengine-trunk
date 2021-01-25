#pragma once
#include "GUIWindow.h"

class CGUIManager;
class ITexture;

class CMapWindow :	public CGUIWindow
{
public:
	CMapWindow(IGUIManager* pGUIManager, IScene& oScene, IRessourceManager& oRessourceManager, IRenderer& oRenderer, int nWidth, int nHeight);
	~CMapWindow();

	void				Display();

protected:
	
private:
	CGUIManager*		m_pGUIManager;
	IScene&				m_oScene;
	IRenderer&			m_oRenderer;
	CGUIWidget			m_oMap;
};

