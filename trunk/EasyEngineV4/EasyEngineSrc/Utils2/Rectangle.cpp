#include "rectangle.h"

CRectangle::CRectangle(void)
{

}


CRectangle::CRectangle(float x, float y, float width, float height)
{
	m_oPos = CPosition(x,y);
	m_oDim = CDimension(width, height);
}


CRectangle::CRectangle(CPosition pos, CDimension dim)
{
	m_oPos = pos;
	m_oDim = dim;
}




CRectangle::~CRectangle(void)
{
}



void CRectangle::SetDimension( const CDimension& dim )
{
	m_oDim.SetDimension( dim.GetWidth(), dim.GetHeight() );
}

void CRectangle::SetPosition( const CPosition& pos )
{
	m_oPos = pos;
}