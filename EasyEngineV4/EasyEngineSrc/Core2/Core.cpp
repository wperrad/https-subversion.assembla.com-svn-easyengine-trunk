// System
#include <windows.h>
#include <gl/gl.h>
#include <time.h>

// stl
#include <sstream>

// engine
#include "Core.h"
#include "IRessource.h"
#include "IRenderer.h"
#include "IGUIManager.h"
#include "IXMLParser.h"
#include "ICameraManager.H"
#include "ICamera.h"
#include "IEntity.h"
#include "IActionManager.h"
#include "IScriptmanager.h"
#include "../Utils2/EasyFile.h"
#include "IWindow.h"
#include "Exception.h"
using namespace std;


CCore::CCore( const ICore::Desc& oCoreDesc ):
ICore( oCoreDesc ),
m_oEventDispatcher( oCoreDesc.m_oEventDispatcher ),
m_oFileSystem( oCoreDesc.m_oFileSystem ),
m_oInputManager( oCoreDesc.m_oInputManager ),
m_oRenderer( oCoreDesc.m_oRenderer ),
m_oRessourceManager( oCoreDesc.m_oRessourceManager ),
m_pGUIManager( oCoreDesc.m_pGUIManager ),
m_oCameraManager( oCoreDesc.m_oCameraManager ),
m_oSceneManager( oCoreDesc.m_oSceneManager ),
m_oActionManager( oCoreDesc.m_oActionManager ),
m_bHUDEnabled( oCoreDesc.m_bHUDEnabled),
m_nFrameCount(0),
m_nLastFps(0),
m_nMsgCount(0),
m_bGUIMode( false ),
m_oWindow( oCoreDesc.m_oWindow )
{
	time( (time_t*) &m_nLastFrameTime );
	//ICamera* pCamera = m_oCameraManager.CreateCamera( ICameraManager::T_FREE_CAMERA, 40.f );
	//m_oCameraManager.SetActiveCamera( pCamera );
}

CCore::~CCore()
{
}

int CCore::CalculFramerate()
{
  	m_nFrameCount ++;
  	time_t nCurrentFrameTime;
  	time( &nCurrentFrameTime );
	int nRet = m_nLastFps;
  	
  	if ( nCurrentFrameTime - m_nLastFrameTime >= 1 )
  	{
  		m_nLastFps = m_nFrameCount;
  		m_nFrameCount = 0;
  		time( &m_nLastFrameTime);
  	}
	return nRet;
}

void CCore::Close()
{
  m_oRenderer.DestroyContext();
}

//void CCore::Print( string sText, int nWidth,int nHeight)
//{	
//	if ( sText == "" )
//		return;
//	PositionnedMessage* pm = new PositionnedMessage;
//	pm->m_sMsg = sText;
//	pm->x = nWidth;
//	pm->y = nHeight;
//	m_vPositionnedMessage[ m_nMsgCount ] = pm;
//	m_nMsgCount++;
//}

//void CCore::Print( string sText )
//{
//	m_vUnPositionnedMessage.push_back( sText );
//}

//void CCore::DisplayNodePosition(CNode* pNode)
//{
//	CVector vPos;
//	pNode->GetWorldPosition( vPos );
//	string sNodeName;
//	pNode->GetName( sNodeName );
//	ostringstream oss;
//	oss << sNodeName  << " : " << vPos.m_x << ", " << vPos.m_y << ", " << vPos.m_z;
//	Print( oss.str() );
//}

//void CCore::DisplayNodeInfos(CNode* pNode)
//{	
//	string sNodeName;
//	pNode->GetName( sNodeName );
//	Print( sNodeName  );
//	CMatrix mat;
//	pNode->GetLocalMatrix( mat );
//	ostringstream ossMat;
//	ossMat << mat.m_00 << "  " << mat.m_01 << "  " << mat.m_02 << "  " << mat.m_03;
//	Print( ossMat.str() );
//	ossMat.str( "" );
//	ossMat << mat.m_10 << "  " << mat.m_11 << "  " << mat.m_12 << "  " << mat.m_13;
//	Print( ossMat.str() );
//	ossMat.str( "" );
//	ossMat << mat.m_20 << "  " << mat.m_21 << "  " << mat.m_22 << "  " << mat.m_23;
//	Print( ossMat.str() );
//	ossMat.str( "" );
//	ossMat << mat.m_30 << "  " << mat.m_31 << "  " << mat.m_32 << "  " << mat.m_33;
//	Print( ossMat.str() );
//	Print(" ");
//}


extern "C" _declspec(dllexport) ICore* CreateCore( const ICore::Desc& oDesc )
{
	return new CCore( oDesc );
}