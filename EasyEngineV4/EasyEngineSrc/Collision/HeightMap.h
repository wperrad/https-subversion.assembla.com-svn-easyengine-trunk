#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

// stl
#include <string>
#include <vector>


class ILoaderManager;
class IFileSystem;
class IBox;
class CVector;
class IGeometryManager;

using namespace std;

class CHeightMap
{
	vector< unsigned char >	m_vPixels;
	int		m_nWidth;
	int		m_nHeight;
	IBox*	m_pModelBox;

	float	GetHeight( const CVector& p );
	void	ModelToMap( int xModel, int yModel, int& xMap, int& yMap );
	void	ModelToMap( int xModel, int zModel, float& xMap, float& yMap );
	int		m_nPrecision;

	static int	GetPixelNumberFromCoord( int nWidth, int nRow, int nColumn, int bpp );

public:
	CHeightMap();
	CHeightMap( string sFileName, ILoaderManager& oLoaderManager );
	CHeightMap( string sFileName, ILoaderManager& oLoaderManager, const IBox& bbox, IGeometryManager& oGeometryManager );
	void	Load( string sFileName, ILoaderManager& oLoaderManager );
	void	GetPixel( int x, int y, CVector& pixel );
	float	GetHeight( float xModel, float zModel );
	void	SetPrecision( int nPrecision );
	static	void	ExtractHeightMapFromTexture( string sFileName, ILoaderManager& oLoaderManager, IFileSystem* pFileSystem, string sOutFileName );
};

#endif // HEIGHTMAP_H