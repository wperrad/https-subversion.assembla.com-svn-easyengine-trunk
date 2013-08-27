#include "IGeometry.h"


class CCircle : public ICircle
{
	CVector2D		m_oCenter;
	float			m_fRadius;

public:

	CCircle( const CVector2D& oCenter, float fRadius );
	void		Set( const CVector2D& oCenter, float fRadius );
	bool		IsSegmentAtLeftSide( const CVector2D& oStartPoint, const CVector2D& oEndPoint ) const;
	bool		IsPointIntoCircle( const CVector2D& oPoint ) const;
	void		ComputeTangent( const CVector2D& oLinePoint, CVector2D& oTangentPoint, bool bLeft ) const;
};