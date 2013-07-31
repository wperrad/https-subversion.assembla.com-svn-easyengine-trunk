#ifndef TextManager_H
#define TextManager_H

// System
#include <windows.h>

//stl
#include <string>
#include <vector>
#include <map>

using namespace std;

struct CRow
{
	int				m_nRowSize;
	string			m_sRowName;
	string			m_sRowCaption;
					CRow( int nRowSize, string sRowName, string sRowCaption );
};

class CTextManager
{
public:
	class CFont
	{
	public:
		struct CDesc
		{
			long	m_nHeight;
			bool	m_bBold;
			bool	m_bItalic;
			bool	m_bUnderlined;
			string	m_sFontName;
					CDesc();
					CDesc( int nHeight, bool bBold, bool bItalic, bool bUnderlined, string sFontName );
					~CDesc();
		};
				CFont( CDesc& oDesc );
		long	GetHeight()const;
		bool	IsItalic();
		bool	IsUnderline();
		bool	IsBold();
		void	Select( HDC hDC );

	private:
		HFONT	m_hFont;
		long	m_nHeight;
		bool	m_bBold;
		bool	m_bItalic;
		bool	m_bUnderlined;
	};

	class CFontManager
	{
	public:
		HFONT					GetFont( CFont::CDesc& oDesc ); //string sFontName, int nHeight, bool bUnderline, bool bBold, bool bItalic );
	private:
		map< string, HFONT >	m_mFont;
	};
	static CFontManager	s_oFontManager;


	struct CDesc
	{
		int		m_nTopMargin;
		int		m_nBottomMargin;
		bool	m_bNumerotation;
		int		m_nLineHeight;
				CDesc( int nTopMargin, int nBottomMargin, bool m_bNumerotation, int nLineHeight );
	};

	CTextManager( const CDesc& oDesc );
	~CTextManager();

	void			BeginDrawText(  HWND hWnd, HDC hDC );
	void			EndDrawText();
	void			DrawText_( const std::string& sText, UINT nTextLine, int nRow );
	void			DrawTextInCurrentCell( const std::string& sText, int nRow = -1 );
	void			EndLine( int n = 1 );
	void			NextRow( int nValue = 1 );
	void			AddRow( string sRowName, string sRowCaption, int nRowSize );
	void			InsertRow( string sRowName, int nRowSize, int nPosition );
	void			Reset();
	unsigned int	GetRowCount()const;
	unsigned int	GetLineCount()const;
	unsigned int	GetLineHeight()const;
	void			InitTextOrigin();
	unsigned int	GetLastPaintDisplayedLine();
	bool			IncrementTextPosition( int dx, int dy, bool bForce = false );
	void			GetRowName( int iRow, std::string& sRowName );
	void			GetRowCaption( int iRow, std::string& sRowName );
	int				GetRowWidth( int iRow );
	void			SetRowWidth( int iRow, int nWidth );
	void			SetTextPosition( int x, int y );
	void			GetTextPosition( int& x, int& y );
	int				GetTextX();
	int				GetTextY();
	void			ReturnToBeginLine();
	void			ScrollText( int x, int y );
	void			ScrollTextAbsolute( bool bVert, int nValue );	
	int				PixelToLine( int nScreenY )const;
	void			HighlightLine( int nLine, int nRGBColor );
	void			SelectLine( int nLine, bool bSelect, int nRGBColor  );
	void			UnselectAllLines();
	void			SelectGroup( int nLastLine, int nRGBColor, int nFirstLine = -1 );
	bool			IsSelectedLine( int nLine );
	void			UpdateText()const;
	int				GetStringLineCount( string s );
	//int				FieldToTextLine( string sField, int nOccurence  )const;
	void			SetBackgroundColor( int nColor );
	void			GetTextOrigin( int& x, int& y );
	void			SetTextOrigin( int x, int y );
	int				CreateFont( long nHeight, bool bBold, bool bItalic, bool bUnderline, string sFontName = "Courrier new"  );
	const CFont&	GetCurrentFont()const;
	void			SetFont( int nFont );
	int				GetTextWidth( string sText, int nFontID );
	static void		CutString( std::string sString, char cDelimiter, std::vector< std::string >& vString, bool bKeepDelimiter = false );

private:
	
	int				GetScrollPosyByTextPosy( int nTexty );
	int				ScreenToText( int nScreenLine );
	int				TextToScreen( int nTextLine );
	void			DrawLineNumber();
	void			DrawLineNumber( int& nInOutx );
	void			TextToPixelPos( int nLine, int nRow, int& x, int& y )const;
	void			CreateFonts( int nLineHeight );
	void			DestroyFonts();


	void			SetTextX( int x );
	void			SetTextY( int y );

	HWND						m_hWnd;
	int							m_nTextx;
	int							m_nTexty;
	UINT						m_nCurrentTextLine;
	int							m_nCurrentRow;
	int							m_nCurrentMargin;	
	std::vector< CRow >			m_vRow;
	HDC							m_hDC;
	unsigned int				m_nLineCount;
	int							m_nCurrentHighlightedLine;
	int							m_nHighlightColor;
	int							m_nLastSelectedLine;
	COLORREF					m_nOrgBkColor;
	std::map< int, bool >		m_mSelectedLines;
	int							m_nSelectionColor;
	bool						m_bNumerotation;
	int							m_nCharWidth;
	int							m_nLineNumberWidth;
	int							m_nTop;
	multimap< string, int >		m_mFieldToLine;
	vector< CFont >				m_vFont;
	int							m_nCurrentFont;

	static int					round( float f );
	
	static const unsigned int	s_nCharWidth;
	int							m_nTextOrgx;
	int							m_nTextOrgy;
};


#endif // TextManager_H