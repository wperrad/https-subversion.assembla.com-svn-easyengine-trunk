#include "Cylinder.h"
#include <math.h>
#include "math/matrix.h"

CCylinder::CCylinder() :
	m_fHeight(0),
	m_fRadius(0)
{
}

CCylinder::CCylinder(const CCylinder& cyl)
{
	m_oTM = cyl.GetTM();
	m_fRadius = cyl.GetRadius();
	m_fHeight = cyl.GetHeight();
}

CCylinder::CCylinder( const CMatrix& oTM, float fRadius, float fHeight ):
m_oTM( oTM ),
m_fRadius( fRadius ),
m_fHeight( fHeight )
{
}

float CCylinder::GetRadius() const
{
	return m_fRadius;
}

float CCylinder::GetHeight() const
{
	return m_fHeight;
}

void CCylinder::Set( const CMatrix& oTM, float fRadius, float fHeight )
{
	m_oTM = oTM;
	m_fRadius = fRadius;
	m_fHeight = fHeight;
}

void CCylinder::ComputeTangent( const CVector& oLinePoint, CVector& oTangentPoint, bool bLeft )
{
	CVector oBase = m_oTM.GetPosition();
	CVector oPointToCenter = oBase - oLinePoint;
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
	CVector oBase = m_oTM.GetPosition();
	CVector oPoint2D( oPoint.m_x, 0.f, oPoint.m_z );
	if( ( oPoint2D - oBase).Norm() > m_fRadius )
		return false;
	if( oPoint.m_y < oBase.m_y || oPoint.m_y > oBase.m_y + m_fHeight )
		return false;
	return true;
}

bool CCylinder::IsIntersect(const IBox& pBox)
{
	throw 1;
	return false;
}

const IPersistantObject& CCylinder::operator >> (CBinaryFileStorage& store) const
{
	store << (int)eCylinder << m_oTM << m_fRadius << m_fHeight;
	return *this;
}

IPersistantObject& CCylinder::operator << (CBinaryFileStorage& store)
{
	store >> m_oTM >> m_fRadius >> m_fHeight;
	return *this;
}

const IPersistantObject& CCylinder::operator >> (CAsciiFileStorage& store) const
{
	return *this;
}

IPersistantObject& CCylinder::operator << (CAsciiFileStorage& store)
{
	return *this;
}

const IPersistantObject& CCylinder::operator >> (CStringStorage& store) const
{
	return *this;
}

IPersistantObject& CCylinder::operator << (CStringStorage& store)
{
	return *this;
}

void CCylinder::GetTM(CMatrix& m) const
{
	m = m_oTM;
}

const CMatrix& CCylinder::GetTM() const
{
	return m_oTM;
}

float CCylinder::ComputeBoundingSphereRadius() const
{
	return sqrtf(pow(m_fHeight / 2.f, 2) + pow(m_fRadius, 2));
}

void CCylinder::GetBase(CVector& oBase)
{
	oBase = m_oTM.GetPosition();
}

IGeometry* CCylinder::Duplicate()
{
	return new CCylinder(*this);
}

void CCylinder::Transform(const CMatrix& tm)
{
	float temp = m_fHeight;
	m_fHeight = m_fRadius * 2;
	m_fRadius = temp;
}

float CCylinder::GetHeight()
{
	return m_fHeight;
}

float CCylinder::GetDistance(const IGeometry& oGeometry) const
{
	return -1.f;
}

float CCylinder::GetDistance(const IBox& oBox) const
{
	return -1.f;
}

float CCylinder::GetDistance(const ICylinder& oBox) const
{
	return -1.f;
}