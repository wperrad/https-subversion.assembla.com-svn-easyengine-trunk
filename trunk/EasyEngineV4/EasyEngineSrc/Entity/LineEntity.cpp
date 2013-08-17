#include "LineEntity.h"

CLineEntity::CLineEntity( IRenderer& oRenderer ):
CShape( oRenderer )
{
}

CLineEntity::CLineEntity( IRenderer& oRenderer, const CVector& first, const CVector& last ):
CShape( oRenderer ),
m_oFirst( first ),
m_oLast( last )
{
}


void CLineEntity::Update()
{
	CShape::Update();
	if( !m_bHidden )
		m_oRenderer.DrawLine( m_oFirst, m_oLast, CVector( 1.f, 1.f, 1.f ) );
}


void CLineEntity::Set( const CVector& first, const CVector& last )
{
	m_oFirst = first;
	m_oLast = last;
}