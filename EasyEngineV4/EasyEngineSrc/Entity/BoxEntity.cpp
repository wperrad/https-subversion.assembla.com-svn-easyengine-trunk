#include "BoxEntity.h"
#include "IGeometry.h"

CBoxEntity::CBoxEntity( IRenderer& oRenderer, IBox& oBox ):
CShape( oRenderer ),
m_oBox( oBox )
{
}

void CBoxEntity::Update()
{
	CShape::Update();
	m_oBox.SetWorldMatrix( m_oWorldMatrix );
	if( !m_bHidden )
		m_oRenderer.DrawBox( m_oBox.GetMinPoint(), m_oBox.GetDimension() );
}

IBox& CBoxEntity::GetBox()
{
	return m_oBox;
}

void CBoxEntity::GetEntityName(string& sName)
{
	sName = m_sEntityName;
}
