#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Position.h"
#include "Dimension.h"

class  CRectangle
{
	

public:
	CPosition	m_oPos;
	CDimension	m_oDim;

	CRectangle(void);
	CRectangle(float x, float y, float width, float height);
	CRectangle(CPosition pos, CDimension dim);
	virtual ~CRectangle();

	void	SetDimension( const CDimension& dim );
	void	SetPosition( const CPosition& pos );	
	
};



#endif //RECTANGLE_H