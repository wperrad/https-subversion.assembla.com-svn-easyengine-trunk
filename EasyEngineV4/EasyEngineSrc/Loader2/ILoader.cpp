#include "ILoader.h"

void ILoader::Read( void* pDestBuffer, unsigned int nElementSize, unsigned int nElementCount, FILE* pFile )
{
	int nSizeRead = (int)fread( pDestBuffer, nElementSize, nElementCount, pFile );
	if( nSizeRead == 0 )
	{
		//MessageBox( NULL, "ILoader::Read() : Erreur de lecture du loader", "Erreur de lecture", MB_ICONERROR );
		fclose( pFile );
		CBadFileFormat e( "ILoader::Read() : Erreur de lecture du loader" );
		throw e;
	}
	int nOffset = ftell( pFile );
	m_nFileOffset += nElementSize * nElementCount;
	if( m_nFileOffset != nOffset )
	{
		//MessageBox( NULL, "ILoader::Read() : Problème d'offset fichier", "", MB_ICONERROR );
		fclose( pFile );
		CBadFileFormat e( "ILoader::Read() : Problème d'offset fichier" );
		throw e;
	}
}

void ILoader::SetAsciiExportPrecision( int nPrecision )
{
	m_nAsciiExportPrecision = nPrecision;
}

int ILoader::ReadInt( FILE* pFile )
{
	int n = 0;
	Read( &n, sizeof(int), 1, pFile );
	return n;
}