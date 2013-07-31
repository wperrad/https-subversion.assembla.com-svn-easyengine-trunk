#include "RenderUtils.h"
#include "IRenderer.h"
#include "IShader.h"
#include "ILoader.h"
#include "IEntity.h"
#include "IGUIManager.h"

void CRenderUtils::DrawBox( const CVector& oMinPoint, const CVector& oDimension, IRenderer& oRenderer )
{
	IShader* pShader = oRenderer.GetShader( "color" );
	pShader->Enable( true );
	oRenderer.DrawBox( oMinPoint, oDimension );
}

void CRenderUtils::ScreenCapture( string sFileName, IRenderer* pRenderer, ILoaderManager* pLoaderManager, IEntity* pScene, IGUIManager* pGUIManager )
{
	vector< unsigned char > vPixels;

	pRenderer->BeginRender();
	pScene->Update();
	if( pGUIManager )
		pGUIManager->OnRender();
	int w, h;
	pRenderer->GetResolution( w, h );
	pRenderer->ReadPixels( 0, 0, w, h, vPixels, IRenderer::T_BGR );
	pRenderer->EndRender();

	ILoader::CTextureInfos ti;
	ti.m_ePixelFormat = ILoader::eRGB;
	ti.m_vTexels.swap( vPixels );
	pRenderer->GetResolution( ti.m_nWidth , ti.m_nHeight );
	if( sFileName.find( ".bmp" ) == -1 )
		sFileName += ".bmp";
	ti.m_sFileName = sFileName;
	pLoaderManager->Export( sFileName, ti );
}