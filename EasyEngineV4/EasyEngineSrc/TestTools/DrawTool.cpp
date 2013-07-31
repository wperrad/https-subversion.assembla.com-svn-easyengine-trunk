#define DRAWTOOL_CPP

#include "DrawTool.h"
#include "Math/Matrix.h"
#include "IShaderManager.h"
#include "IRenderer.h"
#include "ICameraManager.h"
#include "ICamera.h"
#include "../Utils2/Node.h"
#include "../Ressource2/TestMesh.h"
#include "IRessourceManager.h"

using namespace std;

CDrawTool::CDrawTool( const Desc& oDesc ):
IDrawTool( oDesc ),
m_oTestShaderManager( oDesc ),
m_oRenderer( oDesc.m_oRenderer ),
m_oShaderManager( oDesc.m_oShaderManager ),
m_oCameraManager( oDesc.m_oCameraManager ),
m_oRessourceManager( oDesc.m_oRessourceManager )
{
}

void CDrawTool::DrawLine( const CVector& p1, const CVector& p2 )
{
	m_oShaderManager.SetCurrent( "color" );
	CMatrix oCamMatrix, oCamMatrixInverse;
	m_oCameraManager.GetActiveCamera()->GetLocalMatrix( oCamMatrix );
	oCamMatrix.GetInverse( oCamMatrixInverse );
	m_oRenderer.SetModelViewMatrix( oCamMatrixInverse );
	m_oRenderer.BeginDirectGeometry( IRenderer::T_LINES );
	m_oRenderer.SetColor3f( 1., 1., 1. );
	m_oRenderer.SetVertex3f( p1.m_x, p1.m_y, p1.m_z );
	m_oRenderer.SetVertex3f( p2.m_x, p2.m_y, p2.m_z );
	m_oRenderer.EndDirectGeometry();
}

void CDrawTool::DrawBase( const CMatrix& m, float fAxisSize )
{
	string sCurrentShader;
	m_oShaderManager.GetCurrent( sCurrentShader );
	m_oShaderManager.SetCurrent( "color" );
	CMatrix oCamMatrix, oCamMatrixInverse;
	m_oCameraManager.GetActiveCamera()->GetLocalMatrix( oCamMatrix );
	oCamMatrix.GetInverse( oCamMatrixInverse );

	m_oRenderer.SetModelViewMatrix( oCamMatrixInverse * m );

	m_oRenderer.BeginDirectGeometry( IRenderer::T_LINES );
	m_oRenderer.SetColor3f( 1.f, 0.f, 0.f );
	m_oRenderer.SetVertex3f( 0.f, 0.f, 0.f );
	m_oRenderer.SetVertex3f( fAxisSize, 0.f, 0.f );

	m_oRenderer.SetColor3f( 0.f, 1.f, 0.f );
	m_oRenderer.SetVertex3f( 0.f, 0.f, 0.f );
	m_oRenderer.SetVertex3f( 0.f, fAxisSize, 0.f );

	m_oRenderer.SetColor3f( 0.f, 0.f, 1.f );
	m_oRenderer.SetVertex3f( 0.f, 0.f, 0.f );
	m_oRenderer.SetVertex3f( 0.f, 0.f, fAxisSize );

	m_oRenderer.EndDirectGeometry();
	m_oShaderManager.SetCurrent( sCurrentShader );
}

CNode* CDrawTool::CreateBase( float fAxisSize, const CVector vColor )
{
	CNode* pNode = new CNode;
	m_mBase[ pNode ] = fAxisSize;
	return pNode;
}

void CDrawTool::DrawBase( CNode* pNode )
{
	float fAxisSize = m_mBase[ pNode ];
	CMatrix m;
	pNode->GetLocalMatrix( m );
	DrawBase( m, fAxisSize );
}

CNode* CDrawTool::CreateTestMesh( const string& sFileName )
{
	ICamera* pCamera = m_oCameraManager.GetActiveCamera();
	m_oRenderer.CalculProjectionMatrix( m_oProjectionMatrix, pCamera->GetFov() );
	m_oTestShaderManager.SetProjection( m_oProjectionMatrix );
	ITestMesh* pTestMesh = m_oRessourceManager.GetTestRessource( sFileName, m_oTestShaderManager );
	CNode* pNode = new CNode;
	m_mTestMesh[ pNode ] = pTestMesh;
	return pNode;
}

void CDrawTool::DrawTestMesh( CNode* pNode, IRenderer::TDrawStyle style )
{
	CMatrix oModelMatrix, oCamMatrix, oInvCamMatrix;
	pNode->GetLocalMatrix( oModelMatrix );
	m_oCameraManager.GetActiveCamera()->GetLocalMatrix( oCamMatrix );
	oCamMatrix.GetInverse( oInvCamMatrix );
	CMatrix oModelView = oInvCamMatrix * oModelMatrix;
	m_oTestShaderManager.SetModelView( oModelView );

	ITestMesh* pMesh = m_mTestMesh[ pNode ];
	m_oRessourceManager.DrawTestMesh( pMesh, style, *this );
}

extern "C" _declspec(dllexport) IDrawTool* CreateDrawTool( const IDrawTool::Desc& oDesc )
{
	return new CDrawTool( oDesc );
}