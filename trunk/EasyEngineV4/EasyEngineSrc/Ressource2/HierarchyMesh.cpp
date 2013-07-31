#define HIERARCHYMESH_CPP
#include "hierarchymesh.h"

CHierarchyMesh::Desc::Desc( IRenderer& oRenderer, IShader* pShader ):
IRessource::Desc( oRenderer, pShader ),
pRootNode( NULL )
{
}

CHierarchyMesh::CHierarchyMesh( const Desc& oDesc ):
IRessource( oDesc ),
m_pRoot( oDesc.pRootNode )
{
}

CHierarchyMesh::~CHierarchyMesh(void)
{
}

void CHierarchyMesh::Update()
{
}