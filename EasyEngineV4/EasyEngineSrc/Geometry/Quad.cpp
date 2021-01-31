#include "Quad.h"
#include "Segment.h"
#include "IRenderer.h"

CQuad::CQuad(float fLenght, float fWidth) :
	m_fLenght(fLenght),
	m_fWidth(fWidth)
{

}

void CQuad::GetDimension(float& lenght, float& width)
{
	m_fLenght = lenght;
	m_fWidth = width;
}

bool CQuad::IsIntersect(const IGeometry& oGeometry) const
{
	/*CSegment* segment = dynamic_cast<CSegment*>(&oGeometry);
	if (segment) {

	}*/
	throw 1;
	return false;
}

void CQuad::SetTM(const CMatrix& oTM)
{
	m_oTM = oTM;
}

void CQuad::GetTM(CMatrix& oTM) const
{
	oTM = m_oTM;
}

const CMatrix& CQuad::GetTM() const
{
	return m_oTM;
}

const CVector& CQuad::GetBase() const
{
	return m_oTM.GetPosition();
}

float CQuad::ComputeBoundingSphereRadius() const
{
	return m_fLenght;
}

IGeometry* CQuad::Duplicate()
{
	return NULL;
}

float CQuad::GetHeight() const
{
	return 0.f;
}

void CQuad::Transform(const CMatrix& oTM)
{
	m_oTM = oTM;
}

float CQuad::GetDistance(const IGeometry& oGeometry) const
{
	return 0.f;
}

void CQuad::Draw(IRenderer& oRenderer) const
{
	oRenderer.DrawQuad(m_fLenght, m_fWidth);
}

IGeometry::TFace CQuad::GetReactionYAlignedPlane(const CVector& firstPoint, const CVector& lastPoint, float planeHeight, CVector& R)
{
	return eNone;
}

IGeometry::TFace CQuad::GetReactionYAlignedBox(IGeometry& firstPositionBox, IGeometry& lastPositionBox, CVector& R)
{
	return eNone;
}


const IPersistantObject& CQuad::operator >> (CBinaryFileStorage& store) const
{
	store << eQuad << m_fLenght << m_fWidth;
	return *this;
}

IPersistantObject& CQuad::operator << (CBinaryFileStorage& store)
{
	store >> m_fLenght >> m_fWidth;
	return *this;
}

const IPersistantObject& CQuad::operator >> (CAsciiFileStorage& store) const
{
	store << "Type = Quad, lenght = " << m_fLenght << ", width = " << m_fWidth;
	return *this;
}

IPersistantObject& CQuad::operator << (CAsciiFileStorage& store)
{
	//store >> "Type = Quad, lenght = " << m_fLenght << ", width = " << m_fWidth;
	throw 1;
	return *this;
}

const IPersistantObject& CQuad::operator >> (CStringStorage& store) const
{
	store << "Type = Quad, lenght = " << m_fLenght << ", width = " << m_fWidth;
	return *this;
}

IPersistantObject& CQuad::operator << (CStringStorage& store)
{
	throw 1;
	return *this;
}