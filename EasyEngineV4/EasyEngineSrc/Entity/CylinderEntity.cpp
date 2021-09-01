#include "CylinderEntity.h"
#include "IGeometry.h"

CCylinderEntity::CCylinderEntity(EEInterface& oInterface, float fRadius, float fHeight ):
CEntity(oInterface)
{
	m_pCylinder = m_oGeometryManager.CreateCylinder( CMatrix(), fRadius, fHeight );
	SetScaleFactor( m_pCylinder->GetRadius(), m_pCylinder->GetHeight(), m_pCylinder->GetRadius() );
}

void CCylinderEntity::Update()
{
	CEntity::Update();
}