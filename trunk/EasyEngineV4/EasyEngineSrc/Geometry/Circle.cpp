#include "Circle.h"
#include <math.h>
#include "Math/Matrix.h"


CCircle::CCircle( const CVector2D& oCenter, float fRadius ):
m_oCenter( oCenter ),
m_fRadius( fRadius )
{
}

void CCircle::Set( const CVector2D& oCenter, float fRadius )
{
	m_oCenter = oCenter;
	m_fRadius = fRadius;
}

bool CCircle::IsSegmentAtLeftSide( const CVector2D& oStartPoint, const CVector2D& oEndPoint ) const
{
	CVector2D oCenterDirection = m_oCenter - oStartPoint;
	oCenterDirection.Normalize();
	CVector2D oSegDirection = oEndPoint - oStartPoint;
	oSegDirection.Normalize();
	CVector v = oCenterDirection ^ oSegDirection;
	return v.m_z > 0.f;
}

bool CCircle::IsPointIntoCircle( const CVector2D& oPoint ) const
{
	return ( ( oPoint - m_oCenter ).Norm() < m_fRadius );
}

void CCircle::ComputeTangent( const CVector2D& oLinePoint, CVector2D& oTangentPoint, bool bLeft ) const
{
	CVector2D oPointToCenter = m_oCenter - oLinePoint;
	float fPointToCenter = oPointToCenter.Norm();
	float alpha = asinf( m_fRadius / fPointToCenter ) * 180.f / 3.1415927f;
	if( !bLeft ) 
		alpha = -alpha;
	
	CVector2D oPointToCenterNorm = oPointToCenter;
	oPointToCenterNorm.Normalize();
	CVector2D oPointToTangentNorm = CMatrix2X2::GetRotation( alpha ) * oPointToCenterNorm;
	float fPointToTangentNorm = sqrt( oPointToCenter.Norm() * oPointToCenter.Norm() - m_fRadius * m_fRadius );
	CVector2D oPointToTangent = oPointToTangentNorm * fPointToTangentNorm;
	oTangentPoint = oLinePoint + oPointToTangent;
}

CVector2D& CCircle::GetCenter()
{
	return m_oCenter;
}