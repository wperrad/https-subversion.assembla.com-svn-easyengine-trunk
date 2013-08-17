#include "box.h"

CBox::CBox():
m_bInitialized( false )
{
}

CBox::CBox( CVector& oMinPoint, CVector& oDimension ):
m_oMinPoint( oMinPoint ),
m_oDimension( oDimension ),
m_bInitialized( true )
{
	m_fBoundingSphereRadius = ComputeBoundingSphereRadius();
}

CBox::CBox( const CBox& oBox )
{
	m_oMinPoint = oBox.m_oMinPoint;
	m_oDimension = oBox.m_oDimension;
	m_oWorldMatrix = oBox.m_oWorldMatrix;
}

void CBox::Set( const CVector& oMinPoint, const CVector& oDimension )
{
	m_oMinPoint = oMinPoint;
	m_oDimension = oDimension;
	m_fBoundingSphereRadius = ComputeBoundingSphereRadius();
	m_bInitialized = true;
}

void CBox::GetCenter( CVector& oCenter ) const
{
	oCenter = m_oMinPoint +  m_oDimension / 2.f;
}

void CBox::AddPoint( const CVector& p )
{
	if( !m_bInitialized )
	{
		m_oMinPoint = p;
		m_bInitialized = true;
	}

	CVector vMax = m_oMinPoint + m_oDimension;
	if( vMax.m_x < p.m_x )
		vMax.m_x = p.m_x;
	if( vMax.m_y < p.m_y )
		vMax.m_y = p.m_y;
	if( vMax.m_z < p.m_z )
		vMax.m_z = p.m_z;

	if ( p.m_x < m_oMinPoint.m_x )
		m_oMinPoint.m_x = p.m_x;
	if ( p.m_y < m_oMinPoint.m_y )
		m_oMinPoint.m_y = p.m_y;
	if ( p.m_z < m_oMinPoint.m_z )
		m_oMinPoint.m_z = p.m_z;

	
	m_oDimension = vMax - m_oMinPoint;
	m_fBoundingSphereRadius = ComputeBoundingSphereRadius();
}

float CBox::ComputeBoundingSphereRadius() const
{
	CVector oCenter;
	GetCenter( oCenter );
	float fBoundingSphereRadius = 2 * ( m_oMinPoint - oCenter ).Norm();
	return fBoundingSphereRadius;
}

float CBox::ComputeBoundingCylinderRadius( TAxis eGeneratorAxis ) const
{
	CVector oCenter;
	float fRadius;
	switch( eGeneratorAxis )
	{
	case eAxisX:
		oCenter = m_oMinPoint + CVector( 0.f, m_oDimension.m_y / 2.f, m_oDimension.m_z / 2.f );
		break;
	case eAxisY:
		oCenter = m_oMinPoint + CVector( m_oDimension.m_x / 2.f, 0.f, m_oDimension.m_z / 2.f );
		break;
	case eAxisZ:
		oCenter = m_oMinPoint + CVector( m_oDimension.m_x / 2.f, m_oDimension.m_y / 2.f, 0.f );
		break;
	}
	fRadius = ( oCenter - m_oMinPoint ).Norm();
	return fRadius;
}

float CBox::GetBoundingSphereRadius() const
{
	return m_fBoundingSphereRadius;
}

void CBox::Store( CBinaryFileStorage& store ) const
{
	store << m_oMinPoint << m_oDimension << m_fBoundingSphereRadius;
}

void CBox::Load( CBinaryFileStorage& store )
{
	store >> m_oMinPoint >> m_oDimension >> m_fBoundingSphereRadius;
}

void CBox::Store( CAsciiFileStorage& store ) const
{
	store << "Min point : " << m_oMinPoint << ", dimension : " << m_oDimension << ", radius : " << m_fBoundingSphereRadius << "\n";
}

void CBox::Load( CAsciiFileStorage& store )
{
}

void CBox::Store( CStringStorage& store ) const
{
}

void CBox::Load( CStringStorage& store )
{
}

const CVector& CBox::GetMinPoint() const
{
	return m_oMinPoint;
}
	
void CBox::GetWorldMatrix( CMatrix& m ) const
{
	m = m_oWorldMatrix;
}

const CMatrix& CBox::GetWorldMatrix() const
{
	return m_oWorldMatrix;
}

const CVector& CBox::GetDimension() const
{
	return m_oDimension;
}

IBox& CBox::operator=( const IBox& oBox )
{
	m_bInitialized = true;
	m_fBoundingSphereRadius = oBox.GetBoundingSphereRadius();
	m_oMinPoint = oBox.GetMinPoint();
	oBox.GetWorldMatrix( m_oWorldMatrix );
	m_oDimension = oBox.GetDimension();
	return *this;
}

void CBox::SetWorldMatrix( const CMatrix& oMatrix )
{
	m_oWorldMatrix = oMatrix;
}

void CBox::GetPoints( vector< CVector >& vPoints )
{
	vector< CVector > vTemp;
	vTemp.push_back( m_oMinPoint );
	vTemp.push_back( m_oMinPoint + CVector( m_oDimension.m_x, 0, 0 ) );
	vTemp.push_back( m_oMinPoint + CVector( 0, m_oDimension.m_y, 0 ) );
	vTemp.push_back( m_oMinPoint + CVector( 0, 0, m_oMinPoint.m_z ) );
	vTemp.push_back( m_oMinPoint + CVector( m_oDimension.m_x, m_oDimension.m_y, 0 ) );
	vTemp.push_back( m_oMinPoint + CVector( m_oDimension.m_x, 0, m_oDimension.m_z ) );
	vTemp.push_back( m_oMinPoint + CVector( 0, m_oDimension.m_y, m_oDimension.m_z ) );
	vTemp.push_back( m_oMinPoint + CVector( m_oDimension.m_x, m_oDimension.m_y, m_oDimension.m_z ) );
	for( int i = 0; i < vTemp.size(); i++ )
	{
		CVector v = m_oWorldMatrix * vTemp[ i ];
		vPoints.push_back( v );
	}
}