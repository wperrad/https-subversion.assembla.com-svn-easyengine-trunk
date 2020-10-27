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
	//float a = m_oFirst.m_z - m_oLast.m_z;
	//float b = m_oLast.m_x - m_oFirst.m_x;
	//float c1 = m_oFirst.m_x * ( m_oLast.m_z - m_oFirst.m_z ) + m_oFirst.m_z * ( m_oFirst.m_x - m_oLast.m_x );
	float a, b, c1;
	Compute2DLineEquation( a, b, c1 );
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

void CSegment::Compute2DLineEquation( float& a, float& b, float& c ) const
{
	a = m_oFirst.m_z - m_oLast.m_z;
	b = m_oLast.m_x - m_oFirst.m_x;
	c = m_oFirst.m_x * ( m_oLast.m_z - m_oFirst.m_z ) - m_oFirst.m_z * ( m_oLast.m_x - m_oFirst.m_x );
}

void CSegment::GetPoints( CVector& p1, CVector& p2 ) const
{
	p1 = m_oFirst;
	p2 = m_oLast;
}

CSegment2D::CSegment2D( const CVector2D& first, const CVector2D& last ):
m_oFirst( first ),
m_oLast( last )
{
}

void CSegment2D::ComputeProjectedPointOnLine( const CVector2D& oPointToProject, CVector2D& oProjectedPoint ) const
{
	float a, b, c1;
	ComputeLineEquation( a, b, c1 );
	float c2 = oPointToProject.m_x * ( m_oFirst.m_x - m_oLast.m_x ) + oPointToProject.m_y * ( m_oFirst.m_y - m_oLast.m_y );

	oProjectedPoint.m_x = -( a * c1 + b * c2 ) / ( a * a + b * b );
	oProjectedPoint.m_y = ( a * c2 - b * c1 ) /  ( a * a + b * b );
}

float CSegment2D::ComputeDistanceToPoint( const CVector2D& oPoint )
{
	CVector2D H;
	ComputeProjectedPointOnLine( oPoint, H );
	return ( H - oPoint ).Norm();
}

void CSegment2D::ComputeLineEquation( float& a, float& b, float& c ) const
{
	a = m_oFirst.m_y - m_oLast.m_y;
	b = m_oLast.m_x - m_oFirst.m_x;
	c = m_oFirst.m_x * ( m_oLast.m_y - m_oFirst.m_y ) - m_oFirst.m_y * ( m_oLast.m_x - m_oFirst.m_x );
}

void CSegment2D::GetPoints( CVector2D& p1, CVector2D& p2 ) const
{
	p1 = m_oFirst;
	p2 = m_oLast;
}