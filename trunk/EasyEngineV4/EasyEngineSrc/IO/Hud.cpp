#include "Interface.h"
#include "EEPlugin.h"
#include "Hud.h"
#include "IGUIManager.h"



CHud::CHud(EEInterface& oInterface) 
	: m_oGUIManager(*static_cast<IGUIManager*>(oInterface.GetPlugin("GUIManager")))
{
	IEventDispatcher* pEventDispatcher = (IEventDispatcher*) oInterface.GetPlugin("EventDispatcher");
	pEventDispatcher->AbonneToWindowEvent(this, WindowCallback);
}

void CHud::WindowCallback(CPlugin* plugin, IEventDispatcher::TWindowEvent e, int, int)
{
	if (e == IEventDispatcher::T_WINDOWUPDATE) {
		CHud* pHud = (CHud*)plugin;
		pHud->Update();
	}
}

void CHud::Update()
{
	for(int i = 0; i < m_vText.size(); i++)
		m_oGUIManager.Print(m_vText[i].m_sText, m_vText[i].m_nPosx, m_vText[i].m_nPosy);
}

int CHud::Print(string text, int x, int y)
{
	m_vText.push_back(CText(text, x, y));
	return m_vText.size() - 1;
}

void CHud::RemoveText(int index)
{
	if (index < m_vText.size()) {
		vector<CText>::iterator it = m_vText.begin() + index;
		m_vText.erase(it);
	}
}

void CHud::Clear()
{
	m_vText.clear();
}

int CHud::GetLineCount()
{
	return m_vText.size();
}

string CHud::GetName()
{
	return "HUD";
}

extern "C" _declspec(dllexport) IHud* CreateHud(EEInterface& oInterface)
{
	return new CHud(oInterface);
}