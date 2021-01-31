#include "IHud.h"

using namespace std;

#include <string>

class IGUIManager;

struct CText
{
	string m_sText;
	int m_nPosx;
	int m_nPosy;

	CText(string text, int x, int y) :
		m_sText(text),
		m_nPosx(x),
		m_nPosy(y)
	{}
};

class CHud : public IHud 
{
public:
	CHud(const IHud::Desc& oDesc);
	void Update();
	void Print(string text, int x, int y);

private:
	IGUIManager&	m_oGUIManager;
	vector<CText>	m_vText;
	
};

extern "C" _declspec(dllexport) IHud* CreateHud(const IHud::Desc& oDesc);