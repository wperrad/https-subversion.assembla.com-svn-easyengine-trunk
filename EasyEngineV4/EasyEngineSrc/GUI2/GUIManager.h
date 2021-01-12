#ifndef GUIMANAGER_H
#define GUIMANAGER_H

// stl
#include <sstream>
#include <map>
#include <string>

// Engine
#include "Listener.h"
#include "IGUIManager.h"
#include "ILoader.h"
 
class CGUIWindow;
class CGUIWidget;
class CContainer;
class CRectangle;
class CDimension;
class CPosition;
class IShader;
class ITexture;
class IRessource;
class IAnimatableMesh;
class IMesh;
class CTopicsWindow;

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
	bool					m_bGUIMode;
	CGUIWindow*				m_pCurrentWindow;

	void					GetScreenCoordFromTexCoord( const CRectangle& oTexture, const CDimension& oScreenDim, CRectangle& oScreen ) const;
	void					InitFontMap();

#ifdef DISPLAYCURSOR
	CGUIWidget*								m_pCursor;
#endif // DISPLAYCURSOR

	vector< CLine >							m_vText;
	map<unsigned char, CGUIWidget*>			m_mWidgetFontWhite;
	map<unsigned char, CGUIWidget*>			m_mWidgetFontBlue;
	map<unsigned char, CGUIWidget*>			m_mWidgetFontTurquoise;
	map<unsigned char, ILoader::CMeshInfos>	m_mWidgetFontInfos;
	ILoader::CMeshInfos						m_oLastWidgetInfosCreated;
	IRenderer&								m_oRenderer;
	IRessourceManager&						m_oRessourceManager;
	IXMLParser&								m_oXMLParser;
	IInputManager&							m_oInputManager;
	IRessource*								m_pFontMaterial;
	map< int, IAnimatableMesh* >			m_mStaticText;
	std::map< int, CGUIWidget* >			m_mWidget;
	std::map< int, CListener* >				m_mListener;
	CTopicsWindow*							m_pTopicsWindow;

	CGUIWidget*								GetWidgetByHandle( int hWidget ) const;
	CGUIWidget*								CreateImageFromSkin( const std::string& szFileName, unsigned int nWidth=0, unsigned int nHeight=0 );
	IMesh*									CreateImageFromTexture(ITexture* pTexture, const CRectangle& oSkin, const CDimension& oImageSize) const;
	void									CreateQuadMeshInfosFromTexture(ITexture* pTexture, const CRectangle& oSkin, ILoader::CMeshInfos& mi, CRectangle& oFinalSkin) const;
	CGUIWidget*								CreateFontImageFromTexture(ITexture* pTexture, const CRectangle& oSkin);
	CGUIWindow*								_CreateGUIWindow(int nx, int ny , int nWidth, int nHeight);
	int										InsertWidgetInMap( CGUIWidget* pWidget );
	bool									IsVisible(CGUIWindow* pWindow);
	CDimension								GetDimension(CGUIWidget* pWidget);
	void									CreateFontBitmap(string FontName, int nSize, vector< unsigned char >& vData, vector< CPoint >& vCharSize, int r, int g, int b);
	void									FlipBitmap( const unsigned char* data, int w, int h, int depth, vector< unsigned char >& vData );
	IGUIWindow*								CreatePlayerWindow(int nWidth, int nHeight);
	void									RenderText();

	IShader*								m_pShader;
	int										m_nCharspace;
	map< int, bool >						m_mStaticTextToRender;

	
	
public:

					CGUIManager( const Desc& desc );
	virtual			~CGUIManager(void);	
	int				CreateImage( const std::string& sFileName, unsigned int nWidth=0, unsigned int nHeight=0 );
	CGUIWidget*		CreateFontImageFromFile(const std::string& sTextureName, const CRectangle& rect);
	IMesh*			CreateImageFromFile(const string& sTextureName, const CRectangle& skin, const CDimension& oImageSize) const;
	void			SetVisibility( int hWindow, bool bVisible );
	void			AddWidget( int hWindow, int hWidget );
	void			AddWindow( int hWindow );
	void			SetCurrentWindow(IGUIWindow* pWindow);
	int				CreateGUIWindow(int nx, int ny , int nWidth, int nHeight);
	void			SetPosition( int hWidget, int nx, int ny);	
	void			OnRender();
#ifdef 	DISPLAYCURSOR
	CDimension		GetCursorDimension()const;
#endif // DISPLAYCURSOR
	int				CreateListener( IGUIManager::EVENT_CALLBACK pfnCallBack );
	void			AddEventListener( int hWidget, int hListener);
	void			Print( std::string sText, int x, int y, TFontColor color = eWhite);
	void			Print( char c, int x, int y );
	void			SetActive( bool bActivate );
	bool			GetActive();
	unsigned int	GetCurrentFontHeight() const;
	unsigned int	GetCurrentFontWidth( char c ) const;
	unsigned int	GetCharSpace();
	int				CreateStaticText( vector< string >& vText, int nPosX = 0, int nPosY = 0 );
	void			DestroyStaticTest( int nID );
	void			PrintStaticText( int nTextID );
	void			EnableStaticText( int nTextID, bool bEnable );
	IGUIWindow*		GetTopicsWindow();
	void			SetGUIMode(bool bGUIMode);
	bool			GetGUIMode();
	int				GetCurrentFontEspacementY();
};

extern "C" _declspec(dllexport) IGUIManager* CreateGUIManager( const IGUIManager::Desc& );
#endif