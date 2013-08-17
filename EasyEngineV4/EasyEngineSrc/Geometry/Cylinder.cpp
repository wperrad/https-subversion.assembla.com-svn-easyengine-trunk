#include "Cylinder.h"

CCylinder::CCylinder( const CVector& oBase, float fRadius, float fHeight ):
m_oBase( oBase ),
m_fRadius( fRadius ),
m_fHeight( fHeight )
{
}

CVector& CCylinder::GetBase()
{
	return m_oBase;
}

float CCylinder::GetRadius()
{
	return m_fRadius;
}

float CCylinder::GetHeight()
{
	return m_fHeight;
}

void CCylinder::Set( const CVector& oBase, float fRadius, float fHeight )
{
	m_oBase = oBase;
	m_fRadius = fRadius;
	m_fHeight = fHeight;
}