#include "Sphere.h"

CSphere::CSphere( const CVector& oCenter, float fRadius ) : 
m_oCenter( oCenter ), 
m_fRadius( fRadius )
{
}

void CSphere::Set( CVector& oCenter, float fRadius )
{
	m_oCenter = oCenter;
	m_fRadius = fRadius;
}

float CSphere::GetRadius() const
{
	return m_fRadius;
}

void CSphere::SetRadius( float fRadius )
{
	m_fRadius = fRadius;
}

void CSphere::SetCenter( CVector& oCenter )
{
	m_oCenter = oCenter;
}

const CVector&	CSphere::GetCenter() const
{
	return m_oCenter;
}