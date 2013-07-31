#include "Repere.h"
#include "IShader.h"

CRepere::CRepere( IRenderer& oRenderer ):
CShape( oRenderer )
{
}

void CRepere::Update()
{
	CShape::Update();
	if( !m_bHidden )
		m_oRenderer.DrawBase( m_oBase, 1000 );
}