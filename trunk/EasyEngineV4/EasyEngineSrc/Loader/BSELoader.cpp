#include "BSELoader.h"
#include "IFileSystem.h"

CBSELoader::CBSELoader( IFileSystem& oFileSystem ):
m_oFileSystem( oFileSystem )
{
}

void CBSELoader::WriteString( FILE* pFile, string sString )
{
	int nStringNameSize = sString.size();
	fwrite( &nStringNameSize, sizeof( int ), 1 , pFile );
	if( nStringNameSize > 0 )
		fwrite( sString.c_str(), sizeof( char ), nStringNameSize, pFile );
}

void CBSELoader::WriteObject( CSceneObjInfos* pObjInfos, FILE* pFile )
{
	const CEntityInfos* pEntityInfos = dynamic_cast< const CEntityInfos* >( pObjInfos );
	const CLightEntityInfos* pLightEntityInfos = dynamic_cast< const CLightEntityInfos* >( pObjInfos );
	int nType = -1;
	if( pEntityInfos )
		nType = eEntity;
	else if( pLightEntityInfos )
		nType = eLight;		
	fwrite( &nType, sizeof( int ), 1, pFile );
	WriteString( pFile, pObjInfos->m_sRessourceName );
	WriteString( pFile, pObjInfos->m_sRessourceFileName );
	float pMat[16 ];
	pObjInfos->m_oXForm.Get( pMat );
	fwrite( pMat, sizeof( float ), 16, pFile );
	WriteString( pFile, pObjInfos->m_sParentName );
	fwrite( &pObjInfos->m_nParentBoneID, sizeof( int ), 1, pFile );
	if( pEntityInfos )
	{
		WriteString( pFile, pEntityInfos->m_sTypeName );
		WriteString( pFile, pEntityInfos->m_sAnimationFileName );
		fwrite( &pEntityInfos->m_fWeight, sizeof( float ), 1, pFile );
		int nChildCount = pEntityInfos->m_vSubEntityInfos.size();
		fwrite( &nChildCount, sizeof( int ), 1, pFile );
		for( int iChild = 0; iChild < pEntityInfos->m_vSubEntityInfos.size(); iChild++ )
			WriteObject( pEntityInfos->m_vSubEntityInfos[ iChild ], pFile );
	}
	else if( pLightEntityInfos )
	{
		int nLightType = ( int )pLightEntityInfos->m_eType;
		fwrite( &nLightType, sizeof( int ), 1, pFile );
		fwrite( &pLightEntityInfos->m_fIntensity, sizeof( float ), 1, pFile );
		fwrite( &pLightEntityInfos->m_oColor.m_x, sizeof( float ), 1, pFile );
		fwrite( &pLightEntityInfos->m_oColor.m_y, sizeof( float ), 1, pFile );
		fwrite( &pLightEntityInfos->m_oColor.m_z, sizeof( float ), 1, pFile );
		fwrite( &pLightEntityInfos->m_oColor.m_w, sizeof( float ), 1, pFile );
	}
}

void CBSELoader::Export( string sFileName, const IRessourceInfos& ri )
{
	const CSceneInfos* pInfos = static_cast< const CSceneInfos* >( &ri );
	FILE* pFile = m_oFileSystem.OpenFile( sFileName, "wb" );
	WriteString( pFile, pInfos->m_sSceneFileName );
	WriteString( pFile, pInfos->m_sName );
	int nObjectCount = ( int )pInfos->m_vObject.size();
	fwrite( &nObjectCount, sizeof( int ), 1, pFile );
	for( unsigned int i = 0; i < pInfos->m_vObject.size(); i++ )
		WriteObject( pInfos->m_vObject.at( i ), pFile );
	WriteString( pFile, pInfos->m_sPersoName );
	fclose( pFile );
}

void CBSELoader::ReadString( FILE* pFile, string& sString )
{
	int nStringSize = 0;
	Read( &nStringSize, sizeof( int ), 1, pFile );
	if( nStringSize > 0 )
	{
		sString.resize( nStringSize );
		Read( &sString[ 0 ], sizeof( char ), nStringSize, pFile );
	}
}

ILoader::CSceneObjInfos* CBSELoader::ReadObject( FILE* pFile )
{
	CSceneObjInfos* pObjInfos = NULL;
	int nType;
	Read( &nType, sizeof( int ), 1, pFile );
	TObjScene eType = ( TObjScene )nType;
	switch( eType )
	{
	case TObjScene::eEntity:
		pObjInfos = new CEntityInfos;
		break;
	case TObjScene::eLight:
		pObjInfos = new CLightEntityInfos;
		break;
	default:
		{
			CBadFileFormat e("");
			throw e;
		}
	}

	ReadString( pFile, pObjInfos->m_sRessourceName );
	ReadString( pFile, pObjInfos->m_sRessourceFileName );
	float pMat[ 16 ];
	Read( pMat, sizeof( float ), 16, pFile );
	pObjInfos->m_oXForm = CMatrix ( pMat );
	ReadString( pFile, pObjInfos->m_sParentName );
	Read( &pObjInfos->m_nParentBoneID, sizeof( int ), 1, pFile );
	if( eType == eEntity )
	{
		CEntityInfos* pEntityInfos = static_cast< CEntityInfos* >( pObjInfos );
		ReadString( pFile, pEntityInfos->m_sTypeName );
		ReadString( pFile, pEntityInfos->m_sAnimationFileName );
		Read( &pEntityInfos->m_fWeight, sizeof( float ), 1, pFile );
		int nChildCount = 0;
		Read( &nChildCount, sizeof( int ), 1, pFile );
		for( int iChild = 0; iChild < nChildCount; iChild++ )
		{
			CEntityInfos* pChildInfos = static_cast< CEntityInfos* >( ReadObject( pFile ) );
			pEntityInfos->m_vSubEntityInfos.push_back( pChildInfos );
		}
	}
	else if( eType == eLight )
	{
		CLightEntityInfos* pLightEntityInfos = static_cast< CLightEntityInfos* >( pObjInfos );
		int nLightType;
		Read( &nLightType, sizeof( int ), 1, pFile );
		pLightEntityInfos->m_eType = (ILoader::CLightInfos::TLight) nLightType;
		Read( &pLightEntityInfos->m_fIntensity, sizeof( float ), 1, pFile );
		Read( &pLightEntityInfos->m_oColor.m_x, sizeof( float ), 1, pFile );
		Read( &pLightEntityInfos->m_oColor.m_y, sizeof( float ), 1, pFile );
		Read( &pLightEntityInfos->m_oColor.m_z, sizeof( float ), 1, pFile );
		Read( &pLightEntityInfos->m_oColor.m_w, sizeof( float ), 1, pFile );
	}
	return pObjInfos;
}

void CBSELoader::Load( string sFileName, IRessourceInfos& si, IFileSystem& )
{
	m_nFileOffset = 0;
	CSceneInfos* pSceneInfos = static_cast< CSceneInfos* >( &si );
	FILE* pFile = m_oFileSystem.OpenFile( sFileName, "rb" );
	if( !pFile )
	{
		string sMessage = string( "Erreur : fichier \"" ) + sFileName + "\" manquant";
		CFileNotFoundException e( sMessage );
		e.m_sFileName = sFileName;
		throw e;
	}

	ReadString( pFile, pSceneInfos->m_sSceneFileName );
	ReadString( pFile, pSceneInfos->m_sName );
	int nObjectCount;
	Read( &nObjectCount, sizeof( int ), 1, pFile );
	for( int i = 0; i < nObjectCount; i++ )
	{
		CSceneObjInfos* pObjInfos = ReadObject( pFile );
		pSceneInfos->m_vObject.push_back( pObjInfos );
	}
	ReadString( pFile, pSceneInfos->m_sPersoName );
	fclose( pFile );
}