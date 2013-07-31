#ifndef CONTAINER_CPP
#ifndef GUIMANAGER_CPP
#ifndef GUIWIDGET_CPP
#ifndef GUIWINDOW_CPP
#error
#endif
#endif
#endif
#endif

#ifndef CONTAINER_H
#define CONTAINER_H


#include <vector>


class CContainer
{
protected:
	std::vector< CContainer* >			_vChild;
	
	
public:
	CContainer();
	virtual ~CContainer();
};



#endif //CONTAINER_H