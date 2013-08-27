#include "Cylinder.h"
#include <math.h>
#include "math/matrix.h"

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

void CCylinder::ComputeTangent( const CVector& oLinePoint, CVector& oTangentPoint, bool bLeft )
{
	CVector oPointToCenter = m_oBase - oLinePoint;
	oPointToCenter.m_y = 0;
	float fPointToCenter = oPointToCenter.Norm();
	float alpha = asinf( m_fRadius / fPointToCenter ) * 180.f / 3.1415927f;
	if( !bLeft ) alpha = -alpha;
	
	CVector oPointToCenterNorm = oPointToCenter;
	oPointToCenterNorm.Normalize();
	CVector oPointToTangentNorm = CMatrix::GetyRotation( alpha ) * oPointToCenterNorm;
	float fPointToTangentNorm = sqrt( oPointToCenter.Norm() * oPointToCenter.Norm() - m_fRadius * m_fRadius );
	CVector oPointToTangent = oPointToTangentNorm * fPointToTangentNorm;
	oTangentPoint = oLinePoint + oPointToTangent;
}

bool CCylinder::IsPointIntoCylinder( const CVector& oPoint ) const
{
	CVector oPoint2D( oPoint.m_x, 0.f, oPoint.m_z );
	if( ( oPoint2D - m_oBase ).Norm() > m_fRadius )
		return false;
	if( oPoint.m_y < m_oBase.m_y || oPoint.m_y > m_oBase.m_y + m_fHeight )
		return false;
	return true;
}

