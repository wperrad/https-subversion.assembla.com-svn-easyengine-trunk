#ifndef GUIWINDOW_CPP
#ifndef GUIMANAGER_CPP
#error
#endif
#endif

#ifndef GUIWINDOW_H
#define GUIWINDOW_H


#include <vector>
#include "GUIWidget.h"


class CGUIWindow  : public CGUIWidget
{
	std::vector< CGUIWidget* >		_vWidget;
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

};



#endif //GUIWINDOW_H