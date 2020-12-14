#include "Hud.h"




CHud::CHud(const IHud::Desc& oDesc) : IHud(oDesc),
	m_oGUIManager(oDesc.m_oGUIManager)
{

}

void CHud::Update()
{
	for(int i = 0; i < m_vText.size(); i++)
		m_oGUIManager.Print(m_vText[i].m_sText, m_vText[i].m_nPosx, m_vText[i].m_nPosy);
}

void CHud::Print(string text, int x, int y)
{
	m_vText.push_back(CText(text, x, y));
}


extern "C" _declspec(dllexport) IHud* CreateHud(const IHud::Desc& oDesc)
{
	return new CHud(oDesc);
}