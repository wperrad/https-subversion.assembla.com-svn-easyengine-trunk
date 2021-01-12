#ifndef GUIWINDOW_H
#define GUIWINDOW_H


#include <vector>
#include "GUIWidget.h"
#include "IGUIManager.h"


class CGUIWindow  : public CGUIWidget, public IGUIWindow
{
	std::vector< CGUIWidget* >	_vWidget;
	bool						_bVisible;
public:
								CGUIWindow( int nWidth, int nHeight );
	virtual						~CGUIWindow(void);
	void						AddWidget(CGUIWidget* pWidget);
	size_t						GetWidgetCount()const;
	CGUIWidget*					GetWidget(unsigned int nIndex);
	void						SetVisibility(bool bVisible);
	bool						IsVisible();
	void						Clear();
	void						Display();
	void						SetPosition(float fPosX, float fPosY);

};



#endif //GUIWINDOW_H