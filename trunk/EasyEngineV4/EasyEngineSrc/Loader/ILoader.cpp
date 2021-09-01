#include "ILoader.h"

void ILoader::Read( void* pDestBuffer, unsigned int nElementSize, unsigned int nElementCount, FILE* pFile )
{
	int nSizeRead = (int)fread( pDestBuffer, nElementSize, nElementCount, pFile );
	if( nSizeRead == 0 )
	{
		fclose( pFile );
		CBadFileFormat e( "ILoader::Read() : Erreur de lecture du loader" );
		throw e;
	}
	int nOffset = ftell( pFile );
	m_nFileOffset += nElementSize * nElementCount;
	if( m_nFileOffset != nOffset )
	{
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