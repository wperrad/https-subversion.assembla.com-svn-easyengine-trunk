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
};