#include "Cylinder.h"
#include <math.h>
#include "math/matrix.h"
#include "Box.h"
#include "IRenderer.h"

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

void CCylinder::SetTM(const CMatrix& m)
{
	m_oTM = m;
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

bool CCylinder::IsIntersect(const IGeometry& oGeometry) const
{
	const CBox* pBox = dynamic_cast<const CBox*>(&oGeometry);
	if (pBox)
		return IsIntersect(*pBox);
	throw 1;
	return false;
}

bool CCylinder::IsIntersect(const CBox& box) const
{
	CMatrix cylMat, boxMat;
	GetTM(cylMat);
	box.GetTM(boxMat);
	CMatrix cylMatInv;
	cylMat.GetInverse(cylMatInv);
	CMatrix boxMatInCylBase = cylMatInv * boxMat;

	CBox boxTemp;
	boxTemp.Set(box.GetMinPoint(), box.GetDimension());
	boxTemp.SetWorldMatrix(boxMatInCylBase);

	vector<CVector> points;
	boxTemp.GetPoints(points);

	float fMinx = -m_fRadius;
	if(fMinx > CVector::GetMaxx(points))
		return false;
	float fMiny = -m_fHeight / 2.f;
	if (fMiny > CVector::GetMaxy(points))
		return false;
	float fMinz = -m_fRadius;
	if (fMinz > CVector::GetMaxz(points))
		return false;
	float fMaxx = m_fRadius;
	if (fMaxx < CVector::GetMinx(points))
		return false;
	float fMaxy = m_fHeight / 2.f;
	if (fMaxy < CVector::GetMiny(points))
		return false;
	float fMaxz = m_fRadius;
	if (fMaxz < CVector::GetMinz(points))
		return false;

	return true;
	
	//Faire la meme chose en faisant le changement de base dans la base de la boite
	CMatrix boxMatInv;
	boxMat.GetInverse(boxMatInv);
	CMatrix cylMatInBoxBase = boxMatInv * cylMat;
	CVector cylBase = cylMatInBoxBase * CVector(0, 0, 0, 1);
	
	float dx1 = cylBase.m_x - m_fRadius / 2.f;
	float dx2 = box.GetMinPoint().m_x + box.GetDimension().m_x;
	if (dx1 > dx2)
		return false;

	float dy1 = cylBase.m_y - m_fRadius / 2.f;
	float dy2 = box.GetMinPoint().m_y + box.GetDimension().m_y;
	if (dy1 > dy2)
		return false;

	float dz1 = cylBase.m_z - m_fHeight / 2.f;
	float dz2 = box.GetMinPoint().m_z + box.GetDimension().m_z;
	if (dz1 > dz2)
		return false;


	if (cylBase.m_x + m_fRadius < box.GetMinPoint().m_x)
		return false;
	if (cylBase.m_y + m_fRadius < box.GetMinPoint().m_y)
		return false;
	if (cylBase.m_z + m_fHeight / 2.f < box.GetMinPoint().m_z)
		return false;
		
	return true;
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

const CVector& CCylinder::GetBase() const
{
	return m_oTM.GetPosition();
}

IGeometry* CCylinder::Duplicate()
{
	return new CCylinder(*this);
}

void CCylinder::Transform(const CMatrix& tm)
{
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

void CCylinder::Draw(IRenderer& oRenderer) const
{
	throw 1;
}