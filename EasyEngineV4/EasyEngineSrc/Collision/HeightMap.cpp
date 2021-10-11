#include "HeightMap.h"
#include "ILoader.h"
#include "IFileSystem.h"
#include "IGeometry.h"
#include "Interface.h"

extern IGeometryManager* m_pGeometryManager;

CHeightMap::CHeightMap():
m_nPrecision(3)
{
}

CHeightMap::CHeightMap(EEInterface& oInterface, string sFileName,  const IBox& bbox) :
	m_nPrecision(3),
	m_pLoaderManager(static_cast<ILoaderManager*>(oInterface.GetPlugin("LoaderManager"))),
	m_pGeometryManager(static_cast<IGeometryManager*>(oInterface.GetPlugin("GeometryManager")))
{
	Load(sFileName);
	m_pModelBox = m_pGeometryManager->CreateBox();
	*m_pModelBox = bbox;
	bbox.GetDimension();
}

CHeightMap::~CHeightMap()
{

}

void CHeightMap::SetPrecision( int nPrecision )
{
	m_nPrecision = nPrecision;
}

IBox* CHeightMap::GetModelBBox()
{
	return m_pModelBox;
}

void CHeightMap::AdaptGroundMapToModel(const CMatrix& modelTM, const CVector modelDim, float groundAdaptationHeight)
{
	float xMargin = modelDim.m_y;
	float zMargin = modelDim.m_y;
	float x0, z0, x1, z1;
	MapToModel(0, 0, x0, z0);
	MapToModel(1, 1, x1, z1);
	float modelUnit = abs(x1 - x0);
	float bias = modelUnit / 2.f;
	modelUnit -= bias;
	CVector modelPos = modelTM.GetPosition();
	int xModel = -modelDim.m_x / 2.f - xMargin;
	int zModel = 0;
	float hMin = 99999999.f;
	while (xModel < modelDim.m_x / 2.f + xMargin) {
		zModel = -modelDim.m_z / 2.f - zMargin;
		while (zModel < (modelDim.m_z / 2.f + zMargin) ) {
			CVector P((float)xModel, 0, (float)zModel);
			CVector PTransform = modelTM * P;
			float xMap = 0, yMap = 0;
			ModelToMap(PTransform.m_x, PTransform.m_z, xMap, yMap);
			if (xMap < 0.f) 
				xMap = 0.f;
			if (yMap < 0.f) 
				yMap = 0.f;
			if (xMap > m_nWidth) 
				xMap = m_nWidth;
			if (yMap > m_nHeight) 
				yMap = m_nHeight;
			CVector pixel;
			GetPixel(xMap, yMap, pixel);
			float height = GetHeight(pixel);
			if (hMin > height)
				hMin = height;
			zModel += modelUnit;
		}
		xModel += modelUnit;
	}

	hMin += groundAdaptationHeight;
	if (hMin < 0.f) hMin = 0.f;
	if (hMin > 255.f) hMin = 255.f;

	xModel = -modelDim.m_x / 2.f - xMargin;
	while (xModel < (modelDim.m_x / 2.f + xMargin)) {
		zModel = -modelDim.m_z / 2.f - zMargin;
		while (zModel < (modelDim.m_z / 2.f + zMargin)) {
			CVector P((float)xModel, 0, (float)zModel);
			CVector PTransform = modelTM * P;
			float xMap = 0, yMap = 0;
			ModelToMap(PTransform.m_x, PTransform.m_z, xMap, yMap);
			if (xMap < 0.f)
				xMap = 0.f;
			if (yMap < 0.f)
				yMap = 0.f;
			if (xMap > m_nWidth)
				xMap = m_nWidth;
			if (yMap > m_nHeight)
				yMap = m_nHeight;
			SetPixelValue(xMap, yMap, hMin);
			zModel += modelUnit;
		}
		xModel += modelUnit;
	}
}

void CHeightMap::RestoreHeightMap(const CMatrix& modelTM, const CVector& modelDim, string originalHeightMap)
{
	ILoader::CTextureInfos ti;
	ti.m_bFlip = true;
	m_pLoaderManager->Load(originalHeightMap, ti);
	ti.m_vTexels;
	
	float xMargin = modelDim.m_y;
	float zMargin = modelDim.m_y;
	float x0, z0, x1, z1;
	MapToModel(0, 0, x0, z0);
	MapToModel(1, 1, x1, z1);
	float modelUnit = abs(x1 - x0);
	float bias = modelUnit / 10.f;
	modelUnit -= bias;
	CVector modelPos = modelTM.GetPosition();
	
	int xModel = -modelDim.m_x / 2.f - xMargin;
	int zModel = 0;
	float hMin = 99999999.f;
	while (xModel < modelDim.m_x / 2.f + xMargin) {
		zModel = -modelDim.m_z / 2.f - zMargin;
		while (zModel < (modelDim.m_z / 2.f + zMargin)) {
			CVector P((float)xModel, 0, (float)zModel);
			CVector PTransform = modelTM * P;
			float xMap = 0, yMap = 0;
			ModelToMap(PTransform.m_x, PTransform.m_z, xMap, yMap);
			/*
			if (xMap < 0.f)
				xMap = 0.f;
			if (yMap < 0.f)
				yMap = 0.f;
			if (xMap > m_nWidth)
				xMap = m_nWidth;
			if (yMap > m_nHeight)
				yMap = m_nHeight;*/
			CVector pixel;
			GetPixel(ti, xMap, yMap, pixel);
			float height = GetHeight(pixel);
			SetPixelValue(xMap, yMap, height);
			zModel += modelUnit;
		}
		xModel += modelUnit;
	}
}

void CHeightMap::GetFileName(string& fileName)
{
	fileName = m_sFileName;
}

void CHeightMap::Load(string sFileName)
{
	ILoader::CTextureInfos ti;
	ti.m_bFlip = true;
	m_pLoaderManager->Load( sFileName, ti );

	double l = log2(ti.m_nWidth);
	int il = (int)l;
	if (l - il != 0) {
		exception e("Erreur : La texture utilisee pour la height map doit avoir une taille en puissance de 2");
		throw e;
	}

	m_nWidth = ti.m_nWidth;
	m_nHeight = ti.m_nHeight;
	m_vPixels.swap( ti.m_vTexels );
	m_sFileName = sFileName;
}

void CHeightMap::Save(string sFileName)
{
	ILoader::CTextureInfos ti;
	ti.m_bFlip = true;
	ti.m_nWidth = m_nWidth;
	ti.m_nHeight = m_nHeight;
	ti.m_vTexels = m_vPixels;
	ti.m_ePixelFormat = ILoader::eRGB;
	m_pLoaderManager->Export(sFileName, ti);
}

void CHeightMap::GetPixel( int x, int y, CVector& pixel )
{
	int nPixelIndice = 3 * x + y * m_nWidth * 3;
	pixel.m_x = m_vPixels[ nPixelIndice ];
	pixel.m_y = m_vPixels[ nPixelIndice + 1 ];
	pixel.m_z = m_vPixels[ nPixelIndice + 2 ];
}

void CHeightMap::GetPixel(const ILoader::CTextureInfos& ti, int x, int y, CVector& pixel)
{
	int nPixelIndice = 3 * x + y * ti.m_nWidth * 3;
	pixel.m_x = ti.m_vTexels[nPixelIndice];
	pixel.m_y = ti.m_vTexels[nPixelIndice + 1];
	pixel.m_z = ti.m_vTexels[nPixelIndice + 2];
}

void CHeightMap::SetPixelValue(int x, int y, float value)
{
	int nPixelIndice = 3 * x + y * m_nWidth * 3;
	m_vPixels[nPixelIndice] = value;
	m_vPixels[nPixelIndice + 1] = value;
	m_vPixels[nPixelIndice + 2] = value;
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

void CHeightMap::MapToModel(int xMap, int yMap, float& xModel, float& zModel)
{
	float dimx = m_pModelBox->GetDimension().m_x;
	float dimz = m_pModelBox->GetDimension().m_z;
	float h = m_pModelBox->GetDimension().m_y;

	xModel = dimx - dimx / 2.f - (m_nWidth - xMap) * dimx / m_nWidth;
	zModel = dimz - yMap * dimz / m_nHeight - dimz / 2.f;
}


void CHeightMap::ModelToMap( int xModel, int zModel, int& xMap, int& yMap )
{
	xMap = - m_nWidth * (( xModel - m_pModelBox->GetMinPoint().m_x - m_pModelBox->GetDimension().m_x ) / m_pModelBox->GetDimension().m_x) - 1;
	yMap = - m_nHeight * (( zModel - m_pModelBox->GetMinPoint().m_z - m_pModelBox->GetDimension().m_z ) / m_pModelBox->GetDimension().m_z) - 1;
}


void CHeightMap::ModelToMap( int xModel, int zModel, float& xMap, float& yMap )
{
	float dimx = m_pModelBox->GetDimension().m_x;
	float dimz = m_pModelBox->GetDimension().m_z;
	float h = m_pModelBox->GetDimension().m_y;

	xMap = ((m_nWidth - 1) / 2.f) * (1 + 2 * xModel / dimx );
	yMap = ( (1 - m_nWidth) / 2.f) * ( (2 * zModel  / dimz) - 1);
}

float CHeightMap::GetHeight( float xModel, float zModel )
{
	float fxMap, fyMap;
	ModelToMap( xModel, zModel, fxMap, fyMap );

	if( fxMap >= 0 && fxMap < m_nWidth && fyMap >= 0 && fyMap < m_nHeight )
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
		//h = h00;

		float fRet = GetHeightFromPixelValue(h);
		
		return fRet;
	}
	else
		return -100000000.f;
}

float CHeightMap::GetHeightFromPixelValue(float pixelValue)
{
	return pixelValue * m_pModelBox->GetDimension().m_y / 255.f + m_pModelBox->GetMinPoint().m_y;
}

float CHeightMap::GetPixelValueFromHeight(float height)
{
	return 255.f * (height - m_pModelBox->GetMinPoint().m_y ) / m_pModelBox->GetDimension().m_y;
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
		pFileSystem->GetLastDirectory( sDir );
	string sOutPath = sDir + "\\" + sOutFileName;
	oLoaderManager.CreateBMPFromData( vPixels, nNewWidth, nMaxY + 1 - nMinY, bpp * 8, sOutPath );
}

int	CHeightMap::GetPixelNumberFromCoord( int nWidth, int nRow, int nColumn, int bpp )
{
	return bpp * ( nRow * nWidth + nColumn );
}