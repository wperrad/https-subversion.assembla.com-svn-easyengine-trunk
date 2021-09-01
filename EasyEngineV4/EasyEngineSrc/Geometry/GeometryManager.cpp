#include "GeometryManager.h"
#include "WeightTable.h"
#include "Box.h"
#include "Sphere.h"
#include "Segment.h"
#include "Cylinder.h"
#include "Circle.h"
#include "Quad.h"

CGeometryManager::CGeometryManager( CPlugin::Desc& oDesc ) : IGeometryManager()
{
}

IWeightTable* CGeometryManager::CreateWeightTable() const
{
	return new CWeightTable;
}

IBox* CGeometryManager::CreateBox()
{
	IBox* pBox = new CBox;
	m_mBox[ (int)m_mBox.size() ] = pBox;
	return pBox;
}

IBox* CGeometryManager::CreateBox( const IBox& oBox )
{
	const CBox& oCBox = static_cast< const CBox& >( oBox );
	IBox* pBox = new CBox( oCBox );
	m_mBox[ (int)m_mBox.size() ] = pBox;
	return pBox;
}

int CGeometryManager::GetLastCreateBoxID()
{
	return (int)(m_mBox.size() - 1);
}

IBox* CGeometryManager::GetBox( int nID ) const
{
	map< int, IBox* >::const_iterator itBox = m_mBox.find( nID );
	if( itBox != m_mBox.end() )
		return itBox->second;
	return NULL;
}

ISphere* CGeometryManager::CreateSphere()
{
	ISphere* pSphere = new CSphere( CVector(), 0.f );
	m_mSphere[ (int)m_mSphere.size() ] = pSphere;
	return pSphere;
}

ISphere* CGeometryManager::CreateSphere( CVector& oCenter, float fRadius )
{
	ISphere* pSphere = new CSphere( oCenter, fRadius );
	m_mSphere[ (int)m_mSphere.size() ] = pSphere;
	return pSphere;
}

ISegment* CGeometryManager::CreateSegment( const CVector& first, const CVector& last )
{
	return new CSegment( first, last );
}

ICylinder* CGeometryManager::CreateCylinder()
{
	return new CCylinder;
}

ICylinder* CGeometryManager::CreateCylinder( const CMatrix& oTM, float fRadius, float fHeight )
{
	return new CCylinder(oTM, fRadius, fHeight );
}

ICircle* CGeometryManager::CreateCircle( const CVector2D& oCenter, float fRadius )
{
	return new CCircle( oCenter, fRadius );
}

ISegment2D*	CGeometryManager::CreateSegment2D( const CVector2D& first, const CVector2D& last )
{
	return new CSegment2D( first, last );
}

IQuad* CGeometryManager::CreateQuad(float lenght, float width)
{
	return new CQuad(lenght, width);
}

string CGeometryManager::GetName()
{
	return "GeometryManager";
}

extern "C" _declspec(dllexport) IGeometryManager* CreateGeometryManager( IGeometryManager::Desc& oDesc )
{
	return new CGeometryManager( oDesc );
}