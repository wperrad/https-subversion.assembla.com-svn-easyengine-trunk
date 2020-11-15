#define MATERIAL_CPP

#include "material.h"
#include "Exception.h"
#include "Loader/AseLoader.h"
#include "Texture.h"
#include "IShader.h"

using namespace std;

CMaterial::Desc::Desc( IRenderer& oRenderer, IShader* pShader ):
IRessource::Desc( oRenderer, pShader ),
m_fShininess( 0 ),
m_pTexture( NULL )
{
	for (unsigned int i=0 ; i<4 ; i++)
	{
		m_vAmbient.push_back( 0 );
		m_vSpecular.push_back( 0 );
		m_vDiffuse.push_back( 0 );
		m_vEmissive.push_back( 0 );
	}
}

CMaterial::CMaterial( const Desc& oDesc ) :
IRessource( oDesc ),
m_pTexture(NULL),
m_pShader( oDesc.m_pShader ),
m_bUseAdditionalColor(false)
{
	m_vAmbient = oDesc.m_vAmbient;
	m_vDiffuse = oDesc.m_vDiffuse;
	m_vSpecular = oDesc.m_vSpecular;
	m_vEmissive = oDesc.m_vEmissive;
	m_fShininess = oDesc.m_fShininess;
	m_pTexture = oDesc.m_pTexture;
}

CMaterial::~CMaterial(void)
{
}

void CMaterial::Update()
{
	if ( m_pTexture )
	{
		m_pTexture->SetShader( m_pShader );
		m_pTexture->Update();
	}
	else
	{
		GetRenderer().BindTexture( 0, 0, IRenderer::T_2D );
		m_pShader->SendUniformValues( "ValueTexture0", 0 );
	}	

	if (!m_bUseAdditionalColor) {
		GetRenderer().SetMaterialAmbient(&m_vAmbient[0]);
		GetRenderer().SetMaterialDiffuse(&m_vDiffuse[0]);
	}
	else {
		vector<float> newAmbient, newDiffuse;
		for (int i = 0; i < 3; i++) {
			newAmbient.push_back(m_vAmbient[i] * (1 - m_vAdditionalColor[3]) + m_vAdditionalColor[i] * m_vAdditionalColor[3]);
			newDiffuse.push_back(m_vDiffuse[i] * (1 - m_vAdditionalColor[3]) + m_vAdditionalColor[i] * m_vAdditionalColor[3]);
		}
		GetRenderer().SetMaterialAmbient(&newAmbient[0]);
		GetRenderer().SetMaterialDiffuse(&newDiffuse[0]);
		m_bUseAdditionalColor = false;
	}

	GetRenderer().SetMaterialSpecular(&m_vSpecular[0]);
	GetRenderer().SetMaterialEmissive(&m_vEmissive[0]);
	GetRenderer().SetMaterialShininess(m_fShininess);
}

void CMaterial::GetMaterialMatrix( CMatrix& m )
{
	m.Fill(	m_vAmbient[ 0 ], m_vAmbient[ 1 ], m_vAmbient[ 2 ], m_vAmbient[ 3 ] ,
			m_vDiffuse[ 0 ], m_vDiffuse[ 1 ], m_vDiffuse[ 2 ], m_vDiffuse[ 3 ] ,
			m_vSpecular[ 0 ], m_vSpecular[ 1 ], m_vSpecular[ 2 ], m_vSpecular[ 3 ] ,
			m_vEmissive[ 0 ], m_vEmissive[ 1 ], m_vEmissive[ 2 ], m_vEmissive[ 3 ] );
}

void CMaterial::SetShader( IShader* pShader )
{
	//m_pTexture->SetShader( pShader );
	m_pShader = pShader;
}

void CMaterial::SetAdditionalColor(float r, float g, float b, float a)
{
	m_vAdditionalColor.push_back(r);
	m_vAdditionalColor.push_back(g);
	m_vAdditionalColor.push_back(b);
	m_vAdditionalColor.push_back(a);
	m_bUseAdditionalColor = true;
}