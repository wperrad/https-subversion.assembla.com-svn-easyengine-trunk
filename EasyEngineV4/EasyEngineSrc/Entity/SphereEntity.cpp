#include "SphereEntity.h"


CSphereEntity::CSphereEntity( IRenderer& oRenderer, ISphere& oSphere ):
CShape( oRenderer ),
m_oSphere( oSphere )
{
}

void CSphereEntity::Update()
{
	CShape::Update();
	if( !m_bHidden )
	{
		m_oRenderer.DrawSphere( m_oSphere.GetRadius(), 10, 10 );
	}
}

ISphere& CSphereEntity::GetSphere()
{
	return m_oSphere;
}