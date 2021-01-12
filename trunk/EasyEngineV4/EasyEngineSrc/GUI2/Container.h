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