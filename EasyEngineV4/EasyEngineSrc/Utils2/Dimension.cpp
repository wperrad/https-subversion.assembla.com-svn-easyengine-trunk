#include "dimension.h"

CDimension::CDimension(void):
_fWidth( 0 ),
_fHeight( 0 )
{
}

CDimension::CDimension(float fWidth, float fHeight)
{
	_fWidth = fWidth;
	_fHeight = fHeight;
}

CDimension::~CDimension(void)
{
}



void CDimension::SetWidth(float nWidth)
{
	_fWidth = nWidth;
}
	
void CDimension::SetHeight(float nHeight)
{
	_fHeight = nHeight;
}

float CDimension::GetWidth()const
{
	return _fWidth;
}


float CDimension::GetHeight()const
{
	return _fHeight;
}
	
void CDimension::SetDimension(float nWidth, float nHeight)
{
	_fWidth = nWidth;
	_fHeight = nHeight;
}



void CDimension::SetDimension(CDimension d)
{
	_fWidth = d.GetWidth();
	_fHeight = d.GetHeight();
}