#include "HeightMap.h"
#include "ILoader.h"
#include "IFileSystem.h"
#include "IGeometry.h"

extern IGeometryManager* m_pGeometryManager;

CHeightMap::CHeightMap():
m_nPrecision(3)
{
}

CHeightMap::CHeightMap( string sFileName, ILoaderManager& oLoaderManager )
{
	Load( sFileName, oLoaderManager );
}

CHeightMap::CHeightMap( string sFileName, ILoaderManager& oLoaderManager, const IBox& bbox, IGeometryManager& oGeometryManager )
{
	Load( sFileName, oLoaderManager );
	m_pModelBox = oGeometryManager.CreateBox();
	*m_pModelBox = bbox;
	bbox.GetDimension();
}

void CHeightMap::SetPrecision( int nPrecision )
{
	m_nPrecision = nPrecision;
}

void CHeightMap::Load( string sFileName, ILoaderManager& oLoaderManager )
{
	ILoader::CTextureInfos ti;
	oLoaderManager.Load( sFileName, ti );
	m_nWidth = ti.m_nWidth;
	m_nHeight = ti.m_nHeight;
	m_vPixels.swap( ti.m_vTexels );
}

void CHeightMap::GetPixel( int x, int y, CVector& pixel )
{
	int nPixelIndice = 3 * x + y * m_nWidth * 3;
	pixel.m_x = m_vPixels[ nPixelIndice ];
	pixel.m_y = m_vPixels[ nPixelIndice + 1 ];
	pixel.m_z = m_vPixels[ nPixelIndice + 2 ];
}

float CHeightMap::GetHeight( const CVector& p )
{
	float ret = 0.f;
	if( m_nPrecision == 1 )
		ret = (float)p.m_x;
	else if( m_nPrecision == 3 )
	{
		int nColor = p.m_z * pow( 2., 16 ) + p.m_y * pow(2., 8.) + p.m_x;
		ret = (float)nColor / pow( 2., 16. );
	}
	return ret;
}

void CHeightMap::ModelToMap( int xModel, int zModel, int& xMap, int& yMap )
{
	xMap = - m_nWidth * ( xModel - m_pModelBox->GetMinPoint().m_x - m_pModelBox->GetDimension().m_x ) / m_pModelBox->GetDimension().m_x;
	yMap = - m_nHeight * ( zModel - m_pModelBox->GetMinPoint().m_z - m_pModelBox->GetDimension().m_z ) / m_pModelBox->GetDimension().m_z;
}


void CHeightMap::ModelToMap( int xModel, int zModel, float& xMap, float& yMap )
{
	xMap = - m_nWidth * ( xModel - m_pModelBox->GetMinPoint().m_x - m_pModelBox->GetDimension().m_x ) / m_pModelBox->GetDimension().m_x;
	yMap = - m_nHeight * ( zModel - m_pModelBox->GetMinPoint().m_z - m_pModelBox->GetDimension().m_z ) / m_pModelBox->GetDimension().m_z;
}

float CHeightMap::GetHeight( float xModel, float zModel )
{
	float fxMap, fyMap;
	ModelToMap( xModel, zModel, fxMap, fyMap );

	if( fxMap > 0 && fxMap < m_nWidth && fyMap > 0 && fyMap < m_nHeight )
	{
		CVector p00, p01, p10, p11;

		int xMap = (int)fxMap;
		int yMap = (int)fyMap;

		float dx = fxMap - (float)xMap;
		float dy = fyMap - (float)yMap;

		int xMapInc = xMap + 1;
		int yMapInc = yMap + 1;
		if( xMapInc == m_nWidth )
			xMapInc--;
		if( yMapInc == m_nHeight )
			yMapInc--;
		GetPixel( xMap, yMap, p00 );
		GetPixel( xMapInc, yMap, p10 );
		GetPixel( xMap, yMapInc, p01 );
		GetPixel( xMapInc, yMapInc, p11 );
		
		float h00 = GetHeight( p00 );
		float h10 = GetHeight( p10 );
		float h01 = GetHeight( p01 );
		float h11 = GetHeight( p11 );

		float h = (1-dx)*(1-dy)*h00 + (1-dx)*dy*h01 + dx*(1-dy)*h10 + dx*dy*h11;

		float fRet = h * m_pModelBox->GetDimension().m_y / 255.f + m_pModelBox->GetMinPoint().m_y;
		
		return fRet;
	}
	else
		return -100000000.f;
}

void CHeightMap::ExtractHeightMapFromTexture( string sFileName, ILoaderManager& oLoaderManager, IFileSystem* pFileSystem, string sOutFileName )
{
	ILoader::CTextureInfos ti;
	oLoaderManager.Load( sFileName, ti );
	int bpp = 3;
	int nMinX = 0;
	int nMinY = 0;
	int nMaxX = 0;
	int nMaxY = 0;
	bool bFoundGray = false;
	for( int i = 0; i < ti.m_nHeight; i++ )
	{
		for( int j = 0; j < ti.m_nWidth; j++ )
		{
			int nIndice = GetPixelNumberFromCoord( ti.m_nWidth, i, j, bpp ); // i * ti.m_nWidth * bpp + j * bpp;
			int r = ti.m_vTexels[ nIndice ];
			int g = ti.m_vTexels[ nIndice + 1 ];
			int b = ti.m_vTexels[ nIndice + 2 ];
			bool bGray = ( r == g && r == b );
			if( bGray )
			{
				if( !bFoundGray )
				{
					nMinX = j;
					nMinY = i;
					bFoundGray = true;
				}
				else
				{
					if( j > nMaxX )
						nMaxX = j;
					if( i > nMaxY )
						nMaxY = i;
				}
			}
		}
	}
	vector< unsigned char > vPixels;
	int nRestPixel = ( nMaxX - nMinX + 1 ) % 4;
	int nPixelAdd = min( nRestPixel, 4 - nRestPixel );
	if( nPixelAdd < 2 )
		nPixelAdd = -nPixelAdd;
	for( int i = nMinY; i <= nMaxY; i++ )
	{
		for( int j = nMinX; j <= nMaxX; j++ )
		{
			int nIndice = GetPixelNumberFromCoord( ti.m_nWidth, i, j, bpp );
			for( int k = 0; k < bpp; k++ )
				vPixels.push_back( ti.m_vTexels[ nIndice + k ] );
		}
		if( nPixelAdd > 0 )
		{
			for( int k = 0; k < nPixelAdd * bpp; k++ )
				vPixels.push_back( 0 );
		}
		else if( nPixelAdd < 0 )
		{
			for( int k = 0; k < - nPixelAdd * bpp; k++ )
				vPixels.pop_back();
		}
	}
	int nNewWidth = nMaxX + 1 - nMinX + nPixelAdd;
	string sDir;
	if( pFileSystem )
		pFileSystem->GetRootDirectory( sDir );
	string sOutPath = sDir + "\\" + sOutFileName;
	oLoaderManager.CreateBMPFromData( vPixels, nNewWidth, nMaxY + 1 - nMinY, bpp * 8, sOutPath );
}

int	CHeightMap::GetPixelNumberFromCoord( int nWidth, int nRow, int nColumn, int bpp )
{
	return bpp * ( nRow * nWidth + nColumn );
}