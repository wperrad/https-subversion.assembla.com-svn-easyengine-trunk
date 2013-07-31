#ifndef CONTROL_H
#define CONTROL_H

#include "Widget.h"

class CControl : public CWidget
{
protected:
	void						OnCreate();
public:
	struct Desc : public CWidget::Desc
	{
		Desc( int nWidth, int nHeight, CWidget* pParent );
	};

	CControl( const Desc& desc );
	virtual ~CControl();
	//void			CreateWnd( DWORD dwExStyle, std::string sClassName, std::string sWindowName, DWORD dwStyle, int nPosX, int nPosY, int nWidth, int nHeight, const CMenu2* pMenu );
};

#endif // CONTROL_H