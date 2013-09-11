#include "IGeometry.h"

class CSegment : public ISegment
{
	CVector	m_oFirst;
	CVector	m_oLast;
public:
	
	CSegment( const CVector& first, const CVector& last );
	~CSegment();

	void	ComputeProjectedPointOnLine( const CVector& oPointToProject, CVector& oProjectedPoint ) const;
	float	ComputeDistanceToPoint( const CVector& oPoint );
	void	Compute2DLineEquation( float& a, float& b, float& c ) const;
	void	GetPoints( CVector& p1, CVector& p2 ) const;
};

class CSegment2D : public ISegment2D
{
	CVector2D	m_oFirst;
	CVector2D	m_oLast;
public:

	CSegment2D( const CVector2D& first, const CVector2D& last );
	void	ComputeProjectedPointOnLine( const CVector2D& oPointToProject, CVector2D& oProjectedPoint ) const;
	float	ComputeDistanceToPoint( const CVector2D& oPoint );
	void	ComputeLineEquation( float& a, float& b, float& c ) const;
	void	GetPoints( CVector2D& p1, CVector2D& p2 ) const;
};