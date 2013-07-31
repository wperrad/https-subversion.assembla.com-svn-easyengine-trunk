#define GUIMANAGER_CPP

#include <algorithm>

#include "Guimanager.h"
#include "IXMLParser.h"
#include "exception.h"
#include "GUIWindow.h"
#include "IInputManager.h"
#include "IGUIManager.h"
#include "../Utils2/Chunk.h"
#include "IRenderer.h"
#include "IShader.h"
#include "IRessource.h"
#include "../Utils2/Dimension.h"
#include "../Utils2/Position.h"
#include "../Utils2/Rectangle.h"
#include "ILoader.h"

class CMaterial;
using namespace std;


//-----------------------------------------------------------------------------------------------------
//										Constructor
//-----------------------------------------------------------------------------------------------------


CGUIManager::CGUIManager( const Desc &oDesc ):
IGUIManager( oDesc ),
m_pCurrentWindow(NULL),
m_oRenderer( oDesc.m_oRenderer ),
m_oRessourceManager( oDesc.m_oRessourceManager ),
m_oXMLParser( oDesc.m_oXMLParser ),
m_oInputManager( oDesc.m_oInputManager ),
m_bActive( false ),
m_nCharspace( 1 )
{
	int nResWidth, nResHeight;
	m_oRenderer.GetResolution( nResWidth, nResHeight );
	
	if ( oDesc.m_sShaderName == "" )
		m_pShader = m_oRenderer.GetShader( "GUI" );

	CGUIWidget::Init( nResWidth, nResHeight, m_pShader );

#ifdef DISPLAYCURSOR
	m_pCursor = CreateImageFromSkin("GUI.CURSOR", 20,29 );
#endif // DISPLAYCURSOR

	SetActive( true );
	InitFontMap();
}


//-----------------------------------------------------------------------------------------------------
//										Destructor
//-----------------------------------------------------------------------------------------------------
CGUIManager::~CGUIManager(void)
{
	for( std::map< unsigned char, CGUIWidget* >::iterator it = m_mWidgetFont.begin(); it != m_mWidgetFont.end(); it++ )
		delete it->second;
}

unsigned int CGUIManager::GetCharSpace()
{
	return m_nCharspace;
}

void CGUIManager::InitFontMap()
{
	CRectangle rect;
	rect.SetPosition( CPosition( 0, 0 ) );
	rect.SetDimension( CDimension( 256, 256 ) );
	
	vector< unsigned char > vData;
	vector< CPoint > vCharSize;
	CDimension dim = rect.m_oDim;
	CreateFontBitmap( "Arial", dim.GetWidth(), vData, vCharSize );
	IShader* pShader = m_oRenderer.GetShader( "gui");	
	ITexture* pTextureFont = m_oRessourceManager.CreateTexture2D( m_oRenderer, pShader, 3, vData, dim.GetWidth(), dim.GetHeight(), IRenderer::T_RGBA );	
	
	CRectangle char0( 0, 12 * (float)rect.m_oDim.GetHeight() / 16.f, vCharSize[ 48 ].m_x, vCharSize[  48 ].m_y );

	for( int i = 0; i < 16; i++ )
	{
		for( int j = 0; j < 16; j++ )
		{
			char c = (char) ( i * 16 + j );
			CRectangle charRect( j * 16, 16 * i, vCharSize[ i * 16 + j ].m_x, vCharSize[ i * 16 + j ].m_y );
			m_mWidgetFont[ c ] = CreateImageFromTexture( pTextureFont, charRect );
		}
	}
}

void CGUIManager::GetScreenCoordFromTexCoord( const CRectangle& oTexture, const CDimension& oScreenDim, CRectangle& oScreen )
{
	int nResWidth = oScreenDim.GetWidth(), nResHeight = oScreenDim.GetHeight();
	oScreen.m_oPos.SetX( -1 );
	float fWidthRatio = 2 * (float)oTexture.m_oDim.GetWidth() / (float)nResWidth;
	float fHeightRatio = 2 * (float)oTexture.m_oDim.GetHeight() / (float)nResHeight;
	oScreen.m_oPos.SetY( 1 - fHeightRatio );
	oScreen.m_oDim.SetWidth( fWidthRatio );
	oScreen.m_oDim.SetHeight( fHeightRatio );
}

void CGUIManager::FlipBitmap( const unsigned char* data, int w, int h, int depth, vector< unsigned char >& vData )
{	
	for( int i = 0; i < h; i++ )
	{
		for( int j = 0; j < w; j++ )
		{
			for( int k = 0; k < depth; k++ )
			{
				int iIndice = 3 * ( w * ( i + 1 ) - ( j + 1 ) ) + k;
				vData.push_back( data[ iIndice  ] );
			}
		}
	}
}

void CGUIManager::CreateFontBitmap( string FontName, int nSize, vector< unsigned char >& vData, vector< CPoint >& vCharSize )
{
	int Quality = nSize / 16;
	HDC hDC = CreateCompatibleDC( NULL );
	BITMAPINFO bi;
	memset( &bi, 0, sizeof(BITMAPINFO));
	bi.bmiHeader.biHeight = nSize;
	bi.bmiHeader.biWidth = nSize;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biPlanes = 1;
	unsigned char* data = NULL;
	HBITMAP hBitmap = CreateDIBSection( hDC, &bi, DIB_RGB_COLORS, reinterpret_cast< void** >( &data ), NULL, 0 );
	HFONT hFont = CreateFont(Quality, 0, 0, 0, FW_NORMAL, FALSE,
                                  FALSE, FALSE, DEFAULT_CHARSET,
                                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                  ANTIALIASED_QUALITY, DEFAULT_PITCH,
                                  FontName.c_str());

	SelectObject(hDC, hBitmap);
	SelectObject(hDC, hFont);

	SetBkColor(hDC, RGB(0, 0, 0));
	SetTextColor( hDC, RGB(255, 255, 255) );

    char Character = 0;
    for (int j = 0; j < 16; ++j)
	{
        for (int i = 0; i < 16; ++i, ++Character)
        {
            // Stockage de la taille du caractère
            SIZE Size;
            GetTextExtentPoint32(hDC, &Character, 1, &Size);
			vCharSize.push_back( CPoint( Size.cx, Size.cy ) );

            // Affichage du caractère
            RECT Rect = {i * Quality, j * Quality, (i + 1) * Quality, (j + 1) * Quality};
            DrawText(hDC, &Character, 1, &Rect, DT_LEFT);
        }
	}
	vector< unsigned char > vData2;
	FlipBitmap( data, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, bi.bmiHeader.biBitCount / 8, vData2 );
	
	for( unsigned int i = 0; i < vData2.size() / 3; i++ )
	{
		for( int j = 0; j < 3; j++ )
			vData.push_back( vData2[ 3 * i + j ] );		
		if( vData2[ 3 * i ] == 0 && vData2[ 3 * i + 1 ] == 0 && vData2[ 3 * i + 2 ] == 0 )
			vData.push_back( 0 );
		else
			vData.push_back( 255 );
	}
}

CGUIWidget* CGUIManager::CreateImageFromTexture( ITexture* pTexture, const CRectangle& oSkin )
{
	string sShaderName = "gui";
	IShader* pShader = m_oRenderer.GetShader( sShaderName );

	int nTexDimWidth = 0, nTexDimHeight = 0;
	pTexture->GetDimension( nTexDimWidth, nTexDimHeight );
	CRectangle oFinalSkin = oSkin;
	if( oSkin.m_oDim.GetWidth() == 0 || oSkin.m_oDim.GetHeight() == 0 )
		oFinalSkin.SetDimension( CDimension( nTexDimWidth, nTexDimHeight ) );

	pTexture->SetShader( pShader );

	ILoader::CMeshInfos mi;

	
	CRectangle oScreenRect;
	int nResWidth, nResHeight;
	m_oRenderer.GetResolution( nResWidth, nResHeight );
	CDimension oScreenDim( nResWidth, nResHeight );
	GetScreenCoordFromTexCoord( oFinalSkin, oScreenDim, oScreenRect );

	float fScreenXmin = oScreenRect.m_oPos.GetX();
	float fScreenYmin = oScreenRect.m_oPos.GetY();
	float fScreenXmax = fScreenXmin + oScreenRect.m_oDim.GetWidth();
	float fScreenYmax = fScreenYmin + oScreenRect.m_oDim.GetHeight();

	mi.m_vVertex.push_back( fScreenXmin ); mi.m_vVertex.push_back( fScreenYmin ); mi.m_vVertex.push_back( 0 );	
	mi.m_vVertex.push_back( fScreenXmax ); mi.m_vVertex.push_back( fScreenYmin	);  mi.m_vVertex.push_back( 0 );	
	mi.m_vVertex.push_back( fScreenXmin ); mi.m_vVertex.push_back( fScreenYmax ); mi.m_vVertex.push_back( 0 ); 
	mi.m_vVertex.push_back( fScreenXmax ); mi.m_vVertex.push_back( fScreenYmax ); mi.m_vVertex.push_back( 0 );	

	mi.m_vIndex.push_back( 2 );mi.m_vIndex.push_back( 0 );mi.m_vIndex.push_back( 3 );
	mi.m_vIndex.push_back( 1 );mi.m_vIndex.push_back( 3 );mi.m_vIndex.push_back( 0 );

	CRectangle oTextureRect( 0, 0, nTexDimWidth, nTexDimHeight );
	fScreenXmax = 1 - (float)oFinalSkin.m_oPos.GetX() / (float)oTextureRect.m_oDim.GetWidth();
	fScreenYmax = 1 - (float)oFinalSkin.m_oPos.GetY() / (float)oTextureRect.m_oDim.GetHeight();
	fScreenXmin = 1 - (float)( oFinalSkin.m_oPos.GetX() + oFinalSkin.m_oDim.GetWidth() ) / (float) ( oTextureRect.m_oDim.GetWidth() );
	fScreenYmin = 1 - (float)( oFinalSkin.m_oPos.GetY() + oFinalSkin.m_oDim.GetHeight() ) / (float) ( oTextureRect.m_oDim.GetHeight() );
	float pUVVertex[ 8 ] = {  fScreenXmin, fScreenYmin, fScreenXmax, fScreenYmin, fScreenXmin, fScreenYmax, fScreenXmax, fScreenYmax };
	for( int i = 0; i < 8; i++ )
		mi.m_vUVVertex.push_back( pUVVertex[ i ] );
	mi.m_vUVIndex.push_back( 3 );mi.m_vUVIndex.push_back( 1 );mi.m_vUVIndex.push_back( 2 );
	mi.m_vUVIndex.push_back( 0 );mi.m_vUVIndex.push_back( 2 );mi.m_vUVIndex.push_back( 1 );
	
	for( int i = 0; i < 6; i++ )
		mi.m_vNormalFace.push_back( 0.f );

	for( int i = 0; i < 18; i++ )
		mi.m_vNormalVertex.push_back( 0.f );

	mi.m_bCanBeIndexed = false;
	//mi.m_oMaterialInfos.m_sMaterialName = sShaderName;
	mi.m_oMaterialInfos.m_sShaderName = sShaderName;
	mi.m_sShaderName = sShaderName;

	ILoader::CAnimatableMeshData oData;
	oData.m_vMeshes.push_back( mi );
	IRessource* pMaterial = m_oRessourceManager.CreateMaterial( mi.m_oMaterialInfos, m_oRenderer, pTexture );
	IAnimatableMesh* pARect = m_oRessourceManager.CreateMesh( oData, m_oRenderer, pMaterial );
	IMesh* pRect = pARect->GetMesh( 0 );
	CGUIWidget* pWidget = new CGUIWidget( oFinalSkin.m_oDim.GetWidth(), oFinalSkin.m_oDim.GetHeight() );
	pWidget->SetRect( pRect );
	return pWidget;
}

CGUIWidget* CGUIManager::CreateImageFromFile( const string& sTextureName, const CRectangle& skin )
{
	ITexture* pTexture = static_cast< ITexture* > (  m_oRessourceManager.GetRessource( sTextureName, m_oRenderer ) );
	return CreateImageFromTexture( pTexture, skin );
}

int	CGUIManager::CreateImage( const string& sFileName, unsigned int nWidth, unsigned int nHeight )
{
	size_t nDotPos = sFileName.find( "." );
	string sExt = sFileName.substr( nDotPos + 1 );
	std::transform( sExt.begin(), sExt.end(), sExt.begin(), tolower );
	CGUIWidget* pWidget = NULL;
	if( sExt == "cursor" )
		pWidget = CreateImageFromSkin( sFileName, nWidth, nHeight );
	else
		pWidget = CreateImageFromFile( sFileName, CRectangle( 0, 0, nWidth, nHeight ) );
	return InsertWidgetInMap( pWidget );
}

//-----------------------------------------------------------------------------------------------------
//											_CreateImage
//-----------------------------------------------------------------------------------------------------
CGUIWidget* CGUIManager::CreateImageFromSkin( const string& sSkinPath, unsigned int nWidth, unsigned int nHeight )
{
	size_t nDotPos = sSkinPath.find( "." );
	string sImageName = sSkinPath.substr( 0, nDotPos ) + ".tga";
	ITexture* pTexture = static_cast< ITexture* > ( m_oRessourceManager.GetRessource( sImageName, m_oRenderer ) );
	pTexture->SetShader( m_oRenderer.GetShader( "gui" ) );
	int nResWidth, nResHeight;
	m_oRenderer.GetResolution( nResWidth, nResHeight );
	
	ILoader::CMeshInfos mi;
	//vector< float > vVertexArray;

	
	float fNewX = -1.f + 2.f*(float)nWidth/(float)nResWidth;
	float fNewY = 1.f - 2.f*(float)nHeight/(float)nResHeight;

	mi.m_vVertex.push_back( -1.f ); mi.m_vVertex.push_back( fNewY ); mi.m_vVertex.push_back( 0 );	
	mi.m_vVertex.push_back( -1.f ); mi.m_vVertex.push_back( 1.f	);  mi.m_vVertex.push_back( 0 );	
	mi.m_vVertex.push_back( fNewX ); mi.m_vVertex.push_back( 1.f ); mi.m_vVertex.push_back( 0 ); 
	mi.m_vVertex.push_back( fNewX ); mi.m_vVertex.push_back( fNewY ); mi.m_vVertex.push_back( 0 );

	//vector< unsigned int > vIndexArray;
	mi.m_vIndex.push_back( 1 ); mi.m_vIndex.push_back( 0 ); mi.m_vIndex.push_back( 2 );
	mi.m_vIndex.push_back( 3 ); mi.m_vIndex.push_back( 2 ); mi.m_vIndex.push_back( 0 );
	
	string strXMLName = sSkinPath.substr( 0, nDotPos ) + ".xml";
	string sPropertyName = sSkinPath.substr( nDotPos+1, sSkinPath.size() - 1 );
	
	m_oXMLParser.OpenFile( strXMLName );
	CPosition SkinPos;
	int nSkinDimWidth = 0;
	int nSkinDimHeight = 0;
	try
	{
		m_oXMLParser.GetProperty( sPropertyName, SkinPos, nSkinDimWidth, nSkinDimHeight );
		m_oXMLParser.CloseFile();
	}
	catch (CXMLParserException& e)
	{
		string sMessage = sSkinPath + " not found";
		MessageBoxA( NULL, sMessage.c_str(), "skinset not found" , MB_ICONERROR);
	}
	if (nWidth == 0 && nHeight == 0)
	{
		nWidth = nSkinDimWidth;
		nHeight = nSkinDimHeight;
	}
	//vector< float > vUVVertexArray;
	int nTexDimWidth = 0, nTexDimHeight = 0;
	pTexture->GetDimension( nTexDimWidth, nTexDimHeight );
	float fTexMinX, fTexMinY, fTexMaxX, fTexMaxY;
	fTexMinX = (float)SkinPos.GetX() / (float)nTexDimWidth;
	fTexMaxX = ((float)SkinPos.GetX() + (float)nSkinDimWidth )/ (float)nTexDimWidth;

	fTexMaxY = 1- ((float)SkinPos.GetY()  / (float)nTexDimHeight );	
	fTexMinY = 1- (((float)SkinPos.GetY()+ (float)nSkinDimHeight ) / (float)nTexDimHeight );

	mi.m_vUVVertex.push_back( fTexMinX ); mi.m_vUVVertex.push_back( fTexMinY ), mi.m_vUVVertex.push_back( 0 );
	mi.m_vUVVertex.push_back( fTexMinX ); mi.m_vUVVertex.push_back( fTexMaxY ), mi.m_vUVVertex.push_back( 0 );
	mi.m_vUVVertex.push_back( fTexMaxX ); mi.m_vUVVertex.push_back( fTexMaxY ), mi.m_vUVVertex.push_back( 0 );
	mi.m_vUVVertex.push_back( fTexMaxX ); mi.m_vUVVertex.push_back( fTexMinY ), mi.m_vUVVertex.push_back( 0 );

	//vector< unsigned int > vUVIndexArray;
	mi.m_vUVIndex.push_back( 1 ); mi.m_vUVIndex.push_back( 0 ); mi.m_vUVIndex.push_back( 2 );
	mi.m_vUVIndex.push_back( 3 ); mi.m_vUVIndex.push_back( 2 ); mi.m_vUVIndex.push_back( 0 );

	//vector< float > vNormalVertexArray;
	mi.m_vNormalVertex.push_back( 0 ); mi.m_vNormalVertex.push_back( 0 ); mi.m_vNormalVertex.push_back( 1 );
	mi.m_vNormalVertex.push_back( 0 ); mi.m_vNormalVertex.push_back( 0 ); mi.m_vNormalVertex.push_back( 1 );
	mi.m_vNormalVertex.push_back( 0 ); mi.m_vNormalVertex.push_back( 0 ); mi.m_vNormalVertex.push_back( 1 );
	mi.m_vNormalVertex.push_back( 0 ); mi.m_vNormalVertex.push_back( 0 ); mi.m_vNormalVertex.push_back( 1 );
	mi.m_vNormalVertex.push_back( 0 ); mi.m_vNormalVertex.push_back( 0 ); mi.m_vNormalVertex.push_back( 1 );
	mi.m_vNormalVertex.push_back( 0 ); mi.m_vNormalVertex.push_back( 0 ); mi.m_vNormalVertex.push_back( 1 );

	IRessource* pMaterial = NULL;//m_oRessourceManager.CreateMaterial( pTexture, m_oRenderer, "gui" );

	ILoader::CMaterialInfos oMaterialInfos;
	oMaterialInfos.m_sName = "gui";

	ILoader::CAnimatableMeshData oData;
	oData.m_vMeshes.push_back( mi );
	IRessource* pMesh = m_oRessourceManager.CreateMesh( oData, m_oRenderer, pMaterial );
	
	CGUIWidget* pWidget = new CGUIWidget( nWidth, nHeight );
	pWidget->SetRect( pMesh );
	pWidget->SetSkinName( sPropertyName );	
	return pWidget;
}

//-----------------------------------------------------------------------------------------------------
//										SetVisibility
//-----------------------------------------------------------------------------------------------------
void CGUIManager::SetVisibility( int hWindow, bool bVisible )
{
	CGUIWindow* pWindow = dynamic_cast< CGUIWindow* > ( GetWidgetByHandle( hWindow ) );
	if ( pWindow == NULL )
	{
		exception e( "Invalid CGUIWindow handle" );
		throw e;
	}
	pWindow->SetVisibility( bVisible );
}

void CGUIManager::AddWidget( int hWindow, int hWidget )
{
	CGUIWindow* pWindow = dynamic_cast< CGUIWindow* > ( GetWidgetByHandle( hWindow ) );
	if ( pWindow == NULL )
	{
		exception e( "CGUIManager::AddWidget : Window's handle is invalid" );
		throw e;
	}
	CGUIWidget* pWidget = GetWidgetByHandle( hWidget );
	pWindow->AddWidget( pWidget );
}


//-----------------------------------------------------------------------------------------------------
//										CreateGUIWindow
//-----------------------------------------------------------------------------------------------------
int CGUIManager::CreateGUIWindow(int nx, int ny , int nWidth, int nHeight)
{
	CGUIWindow* pGUIWindow = new CGUIWindow(nWidth, nHeight);
	return InsertWidgetInMap( pGUIWindow );
}

CGUIWindow* CGUIManager::_CreateGUIWindow(int nx, int ny , int nWidth, int nHeight)
{
	CGUIWindow* pGUIWindow = new CGUIWindow(nWidth, nHeight);
	pGUIWindow->SetPosition( (float)nx, (float)ny );
	return pGUIWindow;
}

//-----------------------------------------------------------------------------------------------------
//										SetPosition
//-----------------------------------------------------------------------------------------------------
void CGUIManager::SetPosition( int hWidget, int nx, int ny)
{
	CGUIWidget* pWidget = GetWidgetByHandle( hWidget );
	pWidget->SetPosition(static_cast<float>(nx), static_cast<float>(ny));
}

//-----------------------------------------------------------------------------------------------------
//										Update
//-----------------------------------------------------------------------------------------------------
void CGUIManager::OnRender()
{
	m_oRenderer.EnableZTest( false );
	if ( m_bActive )
	{
		int nCursorXPos, nCursorYPos;
		m_oInputManager.GetCursorPos_EE( nCursorXPos, nCursorYPos );
		m_pShader->Enable( true );
		m_oRenderer.EnableTextureBlend( true );

		// test
		int x, y;
		m_oInputManager.GetCursorPos_EE( x, y );
		int nResWidth, nResHeight;
		m_oRenderer.GetResolution( nResWidth, nResHeight );
#ifdef DISPLAYCURSOR
		if ( x > nResWidth - m_pCursor->GetDimension().GetWidth() )
			m_oInputManager.SetMouseCursorXPos((int) ( nResWidth - m_pCursor->GetDimension().GetWidth() ));

		if ( y > nResHeight - m_pCursor->GetDimension().GetHeight())		
			m_oInputManager.SetMouseCursorYPos((int) ( nResHeight - m_pCursor->GetDimension().GetHeight()));
#endif //DISPLAYCURSOR

		if ( x < 0.f )
			m_oInputManager.SetMouseCursorXPos( 0 );
		if ( y < 0.f )
			m_oInputManager.SetMouseCursorYPos( 0 );
		//unsigned int state = m_oInputManager.GetMouseButtonState();
		// fin test

		if (m_pCurrentWindow)
		{
			size_t nWidgetCount = m_pCurrentWindow->GetWidgetCount() ;
			for ( unsigned int i = 0; i < nWidgetCount; i++ )
			{
				CGUIWidget* pWidget = m_pCurrentWindow->GetWidget( i );
				pWidget->Display( m_oRessourceManager );
			}
			IInputManager::TMouseButtonState eButtonState = m_oInputManager.GetMouseButtonState( IInputManager::eMouseButtonLeft );
			for (size_t i=0 ; i<nWidgetCount ; i++)
			{
				CGUIWidget* pWidget = m_pCurrentWindow->GetWidget( ( unsigned int ) i );
				pWidget->UpdateCallback( nCursorXPos,nCursorYPos, eButtonState );
			}
		}

		RenderText();


#ifdef DISPLAYCURSOR
		m_pCursor->SetPosition( static_cast<float>( nCursorXPos ), static_cast<float>( nCursorYPos ) );
		m_pCursor->GetLogicalPosition( fWidgetLogicalPosx, fWidgetLogicalPosy, nResWidth, nResHeight );
		vPos.clear();
		vPos.push_back( fWidgetLogicalPosx );
		vPos.push_back( fWidgetLogicalPosy );
		m_pShader->SendUniformVec2Array( "vImagePosition", vPos );
		m_pCursor->Display( m_oRessourceManager );
#endif // DISPLAYCURSOR
		m_oRenderer.EnableTextureBlend(false);
	}
	m_oRenderer.EnableZTest( true );
}


#ifdef DISPLAYCURSOR
//-----------------------------------------------------------------------------------------------------
//										GetCursorDimension
//-----------------------------------------------------------------------------------------------------
CDimension CGUIManager::GetCursorDimension()const
{
	return m_pCursor->GetDimension();
}
#endif // DISPLAYCURSOR
//-----------------------------------------------------------------------------------------------------
//										CreateListener
//-----------------------------------------------------------------------------------------------------
int CGUIManager::CreateListener( IGUIManager::EVENT_CALLBACK pfnCallBack )
{
	CListener* pListener = new CListener;
	pListener->SetEventCallBack( pfnCallBack );
	int hListener = (int)m_mListener.size();
	m_mListener[ hListener ] = pListener;
	return hListener;
}

//-----------------------------------------------------------------------------------------------------
//										AddEventListener
//-----------------------------------------------------------------------------------------------------
void CGUIManager::AddEventListener( int hWidget, int hListener )
{
	CListener* pListener = m_mListener[ hListener ];
	CGUIWidget* pWidget = m_mWidget[ hWidget ];
	pWidget->SetListener( pListener );
}

//-----------------------------------------------------------------------------------------------------
//										GetDimension
//-----------------------------------------------------------------------------------------------------
CDimension CGUIManager::GetDimension(CGUIWidget* pWidget)
{
	return pWidget->GetDimension();
}



//-----------------------------------------------------------------------------------------------------
//										AddWindow
//-----------------------------------------------------------------------------------------------------
void CGUIManager::AddWindow( int hWindow )
{
	CGUIWindow* pWindow = dynamic_cast< CGUIWindow* > ( GetWidgetByHandle( hWindow ) );
	if ( pWindow == NULL )
	{
		exception e( "Handle de CGUIWindow invalide" );
		throw e;
	}
	m_pCurrentWindow = pWindow;
}



//-----------------------------------------------------------------------------------------------------
//										IsVisible
//-----------------------------------------------------------------------------------------------------
bool CGUIManager::IsVisible(CGUIWindow* pWindow)
{
	return pWindow->IsVisible(); 
}

//-----------------------------------------------------------------------------------------------------
//										Print
//-----------------------------------------------------------------------------------------------------

void CGUIManager::Print( string sText, int x, int y )
{ 
	int nNewSize = m_vText.size() + 1 ;
	m_vText.resize( nNewSize );
	for( unsigned int i = 0; i < sText.size(); i++ )
	{
		CGUIWidget* pWidget = m_mWidgetFont[ sText[ i ] ];
		m_vText[ nNewSize - 1 ].m_vWidget.push_back( *pWidget );
	}
	m_vText[ nNewSize - 1 ].m_nPosX = x;
	m_vText[ nNewSize - 1 ].m_nPosY = y;
}

void CGUIManager::Print( char c, int x, int y )
{
	string s;
	s.push_back( c );
	Print( s, x, y );
}

//-----------------------------------------------------------------------------------------------------
//										EndRenderText
//-----------------------------------------------------------------------------------------------------
void CGUIManager::RenderText()
{
	
	for( unsigned int i = 0; i < m_vText.size(); i++ )
	{
		CLine& oLine = m_vText[ i ];
		int nCursorPosX = oLine.m_nPosX, nCursorPosY = oLine.m_nPosY;
		for( unsigned int j = 0; j < oLine.m_vWidget.size(); j++ )
		{
			CGUIWidget& oWidget = oLine.m_vWidget[ j ];
			if(  oWidget == *m_mWidgetFont[ '\n' ] )
			{
				nCursorPosX = oLine.m_nPosX;
				nCursorPosY += ( int )m_mWidgetFont[ 'A' ]->GetDimension().GetHeight() + 2;
				continue;
			}
			if( j > 0 )
				nCursorPosX += oLine.m_vWidget[ j - 1 ].GetDimension().GetWidth() + m_nCharspace;
			oWidget.SetPosition( nCursorPosX, nCursorPosY );
			oWidget.Display( m_oRessourceManager );
		}
	}
	m_vText.clear();
}

unsigned int CGUIManager::GetCurrentFontHeight() const
{
	map< unsigned char, CGUIWidget* >::const_iterator it =  m_mWidgetFont.find( 'A' );
	return ( unsigned int )it->second->GetDimension().GetHeight();
}

unsigned int CGUIManager::GetCurrentFontWidth( char c ) const
{
	map< unsigned char, CGUIWidget* >::const_iterator it = m_mWidgetFont.find( c );
	return ( unsigned int )it->second->GetDimension().GetWidth();
}

CGUIWidget* CGUIManager::GetWidgetByHandle( int hWidget ) const
{
	map< int, CGUIWidget* >::const_iterator itWidget = m_mWidget.find( hWidget );
	if ( itWidget == m_mWidget.end() )
	{
		exception e( "Identifiant de widget incorrect" );
		throw e;
	}
	CGUIWidget* pWidget = itWidget->second;
	return pWidget;
}

int CGUIManager::InsertWidgetInMap( CGUIWidget* pWidget )
{
	int hWidget = (int)m_mWidget.size();
	m_mWidget[ hWidget ] = pWidget;
	return hWidget;
}

void CGUIManager::SetActive( bool bActivate )
{
	m_bActive = bActivate;
}

bool CGUIManager::GetActive()
{
	return m_bActive;
}


extern "C" _declspec(dllexport) IGUIManager* CreateGUIManager( const IGUIManager::Desc& oDesc )
{
	return new CGUIManager( oDesc );
}