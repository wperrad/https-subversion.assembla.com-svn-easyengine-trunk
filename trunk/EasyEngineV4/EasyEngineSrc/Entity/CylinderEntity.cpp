#include "CylinderEntity.h"
#include "IGeometry.h"

CCylinderEntity::CCylinderEntity( IGeometryManager& gm, IRenderer& r, IRessourceManager& rm, ICollisionManager& cm, float fRadius, float fHeight ):
CEntity( "Cylinder.bme", rm, r, NULL, gm, cm )
{
	m_pCylinder = gm.CreateCylinder( CVector( 0, 0, 0 ), fRadius, fHeight );
	//IAnimatableMesh* pAMesh = static_cast< IAnimatableMesh* >( rm.GetRessource( "Cylinder.bme", r ) );
	SetScaleFactor( m_pCylinder->GetRadius(), m_pCylinder->GetHeight(), m_pCylinder->GetRadius() );
	//m_pRessource = pAMesh->GetMesh( 0 );
}

void CCylinderEntity::Update()
{
	m_eRenderType = IRenderer::eLine;
	CEntity::Update();
}