#pragma pack(1)

// System
#include <windows.h>
#include <commctrl.h>

// stl
#include <sstream>
#include <iomanip>
#include <algorithm>

// program
#include "TextManager.h"


using namespace std;

const unsigned int CTextManager::s_nCharWidth = 8;
CTextManager::CFontManager CTextManager::s_oFontManager;

CRow::CRow( int nRowSize, string sRowName, string sRowCaption ) : 
m_nRowSize( nRowSize ), 
m_sRowName( sRowName ), 
m_sRowCaption( sRowCaption )
{
}

CTextManager::CFont::CDesc::CDesc():
m_nHeight( 15 ),
m_bBold( false ),
m_bItalic( false ),
m_bUnderlined( false ),
m_sFontName( "Courier New" )
{
}

CTextManager::CFont::CDesc::CDesc( int nHeight, bool bBold, bool bItalic, bool bUnderlined, string sFontName ):
m_nHeight( nHeight ),
m_bBold( bBold ),
m_bItalic( bItalic ),
m_bUnderlined( bUnderlined ),
m_sFontName( sFontName )
{
}

CTextManager::CFont::CDesc::~CDesc()
{
}

CTextManager::CFont::CFont( CDesc& oDesc ):
m_bBold( oDesc.m_bBold ),
m_bItalic( oDesc.m_bItalic ),
m_bUnderlined( oDesc.m_bUnderlined ),
m_nHeight( oDesc.m_nHeight ),
m_hFont( 0 )
{
	m_hFont = CTextManager::s_oFontManager.GetFont( oDesc );
}

long CTextManager::CFont::GetHeight()const
{
	return m_nHeight;
}

bool CTextManager::CFont::IsItalic()
{
	return m_bItalic;
}
bool CTextManager::CFont::IsUnderline()
{
	return m_bUnderlined;
}
bool CTextManager::CFont::IsBold()
{
	return m_bBold;
}

void CTextManager::CFont::Select( HDC hDC )
{
	SelectObject( hDC, m_hFont );
}

HFONT CTextManager::CFontManager::GetFont( CFont::CDesc& oDesc )
{
	ostringstream ossID;
	ossID << oDesc.m_sFontName << oDesc.m_nHeight << oDesc.m_bUnderlined << oDesc.m_bBold << oDesc.m_bItalic;
	map< string, HFONT >::iterator itFont = m_mFont.find( ossID.str() );
	if ( itFont != m_mFont.end() )
		return itFont->second;
	LOGFONT lf;
	ZeroMemory( &lf, sizeof( LOGFONT ) );
	strcpy_s( lf.lfFaceName, oDesc.m_sFontName.c_str() );
	lf.lfHeight = oDesc.m_nHeight;
	lf.lfItalic = oDesc.m_bItalic;
	lf.lfUnderline = (BYTE)oDesc.m_bUnderlined;
	int nWeight = 0;
	if ( oDesc.m_bBold )
		nWeight = 600;
	lf.lfWeight = nWeight;
	HFONT hFont = CreateFontIndirect( &lf );
	m_mFont[ ossID.str() ] = hFont;
	return hFont;
}

CTextManager::CDesc::CDesc( int nTopMargin, int nBottomMargin, bool bNumerotation, int nLineHeight ) :
m_nTopMargin( nTopMargin ),
m_nBottomMargin( nBottomMargin ),
m_bNumerotation( bNumerotation ),
m_nLineHeight( nLineHeight )
{
}

CTextManager::CTextManager( const CDesc& oDesc ) :
m_nCurrentTextLine( 0 ),
m_nCurrentRow( 0 ),
m_nLineCount( 0 ),
m_nOrgBkColor( 0x00BBBBBB ),
m_nSelectionColor( RGB( 256, 100, 100 ) ),
m_nCurrentHighlightedLine( -1 ),
m_bNumerotation( oDesc.m_bNumerotation ),
m_nCharWidth( 10 ),
m_nLineNumberWidth( 8 ),
m_hDC( 0 ),
m_hWnd( 0 ),
m_nTextOrgx( 10 ),
m_nTextOrgy( 0 )
{
	m_nTop = ( oDesc.m_nTopMargin ) * oDesc.m_nLineHeight;
	m_nTextx = m_nTextOrgx;
	m_nTexty = m_nTextOrgy;
	CreateFonts( oDesc.m_nLineHeight );
}

CTextManager::~CTextManager(void)
{
	DestroyFonts();
}

int CTextManager::GetTextWidth( string sText, int nFontID )
{
	//GetTextEntentPoint32(
	throw 1;
	return 0;
}

const CTextManager::CFont& CTextManager::GetCurrentFont()const
{
	return m_vFont.at( m_nCurrentFont );
}

void CTextManager::SetFont( int nFont )
{
	m_nCurrentFont = nFont;
}

void CTextManager::GetTextOrigin( int& x, int& y )
{
	x = m_nTextOrgx;
	y = m_nTextOrgy;
}

void CTextManager::SetTextOrigin( int x, int y )
{
	m_nTextOrgx = x;
	m_nTextOrgy = y;
}

void CTextManager::AddRow( string sRowName, string sRowCaption, int nRowSize )
{
	m_vRow.push_back( CRow ( nRowSize, sRowName, sRowCaption ) );
}

unsigned int CTextManager::GetRowCount() const
{
	return ( unsigned int )m_vRow.size();
}

void CTextManager::InitTextOrigin()
{
	m_nTextx = m_nTextOrgx;
	m_nTexty = m_nTextOrgy;
}

unsigned int CTextManager::GetLineCount() const
{
	//return m_nLineCount;
	throw 1;
}

void CTextManager::CreateFonts( int nHeight )
{
	CFont::CDesc oDesc;
	oDesc.m_nHeight = nHeight;
	CFont oFont( oDesc );
	m_vFont.push_back( oFont );
	m_nCurrentFont = (int)( m_vFont.size() - 1 );
}

int CTextManager::CreateFont( long nHeight, bool bBold, bool bItalic, bool bUnderline, string sFontName )
{
	CFont::CDesc oDesc;
	oDesc.m_bBold = bBold;
	oDesc.m_bItalic = bItalic;
	oDesc.m_bUnderlined = bUnderline;
	oDesc.m_nHeight = nHeight;
	oDesc.m_sFontName = sFontName;
	CFont oFont( oDesc );
	m_vFont.push_back( oFont );
	return (int)( m_vFont.size() - 1 );
}

void CTextManager::DestroyFonts()
{
	return;
	for ( unsigned int i = 0; i < m_vFont.size(); i++ )
	{
		//m_vFont.at( i ).DeleteFont();
	}
	m_vFont.clear();
}

void CTextManager::BeginDrawText( HWND hWnd, HDC hDC )
{
	m_hWnd = hWnd;

	RECT rect;
	GetClientRect( hWnd, &rect );
	POINT p;
	MoveToEx( hDC, 0, m_nTop - 1, &p );
	//LineTo( hDC, rect.right, m_nTop  - 1 );

	m_hDC = hDC;
	m_nCurrentRow = 0;
	m_nCurrentTextLine = 0;
	SetScrollRange( m_hWnd, SB_VERT, 0, m_nLineCount, TRUE );
	m_vFont.at( m_nCurrentFont ).Select( hDC );
}

void CTextManager::EndDrawText()
{
	if( m_nLineCount < m_nCurrentTextLine )
		m_nLineCount = m_nCurrentTextLine;
}

void CTextManager::UpdateText()const
{
	RECT rect;
	GetClientRect( m_hWnd, &rect );
	rect.top = m_nTop;
	InvalidateRect( m_hWnd, &rect, TRUE );
}

void CTextManager::DrawLineNumber( int& nInOutx )
{
	DrawLineNumber();
	if ( m_bNumerotation )
		nInOutx += m_nLineNumberWidth * m_nCharWidth;
}

void CTextManager::DrawLineNumber()
{
	if ( m_bNumerotation )
	{
		int x, y;
		TextToPixelPos( m_nCurrentTextLine, m_nCurrentRow, x, y );
		ostringstream ossLine;
		ossLine << m_nCurrentTextLine;
		if ( m_nCurrentRow == 0 )
		{
			int size = (int)ossLine.str().size();
			TextOut( m_hDC, x, y, ossLine.str().c_str(), size );
		}
	}
}

void CTextManager::TextToPixelPos( int nLine, int nRow, int& x, int& y )const
{
	int nMargin = 0;
	for ( int i = 0; i < nRow; i++ )
		nMargin += m_vRow.at( i ).m_nRowSize;
	x = m_nTextOrgx + m_nTextx + nMargin;
	y = m_nTextOrgy + m_nTexty + nLine * m_vFont.at( m_nCurrentFont ).GetHeight() + m_nTop;
}

unsigned int CTextManager::GetLineHeight() const
{
	return m_vFont.at( m_nCurrentFont ).GetHeight();
}

void CTextManager::SetBackgroundColor( int nColor )
{
	m_nOrgBkColor = nColor;
}

void CTextManager::DrawText_( const std::string& sText, UINT nTextLine, int nRow )
{
	int x, y;
	TextToPixelPos( nTextLine, nRow, x, y );
	if ( y >= m_nTop )
	{
		bool bIsSelected = false, bIsHighlight = false;
		map< int, bool >::iterator itSelection = m_mSelectedLines.find( nTextLine );
		if ( itSelection != m_mSelectedLines.end() )
			if ( itSelection->second )
				bIsSelected = true;

		if ( m_nCurrentHighlightedLine == nTextLine )
			bIsHighlight = true;

		int nBkColor = m_nOrgBkColor;
		if( bIsSelected && !bIsHighlight )
			nBkColor = m_nSelectionColor;
		else
		{
			if ( bIsSelected && bIsHighlight )
				nBkColor = m_nHighlightColor;
			else
			{
				if ( bIsHighlight )
					nBkColor = m_nHighlightColor;
			}
		}

		SetBkColor( m_hDC, nBkColor );
		DrawLineNumber( x );
		int nTextCount = (int)sText.size();
		if ( m_vRow[ nRow ].m_nRowSize == 0 )
			nTextCount = 0;
		TextOut( m_hDC, x, y, sText.c_str(), nTextCount );
		if ( nTextLine + 1 > m_nLineCount )
			m_nLineCount = nTextLine + 1;
	}
}

void CTextManager::DrawTextInCurrentCell( const std::string& sText, int nRow )
{
	if ( nRow == -1 )
		nRow = m_nCurrentRow;
	DrawText_( sText, m_nCurrentTextLine, nRow );
	NextRow();
}

void CTextManager::EndLine( int n )
{
	m_nCurrentTextLine += n;
	ReturnToBeginLine();
}

void CTextManager::ReturnToBeginLine()
{
	m_nCurrentRow = 0;
	m_nCurrentMargin = 0;
	DrawLineNumber();
}

bool CTextManager::IncrementTextPosition( int dx, int dy, bool bForce )
{
	bool bChanged = false;
	if ( dx < 0 || ( m_nTextx < m_nTextOrgx && dx > 0 ) || bForce )
	{
		if ( !bForce && ( m_nTextx + dx > m_nTextOrgx ) )
			m_nTextx = m_nTextOrgx;
		else
			m_nTextx += dx;
		bChanged = true;
	}
	else
	{
		if ( dy < 0 || ( m_nTexty < m_nTextOrgy && dy > 0 ) || bForce )
		{
			if ( !bForce && ( m_nTexty + dy > m_nTextOrgy ) )
				m_nTexty = m_nTextOrgx;
			else
				m_nTexty += dy;
			bChanged = true;
		}
	}
	return bChanged;
}


void CTextManager::NextRow( int nValue )
{
	if ( m_nCurrentRow + 1 >= (int)m_vRow.size() )
		EndLine();
	else
	{
		m_nCurrentMargin += m_vRow[ m_nCurrentRow ].m_nRowSize;
		m_nCurrentRow++;
	}
}

void CTextManager::CutString( std::string sString, char cDelimiter, vector< string >& vString, bool bKeepDelimiter )
{
	bool bChar = false;
	int iCharPos = 0;
	string sNewText = sString;
	while ( iCharPos != -1 && sNewText != "" )
	{
		iCharPos = (int)sNewText.find_first_of( cDelimiter );
		if ( iCharPos != -1 )
		{
			if ( iCharPos > 0 )
				vString.push_back( sNewText.substr( 0, iCharPos ) );
			if( bKeepDelimiter )
			{
				string sDelimiter;
				sDelimiter.push_back( cDelimiter );
				vString.push_back( sDelimiter );
			}
			sNewText = sNewText.substr( iCharPos + 1 );
		}
		else
		{
			if ( sNewText != "" )
				vString.push_back( sNewText );
		}
	}
}

void CTextManager::GetRowName( int iRow, std::string& sRowName )
{
	sRowName = m_vRow[ iRow ].m_sRowName;
}

void CTextManager::GetRowCaption( int iRow, string& sRowCaption )
{
	sRowCaption = m_vRow[ iRow ].m_sRowCaption;
}

int CTextManager::GetRowWidth( int iRow )
{
	return m_vRow[ iRow ].m_nRowSize;
}

void CTextManager::SetRowWidth( int iRow, int nWidth )
{
	m_vRow[ iRow ].m_nRowSize = nWidth;
}

unsigned int CTextManager::GetLastPaintDisplayedLine()
{
	return m_nLineCount;
}

int CTextManager::round( float f )
{
	int e = static_cast< int >( f );
	float r = f - (float)e;
	if ( r >= 0.5 )
		return e+1;
	return e;
}

int CTextManager::GetScrollPosyByTextPosy( int nTexty )
{
	int nMin, nMax;
	GetScrollRange( m_hWnd, SB_VERT, &nMin, &nMax );
	float c = - (float)( (int)m_vFont.at( m_nCurrentFont ).GetHeight() * (int)m_nLineCount ) / (float)( nMax - nMin );
	float b = (float) ( m_nTextOrgy - c * nMin );
	float fScrollPos = ( (float)m_nTexty - b ) / c;
	return round( fScrollPos );
}

void CTextManager::SetTextPosition( int x, int y )
{
	int xTemp = m_nTextx;
	int yTemp = m_nTexty;
	SetTextX( x );
	SetTextY( y );
	int dx = m_nTextx - xTemp;
	int dy = m_nTexty - yTemp;

	RECT rect;
	GetClientRect( m_hWnd, &rect );
	rect.top += m_nTop;
	ScrollWindow( m_hWnd, dx, dy, &rect, &rect );

	if ( dx != 0 || dy != 0 )
	{
		int nBar = SB_VERT;
		int delta = dy;
		if ( dx != 0 ) 
		{
			nBar = SB_HORZ;
			delta = dx;
		}
		int nNewScrollPos = 0;
		if ( dy != 0 )
			nNewScrollPos = GetScrollPosyByTextPosy( m_nTexty );
		SetScrollPos( m_hWnd, nBar, nNewScrollPos, TRUE );
	}
}

void CTextManager::SetTextX( int x )
{
	if ( m_nTextx + x <= m_nTextOrgx )
		m_nTextx = x;
	else
		m_nTextx = m_nTextOrgx;
}

void CTextManager::SetTextY( int y )
{
	if ( m_nTexty + y <= m_nTextOrgy )
		m_nTexty = y;
	else
		m_nTexty = m_nTextOrgy;
}

void CTextManager::GetTextPosition( int& x, int& y )
{
	x = m_nTextx;
	y = m_nTexty;
}

int CTextManager::GetTextX()
{
	return m_nTextx;
}

int CTextManager::GetTextY()
{
	return m_nTexty;
}

int CTextManager::ScreenToText( int nScreenLine )
{
	int nDelta = (int)( (float)m_nTexty / (float)m_vFont.at( m_nCurrentFont ).GetHeight() );
	return nScreenLine - nDelta;
}

int CTextManager::TextToScreen( int nTextLine )
{
	int nDelta = (int)( (float)(m_nTexty + m_nTop ) / (float)m_vFont.at( m_nCurrentFont ).GetHeight() );
	return nTextLine + nDelta;
}

int CTextManager::PixelToLine( int nScreenY )const
{
	float fCorrection = -m_vFont.at( m_nCurrentFont ).GetHeight() / 3;
	int nTest = round ( (float)( nScreenY + fCorrection - m_nTextOrgy - m_nTexty - m_nTop ) / (float)m_vFont.at( m_nCurrentFont ).GetHeight()  );
	return nTest;
}

void CTextManager::HighlightLine( int nTextLine, int nRGBColor )
{
	int nScreenLine = TextToScreen( nTextLine );
	if ( ( nTextLine != m_nCurrentHighlightedLine ) && ( nScreenLine >=0 ) )
	{
		RECT rect;
		GetClientRect( m_hWnd, &rect );
		rect.top = ( nScreenLine ) * m_vFont.at( m_nCurrentFont ).GetHeight() ;
		rect.bottom = ( nScreenLine + 1 ) * m_vFont.at( m_nCurrentFont ).GetHeight() ;
		InvalidateRect( m_hWnd, &rect, TRUE );

		int nCurrentSelectedScreenLine = TextToScreen( m_nCurrentHighlightedLine );
		rect.top = nCurrentSelectedScreenLine * m_vFont.at( m_nCurrentFont ).GetHeight() ;
		rect.bottom = ( nCurrentSelectedScreenLine + 1 ) * m_vFont.at( m_nCurrentFont ).GetHeight() ;
		InvalidateRect( m_hWnd, &rect, TRUE );

		m_nCurrentHighlightedLine = nTextLine;
		m_nHighlightColor = nRGBColor;
	}
}

void CTextManager::SelectLine( int nTextLine, bool bSelect, int nRGBColor )
{
	m_nLastSelectedLine = nTextLine;
	m_mSelectedLines[ nTextLine ] = bSelect;
	m_nSelectionColor = nRGBColor;
	HighlightLine( nTextLine, nRGBColor );
}

void CTextManager::SelectGroup( int nLastLine, int nRGBColor, int nFirstLine )
{
	if ( nFirstLine == -1 )
		nFirstLine = m_nLastSelectedLine;
	int nMin, nMax;
	if ( nFirstLine < nLastLine )
	{
		nMin = nFirstLine;
		nMax = nLastLine;
	}
	else
	{
		nMin = nLastLine;
		nMax = nFirstLine;
	}
	for ( int i = nMin; i <= nMax; i++ )
		SelectLine( i, true, nRGBColor );
}

void CTextManager::UnselectAllLines()
{
	map< int, bool >::iterator itLine = m_mSelectedLines.begin();
	for ( itLine; itLine != m_mSelectedLines.end(); itLine++ )
	{
		if ( itLine->second )
		{
			itLine->second = false;
			int nScreenLine = TextToScreen( itLine->first );
			RECT rect;
			GetClientRect( m_hWnd, &rect );
			rect.top = nScreenLine * m_vFont.at( m_nCurrentFont ).GetHeight() ;
			rect.bottom = ( nScreenLine + 1 ) * m_vFont.at( m_nCurrentFont ).GetHeight() ;
			InvalidateRect( m_hWnd, &rect, TRUE );
		}
	}
}

bool CTextManager::IsSelectedLine( int nTextLine )
{
	map< int, bool >::iterator itLine = m_mSelectedLines.find( nTextLine );
	if( itLine != m_mSelectedLines.end() )
		return itLine->second;
	return false;
}

void CTextManager::ScrollText( int x, int y )
{
	int xorg = m_nTextx;
	int yorg = m_nTexty;
	if ( x != 0 )
		xorg += s_nCharWidth * x;
	if ( y != 0 )
		yorg += m_vFont.at( m_nCurrentFont ).GetHeight()  * y;
	SetTextPosition( xorg, yorg );
}

void CTextManager::ScrollTextAbsolute( bool bVert, int nValue )
{
	int x = m_nTextx;
	int y = m_nTexty;
	int nPixelValue = - static_cast< int >( m_vFont.at( m_nCurrentFont ).GetHeight()  ) * nValue;
	if ( bVert )
		y = nPixelValue;
	else
		x = nPixelValue;
	SetTextPosition( x, y );
 }

int CTextManager::GetStringLineCount( string s )
{
	int nIndex = -1;
	int nLnCount = 0;
	do
	{
		nIndex = s.find( "\n", nIndex + 1 );
		if ( nIndex != -1 )
			nLnCount++;
	}
	while( nIndex != -1 );
	return nLnCount;
}