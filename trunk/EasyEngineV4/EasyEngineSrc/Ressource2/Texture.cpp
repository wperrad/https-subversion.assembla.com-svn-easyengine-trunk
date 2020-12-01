#define TEXTURE_CPP

#include "texture.h"
#include "Exception.h"
#include "IRenderer.h"
#include "IShader.h"

using namespace std;


CTextureBase::CDesc::CDesc( IRenderer& oRenderer, IShader* pShader, int nUnitTexture ):
IRessource::Desc( oRenderer, pShader ),
m_nUnitTexture( nUnitTexture ),
m_pData( NULL )
{
}

CTextureBase::CTextureBase( const CDesc& oDesc ):
ITexture( oDesc ),
m_nUnitTexture( oDesc.m_nUnitTexture ),
m_nID( -1 ),
m_pShader( oDesc.m_pShader )
{
}

void CTextureBase::SetShader( IShader* pShader )
{
	m_pShader = pShader;
}


CTexture1D::CDesc::CDesc( IRenderer& oRenderer, IShader* pShader, int nUnitTexture ) :
CTextureBase::CDesc( oRenderer, pShader, nUnitTexture ),
m_nSize( 0 )
{
}

CTexture1D::CTexture1D( CDesc& oDesc ):
CTextureBase( oDesc ),
m_nSize( oDesc.m_nSize ),
m_bFirstUpdate( true )
{
	m_nID = GetRenderer().CreateTexture1D( oDesc.m_pData, oDesc.m_nSize, IRenderer::T_RGBA );
	//delete oDesc.m_pData;
}

void CTexture1D::GetDimension( int& nWidth, int& nHeight )
{
	nWidth = m_nSize;
	nHeight = 1;
}
int nCounter = 0;
float fValue = 0.f;
void CTexture1D::Update()
{
	GetRenderer().BindTexture( m_nID, m_nUnitTexture, IRenderer::T_1D );
	try
	{
		m_pShader->SendUniformValues( "MaterialMap", m_nUnitTexture );
	}
	catch(exception& e)
	{
		if( m_bFirstUpdate )
			MessageBox(NULL, e.what(), "void CTexture1D::Update()", MB_ICONEXCLAMATION);
	}
	//m_pShader->SendUniformValues( "ValueTextureMaterial", m_nID );	
	try
	{
		m_pShader->SendUniformValues( "ArraySize", (float)m_nSize );
	}
	catch( exception& e )
	{
		if ( m_bFirstUpdate )
			MessageBox(NULL, e.what(), "void CTexture1D::Update()", MB_ICONEXCLAMATION);
	}
	if ( m_bFirstUpdate )
		m_bFirstUpdate = false;

}

IShader* CTexture1D::GetCurrentShader() const
{
	return m_pShader;
}

CTexture2D::CDesc::CDesc( IRenderer& oRenderer, IShader* pShader, int nUnitTexture ):
CTextureBase::CDesc( oRenderer, pShader, m_nUnitTexture ),
m_nWidth( 0 ),
m_nHeight( 0 )
{
}

CTexture2D::CTexture2D(  CDesc& oDesc ):
CTextureBase( oDesc ),
m_nReponse( -1 )
{
	if ( oDesc.m_vTexels.size() == 0 )
	{
		CRessourceException e( "" );
		throw e;
	}
	m_nWidth = oDesc.m_nWidth;
	m_nHeight = oDesc.m_nHeight;
	//m_nID = GetRenderer().CreateTexture2D( oDesc.m_vTexels, m_nWidth, m_nHeight, oDesc.m_eFormat );
	m_nID = GetRenderer().CreateMipmaps2D(oDesc.m_vTexels, m_nWidth, m_nHeight, oDesc.m_eFormat);
}

IShader* CTexture2D::GetCurrentShader() const
{
	return m_pShader;
}

void CTexture2D::Update()
{
	GetRenderer().BindTexture( m_nID, m_nUnitTexture, IRenderer::T_2D );
	m_pShader->SendUniformValues( "baseMap", m_nUnitTexture );
	try
	{
		m_pShader->SendUniformValues( "ValueTexture0", m_nID );
	}
	catch( exception& e )
	{
		if( m_nReponse != 6 )
		{
			string sMessage = e.what();
			sMessage += "\nVoulez vous continuer et ignorer tous les avertissements de ce type ?";
			m_nReponse = MessageBox( NULL, sMessage.c_str(), "", MB_YESNO );
		}
	}
}

void CTexture2D::GetDimension( int& nWidth, int& nHeight )
{
	nWidth = m_nWidth;
	nHeight = m_nHeight;
}