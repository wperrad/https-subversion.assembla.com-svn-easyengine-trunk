#include <windows.h>

// stl
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

// Engine 
#include "../Utils2/Node.h"
#include "../Utils2/DebugTool.h"
#include "ICore.h"
#include "IInputManager.h"
#include "IGUIManager.h"
#include "IRenderer.h"
#include "IFileSystem.h"
#include "IWindow.h"
#include "IWidget.h"
#include "ITestMesh.h"
#include "IRessource.h"
#include "ILoader.h"
#include "IXMLParser.h"
#include "IDrawTool.h"
#include "ICameraManager.h"
#include "IActionManager.h"
#include "IConsole.h"
#include "IScriptManager.h"
#include "IEntity.h"
#include "../Utils2/Chunk.h"
#include "ICamera.h"
#include "Exception.h"
#include "ScriptRegistration.h"
#include "ICollisionManager.h"
#include "ISystems.h"
#include "IGeometry.h"

#define CATCH_EXCEPTION

using namespace std;

struct CGFXOption
{
	bool m_bFullscreen;
	CGFXOption() : m_bFullscreen( true ){}
};

void  OnWindowEvent( CPlugin* pPlugin, IEventDispatcher::TWindowEvent e, int nWidth, int nHeight );
void UpdateCamera();
void GetOptionsByCommandLine( string sCommandArguments, CGFXOption& oOption );

ICore*					m_pCore = NULL;
IDrawTool*				m_pDrawTool = NULL;
IWindow*				m_pWindow = NULL;
ILoaderManager*			m_pLoaderManager = NULL;
IRessourceManager*		m_pRessourceManager = NULL;
IRenderer*				m_pRenderer = NULL;
IRenderer*				m_pSoftRenderer = NULL;
ICameraManager* 		m_pCameraManager = NULL;
IEntityManager* 		m_pEntityManager = NULL;
IInputManager*			m_pInputManager = NULL;
IScriptManager* 		m_pScriptManager = NULL;
IGUIManager*			m_pGUIManager = NULL;
IFileSystem*			m_pFileSystem = NULL;
IConsole*				m_pConsole = NULL;
ISceneManager*			m_pSceneManager  = NULL;
IActionManager*			m_pActionManager = NULL;
ICollisionManager*		m_pCollisionManager= NULL;
ISystemsManager*		m_pSystemsManager = NULL;
IEventDispatcher*		m_pEventDispatcher = NULL;
IXMLParser*				m_pXMLParser = NULL;
IGeometryManager*		m_pGeometryManager = NULL;

IEntity* m_pRepere = NULL;

string s_strMessage = "none";
vector< CNode* > vRotateEntities;
vector< CNode* > vMarkedEntities;
CNode* m_pBase;
CNode* m_pTestMesh;
vector< IEntity* > m_vAnimatedEntities;

IEntity* LoadAndLinkAnimatedentity( string sFileName, string sAnimationFile, IEntity* pScene, IRenderer& oRenderer, float fzpos, float fRoll = 0.f, bool bAnimate = true );
IEntity* LoadAndLinkEntity( string sFileName, IEntity* pScene, IRenderer& oRenderer, float fzpos, float fRoll = 0.f );

vector< IEntity* > m_vLight;
bool	m_bFirstTimeOpenFile = true;
CDebugTool* m_pDebugTool = NULL;
CNode* m_pEntity = NULL;
IEntity* m_pScene = NULL; 

float m_nDeltaTickCount = 0;
float m_nLastTickCount = 0;
bool  m_bGUIMode = false;
int m_nLastGameMousePosx, m_nLastGameMousePosy;
CMatrix ident;
bool m_bRenderScene = true;
bool bCapture = false;

//void TestScript()
//{
//	char pBuffer[ 256 ];
//	FILE* pFile = m_pFileSystem->OpenFile( "script.txt", "r" );
//	if( !pFile )
//		MessageBox( NULL, "Fichier \"script.txt\" introuvable", "", MB_ICONERROR );
//	else
//	{
//		unsigned int nSize = (unsigned int)fread( pBuffer, 1, 256, pFile );
//		pBuffer[ nSize ] = 0;
//		fclose( pFile );
//		m_pScriptManager->ExecuteCommand( pBuffer );
//	}
//}

void SetGUIMode( bool bGUI )
{
	if ( bGUI )
	{
		m_pInputManager->SetMouseCursorPos(0,0);
		m_bGUIMode = true;
	}
	else
	{
		m_pActionManager->SetGameMousePos( m_nLastGameMousePosx, m_nLastGameMousePosy );
		m_bGUIMode = false;
	}
	m_pCameraManager->GetActiveCamera()->Freeze( bGUI );
	ShowCursor( bGUI );
}

void InitScene( ISceneManager* pSceneManager )
{
	IEntity* pScene = pSceneManager->CreateScene( "Game", "", *m_pGeometryManager );
	m_pScene = pScene;
	
	m_pRepere = m_pEntityManager->CreateRepere( *m_pRenderer );
	m_pRepere->Link( pScene );
	try
	{
		FILE* pFile = m_pFileSystem->OpenFile( "start.eas", "r" );
		m_pConsole->Open( true );
		if( pFile )
		{
			fseek( pFile, 0, SEEK_END );
			long pos = ftell( pFile );
			fclose( pFile );
			if( pos > 0 )
				m_pScriptManager->ExecuteCommand( "run(\"start\");" );
		}
		else
			m_pConsole->Println( "Fichier start introuvable." );
	}
	catch( CEException& e )
	{
		MessageBoxA( NULL, e.what(), "", MB_ICONERROR );
	}
}

void UpdateCamera()
{
	IInputManager::KEY_STATE eStateZ = m_pActionManager->GetKeyActionState( "Avancer");
	IInputManager::KEY_STATE eStateS = m_pActionManager->GetKeyActionState( "Reculer");
	IInputManager::KEY_STATE eStateD = m_pActionManager->GetKeyActionState( "StrafeRight");	
	IInputManager::KEY_STATE eStateQ = m_pActionManager->GetKeyActionState( "StrafeLeft");
	IInputManager::KEY_STATE eMoreSpeed = m_pActionManager->GetKeyActionState( "MoreSpeed" );
	IInputManager::KEY_STATE eLessSpeed = m_pActionManager->GetKeyActionState( "LessSpeed" );

	m_pActionManager->ForceActionState( "MoreSpeed", IInputManager::RELEASED );
	m_pActionManager->ForceActionState( "LessSpeed", IInputManager::RELEASED );

	int avance = ( eStateZ == IInputManager::PRESSED || eStateZ == IInputManager::JUST_PRESSED ) - ( eStateS == IInputManager::PRESSED || eStateS == IInputManager::JUST_PRESSED);
	int gauche = ( eStateQ == IInputManager::JUST_PRESSED || eStateQ == IInputManager::PRESSED ) - ( eStateD == IInputManager::JUST_PRESSED || eStateD == IInputManager::PRESSED );
	avance *= 10;
	gauche *= 10;

	if ( eLessSpeed == IInputManager::JUST_PRESSED )
		m_pCameraManager->GetActiveCamera()->SetSpeed( m_pCameraManager->GetActiveCamera()->GetSpeed() / 2.f );
	if ( eMoreSpeed == IInputManager::JUST_PRESSED )
		m_pCameraManager->GetActiveCamera()->SetSpeed( m_pCameraManager->GetActiveCamera()->GetSpeed() * 2.f );
	m_pActionManager->GetGameMousePos( m_nLastGameMousePosx, m_nLastGameMousePosy );

	ICamera* pActiveCamera = m_pCameraManager->GetActiveCamera();
	if( m_pCameraManager->GetCameraType( pActiveCamera ) == ICameraManager::T_FREE_CAMERA )
	{
		int x, y;
		m_pInputManager->GetOffsetMouse( x, y );
		float s = m_pInputManager->GetMouseSensitivity(); // 1/20
		m_pCameraManager->GetActiveCamera()->Move( -x * s, -y * s, 0.f, (float)avance, (float)gauche, 0.f );
	}
}

void OnKeyAction( CPlugin* pPlugin, unsigned int key, IInputManager::KEY_STATE state )
{
	switch( state )
	{
	case IInputManager::JUST_PRESSED:
		if(key == VK_TAB)
			SetGUIMode(!m_bGUIMode);
		else if(key == m_pConsole->GetConsoleShortCut())
			m_pConsole->Open(!m_pConsole->IsOpen());
	}
}

void UpdatePerso()
{
	if( !m_pConsole->IsOpen() )
	{
		IEntity* pPerso = m_pEntityManager->GetPerso();
		if( pPerso )
		{
			IInputManager::KEY_STATE eStateWalk = m_pActionManager->GetKeyActionState( "AvancerPerso");
			if( eStateWalk == IInputManager::JUST_PRESSED || eStateWalk == IInputManager::PRESSED )
			{
				pPerso->RunAction( "run", true );
				m_pActionManager->ForceActionState( "AvancerPerso", IInputManager::PRESSED );
			}
			else if( eStateWalk == IInputManager::JUST_RELEASED )
			{
				pPerso->RunAction( "stand", true );
				m_pActionManager->ForceActionState( "AvancerPerso", IInputManager::RELEASED );
			}
			IInputManager::TMouseButtonState eStatePiedG = m_pActionManager->GetMouseActionState( "HitLeftFoot");
			if( eStatePiedG == IInputManager::eMouseButtonStateJustDown )
			{
				pPerso->RunAction( "HitLeftFoot", false );
			}
			if( m_pCameraManager->GetCameraType( m_pCameraManager->GetActiveCamera() ) == ICameraManager::T_LINKED_CAMERA )
			{
				int x, y;
				m_pInputManager->GetOffsetMouse( x, y );
				float s = m_pInputManager->GetMouseSensitivity();
				pPerso->Roll( - x * s );
			}
		}
	}
}

void OnUpdateWindow()
{
	for ( unsigned int i = 0; i < vRotateEntities.size(); i++ )
		vRotateEntities[ i ]->Yaw( 0.2f );

	int nTickCount = GetTickCount();
	m_nDeltaTickCount = nTickCount - m_nLastTickCount;
	m_nLastTickCount = (float)nTickCount;
	m_pInputManager->OnUpdate();
	//m_pGUIManager->Print( "Salut les connards !", 800, 0 );
	if( !m_pConsole->IsOpen() )
		UpdateCamera();
	UpdatePerso();
	m_pRenderer->BeginRender();
	if( m_bRenderScene )
		m_pSceneManager->GetScene( "Game" )->Update();
	m_pGUIManager->OnRender();
	m_pConsole->Update();
	if( bCapture )
	{
		vector< unsigned char > vPixels;
		int w, h;
		m_pRenderer->GetResolution( w, h );
		m_pRenderer->ReadPixels( 0, 0, w, h, vPixels, IRenderer::T_BGR );
		ILoader::CTextureInfos ti;
		ti.m_ePixelFormat = ILoader::eBGR;
		ti.m_nWidth = 1280;
		ti.m_nHeight = 1024;
		ti.m_vTexels.swap( vPixels );
		m_pLoaderManager->Export( "test_HM.bmp", ti );
		bCapture = false;
	}
  	m_pRenderer->EndRender();
}

void OnRender( IRenderer* pRenderer )
{

}

void InitPlugins( string sCmdLine )
{
	string sDirectoryName;
#ifdef _DEBUG
		sDirectoryName = "..\\..\\EasyEngine\\Debug\\";
#else
		sDirectoryName = "..\\..\\EasyEngine\\release\\";
#endif

	CGFXOption oOption;
	GetOptionsByCommandLine( sCmdLine, oOption );
		
	IFileSystem::Desc oFileSystemDesc( NULL, "FileSystem" );
	m_pFileSystem = static_cast< IFileSystem* > ( CPlugin::Create( oFileSystemDesc, sDirectoryName + "FileUtils.dll", "CreateFileSystem" ) );
	m_pFileSystem->Mount( "..\\data" );
	m_pFileSystem->Mount( "..\\..\\EasyEngine\\data" );

	IEventDispatcher::Desc oEventDispatcherDesc( NULL, "Event dispatcher " );
	m_pEventDispatcher = static_cast< IEventDispatcher* >( CPlugin::Create( oEventDispatcherDesc, sDirectoryName + "WindowsGUI.dll", "CreateEventDispatcher" ) );

	int nResX = 1920, nResY = 1080;

	IInputManager::Desc oInputManagerDesc( *m_pEventDispatcher );
	oInputManagerDesc.m_nResX = nResX;
	oInputManagerDesc.m_nResY = nResY;
	m_pInputManager = static_cast< IInputManager* >( CPlugin::Create( oInputManagerDesc, sDirectoryName + "IO.dll", "CreateInputManager" ) );

	IActionManager::Desc oActionManagerDesc( *m_pInputManager );
	m_pActionManager = static_cast< IActionManager* >( CPlugin::Create( oActionManagerDesc, sDirectoryName + "IO.dll", "CreateActionManager" ) );

	IWindow::Desc oWindowDesc( nResX, nResY, "AC", *m_pEventDispatcher );
	oWindowDesc.m_bFullscreen = oOption.m_bFullscreen;
	oWindowDesc.m_nBits = 32;
	m_pWindow = static_cast< IWindow* >( CPlugin::Create( oWindowDesc, sDirectoryName + "WindowsGUI.dll", "CreateWindow2" ) );

	IRenderer::Desc oRendererDesc( *m_pWindow, *m_pFileSystem );
	oRendererDesc.m_sDefaultShader = "PerPixelLighting";
	oRendererDesc.m_sShaderDirectory = "Shaders";
	m_pRenderer = static_cast< IRenderer* >( CPlugin::Create( oRendererDesc, sDirectoryName + "Renderer.dll", "CreateRenderer" ) );
	m_pRenderer->SetBackgroundColor( 0, 100, 255 );

	m_pRenderer->AbonneToRenderEvent( OnRender );

	IGeometryManager::Desc oGeometryManagerDesc( NULL, "" );
	m_pGeometryManager = static_cast< IGeometryManager* >( CPlugin::Create( oGeometryManagerDesc, "Geometry.dll", "CreateGeometryManager" ) );

	ILoaderManager::Desc oLoaderDesc( *m_pFileSystem, *m_pGeometryManager );
	m_pLoaderManager =  static_cast< ILoaderManager* >( CPlugin::Create( oLoaderDesc, sDirectoryName + "Loader.dll", "CreateLoaderManager" ) );

	ISystemsManager::Desc oSystemsDesc( *m_pGeometryManager );
	m_pSystemsManager = static_cast< ISystemsManager* >( CPlugin::Create( oSystemsDesc, sDirectoryName + "Systems.dll", "CreateSystemsManager" ) );

	IRessourceManager::Desc oRessourceManagerDesc( *m_pRenderer, *m_pFileSystem, *m_pLoaderManager, *m_pSystemsManager );
	m_pRessourceManager = static_cast< IRessourceManager* >( CPlugin::Create( oRessourceManagerDesc, sDirectoryName + "Ressource.dll", "CreateRessourceManager" ) );

	ICollisionManager::Desc oCollisionManagerDesc( *m_pRenderer, *m_pLoaderManager, *m_pGeometryManager );
	oCollisionManagerDesc.m_pFileSystem = m_pFileSystem;
	m_pCollisionManager = static_cast< ICollisionManager* >( CPlugin::Create( oCollisionManagerDesc, "Collision.dll", "CreateCollisionManager" ) );

	IEntityManager::Desc oEntityManagerDesc( *m_pRessourceManager, *m_pRenderer, *m_pFileSystem, *m_pCollisionManager, *m_pGeometryManager );
	m_pEntityManager = static_cast< IEntityManager* >( CPlugin::Create( oEntityManagerDesc, sDirectoryName + "Entity.dll", "CreateEntityManager" ) );

	vector< IRenderer* > vRenderer;
	vRenderer.push_back( m_pRenderer );
	if ( m_pSoftRenderer )
		vRenderer.push_back( m_pSoftRenderer );
	ICameraManager::Desc oCameraManagerDesc( vRenderer );
	m_pCameraManager = static_cast< ICameraManager* >( CPlugin::Create( oCameraManagerDesc, sDirectoryName + "Entity.dll", "CreateCameraManager" ) );

	ISceneManager::Desc oSceneManagerDesc( *m_pRessourceManager, *m_pRenderer, *m_pCameraManager, *m_pEntityManager, *m_pLoaderManager, *m_pCollisionManager );
	m_pSceneManager = static_cast< ISceneManager* >( CPlugin::Create( oSceneManagerDesc, sDirectoryName + "Entity.dll", "CreateSceneManager" ) );

	IXMLParser::Desc oXMLParserDesc( *m_pFileSystem );
	m_pXMLParser = static_cast< IXMLParser* >( CPlugin::Create( oXMLParserDesc, sDirectoryName + "FileUtils.dll", "CreateXMLParser" ) );

	IGUIManager::Desc oGUIManagerDesc( *m_pRenderer, *m_pRessourceManager, *m_pXMLParser, *m_pInputManager );
	m_pGUIManager = static_cast< IGUIManager* >( CPlugin::Create( oGUIManagerDesc, sDirectoryName + "GUI.dll", "CreateGUIManager" ) );

	IScriptManager::Desc oScriptManagerDesc( *m_pFileSystem );
	m_pScriptManager = static_cast< IScriptManager* >( CPlugin::Create( oScriptManagerDesc, sDirectoryName + "Script.dll", "CreateScriptManager" ) );
	RegisterAllFunctions( m_pScriptManager );

	IConsole::Desc oConsoleDesc( *m_pInputManager, *m_pGUIManager, *m_pScriptManager );
	oConsoleDesc.xPos = 50;
	oConsoleDesc.yPos = 100;
	oConsoleDesc.m_nHeight = 800;
	m_pConsole = static_cast< IConsole* >( CPlugin::Create( oConsoleDesc, sDirectoryName + "IO.dll", "CreateConsole" ) );
	m_pConsole->SetConsoleShortCut(192);
}

int WINAPI WinMain( HINSTANCE hIstance, HINSTANCE hPrevInstance, LPSTR plCmdLine, int nCmdShow )
{
	m_pDebugTool = new CDebugTool;
#ifdef CATCH_EXCEPTION
	try
	{
#endif // CATCH_EXCEPTION

		InitPlugins( plCmdLine );

		
		ICamera* pFreeCamera = m_pCameraManager->CreateCamera( ICameraManager::T_FREE_CAMERA, 40.f, *m_pEntityManager );
		m_pCameraManager->SetActiveCamera( pFreeCamera );
		ICamera* pLinkCamera = m_pCameraManager->CreateCamera( ICameraManager::T_LINKED_CAMERA, 40.f, *m_pEntityManager );
		const char pAzerty[] = {'Z', 'S', 'Q', 'D'};
		const char pQwerty[] = {'W', 'S', 'A', 'D'};
		const char* pCurrent = pQwerty;

		m_pActionManager->AddKeyAction("Avancer" , pCurrent[0]);
		m_pActionManager->AddKeyAction("Reculer" , pCurrent[1]);
		m_pActionManager->AddKeyAction("StrafeLeft" , pCurrent[2]);
		m_pActionManager->AddKeyAction("StrafeRight" , pCurrent[3]);
		m_pActionManager->AddKeyAction("MoreSpeed" , 'V');
		m_pActionManager->AddKeyAction("LessSpeed" , 'C');
		m_pActionManager->AddKeyAction("CameraYaw" , AXIS_H);
		m_pActionManager->AddKeyAction("CameraPitch" , AXIS_V);
		m_pActionManager->AddKeyAction("Console", 222 );
		m_pActionManager->AddGUIAction("CursorX" , AXIS_H);
		m_pActionManager->AddGUIAction("CursorY" , AXIS_V);

		m_pActionManager->AddKeyAction( "AvancerPerso", 'T' );
		m_pActionManager->AddMouseAction( "HitLeftFoot", IInputManager::eMouseButtonLeft, IInputManager::eMouseButtonStateJustDown );

		m_pEventDispatcher->AbonneToWindowEvent( NULL, OnWindowEvent );
		m_pInputManager->AbonneToKeyEvent( NULL, OnKeyAction );
		
		InitScene( m_pSceneManager );
		m_pWindow->ShowModal();
		m_pRenderer->DestroyContext();

		delete m_pDebugTool;
		delete m_pCore;
		delete m_pConsole;
		delete m_pScriptManager;
		delete m_pGUIManager;
		delete m_pXMLParser;
		delete m_pEntityManager;	
		delete m_pSceneManager;
		delete m_pRessourceManager;
		delete m_pLoaderManager;
		delete m_pCameraManager;
		delete m_pSoftRenderer;
		delete m_pRenderer;
		delete m_pWindow;
		delete m_pActionManager;
		delete m_pInputManager;
		delete m_pEventDispatcher;
		delete m_pFileSystem;

#ifdef CATCH_EXCEPTION
	}
	catch( CFileNotFoundException& e )
	{
		string sMessage = string( "Fichier \"" ) + e.what() + "\" introuvable";
		MessageBoxA( NULL, sMessage.c_str(), "Erreur fichier", MB_ICONERROR );
	}
 	catch( exception& e )
	{
		MessageBoxA( NULL, e.what(), "", MB_ICONERROR );
	}
#endif // CATCH_EXCEPTION
	

	return 0;
}

void  OnWindowEvent( CPlugin* pPlugin, IEventDispatcher::TWindowEvent e, int nWidth, int nHeight )
{
	int test = 0;
	switch( e )
	{
	case IEventDispatcher::T_WINDOWUPDATE:
		OnUpdateWindow();
		break;
	}
}

//
//void OnPic01( IGUIManager::ENUM_EVENT eEvent )
//{
//	if ( eEvent == IGUIManager::EVENT_NONE )
//		return;
//	bool bDisplayMessage = false;
//	switch ( eEvent )
//	{
//	case IGUIManager::EVENT_LMOUSECLICK:
//		{
//			s_strMessage = "Click";
//			bDisplayMessage = true;
//			break;
//		}
//	case IGUIManager::EVENT_LMOUSERELEASED:
//		{
//			s_strMessage = "Released";
//			break;
//		}
//	case IGUIManager::EVENT_MOUSEMOVE:
//		{
//			s_strMessage = "Mouse move";
//			break;
//		}
//	case IGUIManager::EVENT_MOUSEENTERED:
//		{
//			s_strMessage = "Mouse entered";			
//			break;
//		}
//	case IGUIManager::EVENT_MOUSEEXITED:
//		{
//			s_strMessage = "Mouse exited";
//			break;
//		}	
//	}
//	if ( bDisplayMessage )
//		MessageBoxA( NULL, s_strMessage.c_str(), "", MB_OK );
//}


void OnInitGUI()
{
	//int hAnitaWidget = g_pCore->GUI_CreateImage( "anita.TETE",303,242 );
	//g_pCore->GUI_SetPosition( hAnitaWidget, 20, 500 );
	//
	//int hAnitaWidget2 = g_pCore->GUI_CreateImage("anita.SEIN", 263,136);
	//g_pCore->GUI_SetPosition( hAnitaWidget2, 400, 20 );

	//s_hWindow = g_pCore->GUI_CreateWindow( 0, 0, 1280, 1024 );
	//g_pCore->GUI_AddWidget( s_hWindow , hAnitaWidget );
	//g_pCore->GUI_AddWidget( s_hWindow , hAnitaWidget2 );
	////g_pCore->GUI_AddWidget( s_hWindow , hAnitaWidget3 );

	//int hAnitaListener = g_pCore->GUI_CreateListener( OnPic01 );
	//g_pCore->GUI_AddEventListener( hAnitaWidget2, hAnitaListener );
	//g_pCore->GUI_AddWindow( s_hWindow );
	//g_pCore->GUI_SetVisibility( s_hWindow, false );
}


void GetOptionsByCommandLine( string sCommandArguments, CGFXOption& oOption )
{
	if ( sCommandArguments.size() > 0 )
	{
		int nBegin = static_cast< int >( sCommandArguments.find( "-" ) );
		int nEnd = static_cast< int >( sCommandArguments.find( "=" ) );
		string sArgName = sCommandArguments.substr( nBegin + 1, nEnd - 1 );
		int nEndArgValue = static_cast< int >( sCommandArguments.find( " ", nEnd ) );
		if ( nEndArgValue == -1 )
		{
			nEndArgValue = static_cast< int >( sCommandArguments.find( "\n", nEnd + 1 ) );
			if ( nEndArgValue == -1 )
				nEndArgValue = static_cast< int >( sCommandArguments.size() - nEnd + 1 );
		}
		string sArgValue = sCommandArguments.substr( nEnd + 1, nEndArgValue );
		string sArgNameLow = sArgName;
		string sArgValueLow = sArgValue;
		transform( sArgName.begin(), sArgName.end(), sArgNameLow.begin(), tolower );
		transform( sArgValue.begin(), sArgValue.end(), sArgValueLow.begin(), tolower );
		if ( sArgName == "fullscreen" )
		{
			if ( sArgValue == "true" ) 
				oOption.m_bFullscreen = true;
			else
			{
				if ( sArgValue == "false" )
					oOption.m_bFullscreen = false;
				else
				{
					string sMessage = string( " Mauvais argument de la ligne de commande pour \"fullscreen\", valeur " ) + sArgValue + " non reconnue";
					exception e( sMessage.c_str() );
					throw e;
				}
			}
		}
	}
}