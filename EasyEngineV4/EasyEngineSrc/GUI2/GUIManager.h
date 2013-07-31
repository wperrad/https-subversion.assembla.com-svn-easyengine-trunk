#ifndef GUIMANAGER_CPP
#error
#endif

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

// stl
#include <sstream>
#include <map>
#include <string>

// Engine
#include "Listener.h"
#include "IGUIManager.h"
 
class CGUIWindow;
class CGUIWidget;
class CContainer;
class CRectangle;
class CDimension;
class CPosition;
class IShader;
class ITexture;
class IRessource;



using namespace std;

struct CPoint
{
	float m_x;
	float m_y;
	CPoint( float x, float y ) : m_x( x ), m_y( y ){}
};



class CGUIManager : public IGUIManager
{
	struct CLine
	{
		vector< CGUIWidget >	m_vWidget;
		int						m_nPosX;
		int						m_nPosY;
	};

	ILoaderManager*			m_pLoaderManager;
	bool					m_bActive;
	CGUIWindow*				m_pCurrentWindow;

	void					GetScreenCoordFromTexCoord( const CRectangle& oTexture, const CDimension& oScreenDim, CRectangle& oScreen );
	void					InitFontMap();

#ifdef DISPLAYCURSOR
	CGUIWidget*								m_pCursor;
#endif // DISPLAYCURSOR

	vector< CLine >							m_vText;
	map< unsigned char, CGUIWidget* >		m_mWidgetFont;
	IRenderer&								m_oRenderer;
	IRessourceManager&						m_oRessourceManager;
	IXMLParser&								m_oXMLParser;
	IInputManager&							m_oInputManager;

	ITexture*								m_pFontTexture;

	std::map< int, CGUIWidget* >			m_mWidget;
	std::map< int, CListener* >				m_mListener;
	CGUIWidget*								GetWidgetByHandle( int hWidget ) const;
	CGUIWidget*								CreateImageFromSkin( const std::string& szFileName, unsigned int nWidth=0, unsigned int nHeight=0 );
	CGUIWidget*								CreateImageFromFile( const std::string& sTextureName, const CRectangle& rect);
	CGUIWidget*								CreateImageFromTexture( ITexture* pTexture, const CRectangle& oSkin );
	CGUIWindow*								_CreateGUIWindow(int nx, int ny , int nWidth, int nHeight);
	int										InsertWidgetInMap( CGUIWidget* pWidget );
	bool									IsVisible(CGUIWindow* pWindow);
	CDimension								GetDimension(CGUIWidget* pWidget);
	void									CreateFontBitmap( string FontName, int nSize, vector< unsigned char >& vData, vector< CPoint >& vCharSize );
	void									FlipBitmap( const unsigned char* data, int w, int h, int depth, vector< unsigned char >& vData );
	IShader*								m_pShader;
	int										m_nCharspace;

	void									RenderText();
	
	
public:

					CGUIManager( const Desc& desc );
	virtual			~CGUIManager(void);	
	int				CreateImage( const std::string& sFileName, unsigned int nWidth=0, unsigned int nHeight=0 );
	void			SetVisibility( int hWindow, bool bVisible );
	void			AddWidget( int hWindow, int hWidget );
	void			AddWindow( int hWindow );
	int				CreateGUIWindow(int nx, int ny , int nWidth, int nHeight);
	void			SetPosition( int hWidget, int nx, int ny);	
	void			OnRender();
#ifdef 	DISPLAYCURSOR
	CDimension		GetCursorDimension()const;
#endif // DISPLAYCURSOR
	int				CreateListener( IGUIManager::EVENT_CALLBACK pfnCallBack );
	void			AddEventListener( int hWidget, int hListener);
	void			Print( std::string sText, int x, int y);
	void			Print( char c, int x, int y );
	void			SetY( int hWidget, int y );
	void			SetSkinName( int hWindget, const std::string& sName );
	void			SetActive( bool bActivate );
	bool			GetActive();
	unsigned int	GetCurrentFontHeight() const;
	unsigned int	GetCurrentFontWidth( char c ) const;
	unsigned int	GetCharSpace();
};

extern "C" _declspec(dllexport) IGUIManager* CreateGUIManager( const IGUIManager::Desc& );
#endif