#include "Segment.h"


CSegment::CSegment( const CVector& first, const CVector& last ) :
m_oFirst( first ),
m_oLast( last )
{
}


CSegment::~CSegment()
{
}

void CSegment::ComputeProjectedPointOnLine( const CVector& oPointToProject, CVector& oProjectedPoint ) const
{
	float a = m_oFirst.m_z - m_oLast.m_z;
	float b = m_oLast.m_x - m_oFirst.m_x;
	float c1 = m_oFirst.m_x * ( m_oLast.m_z - m_oFirst.m_z ) + m_oFirst.m_z * ( m_oFirst.m_x - m_oLast.m_x );
	float c2 = oPointToProject.m_x * ( m_oFirst.m_x - m_oLast.m_x ) + oPointToProject.m_z * ( m_oFirst.m_z - m_oLast.m_z );

	oProjectedPoint.m_x = -( a * c1 + b * c2 ) / ( a * a + b * b );
	oProjectedPoint.m_y = oPointToProject.m_y;
	oProjectedPoint.m_z = ( a * c2 - b * c1 ) /  ( a * a + b * b );
}

float CSegment::ComputeDistanceToPoint( const CVector& oPoint )
{
	CVector H;
	ComputeProjectedPointOnLine( oPoint, H );
	return ( H - oPoint ).Norm();
}