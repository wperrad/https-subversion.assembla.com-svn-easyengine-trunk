#include "Repere.h"
#include "IShader.h"

CRepere::CRepere( IRenderer& oRenderer ):
CShape( oRenderer )
{
	SetEntityName("Repere");
}

void CRepere::Update()
{
	CShape::Update();
	if( !m_bHidden )
		m_oRenderer.DrawBase( m_oBase, 1000 );
}

void CRepere::SetEntityName(string sName) 
{ 
	m_sEntityName = sName;
}

void CRepere::GetEntityName(string& sName)
{
	sName = m_sEntityName;
}