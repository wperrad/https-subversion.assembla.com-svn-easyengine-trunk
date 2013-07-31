#include <windows.h>
#include <string>
#include <vector>

using namespace std;

struct CPoint
{
	float m_x;
	float m_y;
	CPoint(int x, int y):m_x(x), m_y(y){}
};

void CreateFontBitmap( string FontName, int nSize, vector< unsigned char >& vData, vector< CPoint>& vCharPos );
//void CreateBMPFromData( const vector< unsigned char >& vData, int nWidth, int nHeight, int nBitPerPixel, string sFileName );

//int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
//{
//	vector< unsigned char > vData;
//	int nSize = 256;
//	vector< CPoint > vCharPos;
//	CreateFontBitmap( "Arial", nSize, vData, vCharPos );
//	CreateBMPFromData( vData, nSize, nSize, 24, "font2.bmp" );
//	return TRUE;
//}

//void CreateBMPFromData( const vector< unsigned char >& vData, int nWidth, int nHeight, int nBitPerPixel, string sFileName )
//{
//	// file header
//	FILE* pFile = fopen( sFileName.c_str(), "w" );
//	char* sMagic = "BM";
//	fwrite( sMagic, 1, 2, pFile );	
//	int nSize = 0;
//	fwrite( &nSize, 4, 1, pFile );
//	int id = 0;
//	fwrite( &id, 1, 2, pFile );
//	fwrite( &id, 1, 2, pFile );
//	int DataAdress = 0;
//	int nDataAdressOffset = ftell(pFile);
//	fwrite( &DataAdress, 4, 1, pFile );
//
//	// image header
//	int nHeaderSize = 40;
//	fwrite( &nHeaderSize, 4, 1, pFile );
//	fwrite( &nWidth, 4, 1, pFile );
//	fwrite( &nHeight, 4, 1, pFile );
//	int nPlanes = 1;
//	fwrite( &nPlanes, 2, 1, pFile );
//	fwrite( &nBitPerPixel, 2, 1, pFile );
//	int iData = 0;
//	fwrite( &iData, 4, 1, pFile );
//	fwrite( &iData, 4, 1, pFile );
//	fwrite( &iData, 4, 1, pFile );
//	fwrite( &iData, 4, 1, pFile );
//	fwrite( &iData, 4, 1, pFile );
//	fwrite( &iData, 4, 1, pFile );
//
//	int nDataAdress = ftell(pFile);
//
//	// Pixel data
//	fwrite( &vData[ 0 ], sizeof( unsigned char ), vData.size(), pFile );
//	nSize = ftell(pFile);
//
//	// complete
//	fseek( pFile, 2, SEEK_SET );
//	fwrite( &nSize, 4, 1, pFile );
//	fseek( pFile, nDataAdressOffset, SEEK_SET );
//	fwrite( &nDataAdress, 4, 1, pFile );
//	fclose( pFile );
//}

void CreateFontBitmap( string FontName, int nSize, vector< unsigned char >& vData, vector< CPoint >& vCharSize )
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

	// Tableau servant à stocker la taille de chaque caractère
	//CPoint CharSize[ 256 ];
    char Character = 0;
    for (int j = 0; j < 16; ++j)
	{
        for (int i = 0; i < 16; ++i, ++Character)
        {
            // Stockage de la taille du caractère
            SIZE Size;
            GetTextExtentPoint32(hDC, &Character, 1, &Size);
            //CharSize[i + j * 16].m_x = Size.cx;
            //CharSize[i + j * 16].m_y = Size.cy;
			vCharSize.push_back( CPoint( Size.cx, Size.cy ) );

            // Affichage du caractère
            RECT Rect = {i * Quality, j * Quality, (i + 1) * Quality, (j + 1) * Quality};
            DrawText(hDC, &Character, 1, &Rect, DT_LEFT);
        }
	}
	int nDataCount = bi.bmiHeader.biWidth * bi.bmiHeader.biHeight * bi.bmiHeader.biBitCount / 8;
	for( int i = 0; i < nDataCount; i++ )
		vData.push_back( data[ i ] );
}