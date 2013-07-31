#include "BKELoader.h"
#include "IFileSystem.h"
#include "../Utils2/Chunk.h"
#include "../Utils2/StringUtils.h"
#include "../Utils2/DebugTool.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Exception.h"
#include "Utils/IStorage.h"

#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

CBKELoader::CBKELoader( IFileSystem& oFileSystem ):
m_oFileSystem( oFileSystem )
{
}

void CBKELoader::Load( string sFileName, ILoader::IRessourceInfos& ri, IFileSystem& oFileSystem )
{
	CAnimationInfos* pAnimInfos = dynamic_cast< CAnimationInfos* >( &ri );
	if( !pAnimInfos )
	{
		ostringstream oss;
		oss << "\"" << sFileName << "\" : bad Type format";
		CBadTypeException e( oss.str() );
		throw e;
	}
	m_nFileOffset = 0;
	string sRootDirectory;
	oFileSystem.GetRootDirectory( sRootDirectory );
	CBinaryFileStorage fs;
	if ( !fs.OpenFile( sRootDirectory + "\\" + sFileName, CBinaryFileStorage::eRead ) )
	{
		CFileNotFoundException e( "Fichier introuvable" );
		e.m_sFileName = sFileName;
		throw e;
	}
	string sName;
	int nExtPos = sFileName.find_last_of( "." );
	pAnimInfos->m_sName = sFileName.substr( 0, nExtPos );
	fs >> pAnimInfos->m_nStartTime >> pAnimInfos->m_nEndTime >> pAnimInfos->m_nBoneCount;

	int iKeyIndex = 0;
	for ( int iBone = 0; iBone < pAnimInfos->m_nBoneCount; iBone++ )
	{
		unsigned int nBoneID, nKeyCount;
		fs >> nBoneID >> nKeyCount;
		pAnimInfos->m_vBonesIDArray.push_back( nBoneID );
		pAnimInfos->m_vKeyCountArray.push_back( nKeyCount );
		for ( unsigned int iKey = 0; iKey < nKeyCount; iKey++ )
		{
			unsigned int nTimeValue;
			int nKeyType;
			fs >> nTimeValue >> nKeyType;
			pAnimInfos->m_vTimeValueArray.push_back( nTimeValue );
			pAnimInfos->m_vKeyTypeArray.push_back( nKeyType );

			CMatrix mLocal, mWorld;
			fs >> mLocal >> mWorld;
			pAnimInfos->m_vLocalTMArray.push_back( mLocal );
			pAnimInfos->m_vWorldTMArray.push_back( mWorld );

			float fAngle;
			fs >> fAngle;
			pAnimInfos->m_vAngleArray.push_back( fAngle );
			float fComposant;
			for ( int iComposant = 0; iComposant < 3; iComposant++ )
			{
				fs >> fComposant;
				pAnimInfos->m_vAxisArray.push_back( fComposant );
			}
			CQuaternion q;
			fs >> q;
			pAnimInfos->m_vLocalQuatArray.push_back( q );
			iKeyIndex++;
		}
	}
	fs.CloseFile();
}

void CBKELoader::Export( const std::string& sFileName, const CChunk& oChunk)
{
	string sExt;
	CStringUtils::GetExtension( sFileName, sExt );
	if( sExt == "txt" )
		ExportAscii( sFileName, oChunk );
}

void CBKELoader::ExportAscii( const std::string& sFileName, const CChunk& oChunk )
{
	int* pBoneCount =  static_cast< int* >( oChunk.Get( "BoneCount" ) );
	vector< unsigned int >* pBonesIDArray = static_cast< vector< unsigned int >* >( oChunk.Get( "BonesIDArray" ) );
	vector< unsigned int >* pKeyCountArray = static_cast< vector< unsigned int >* >( oChunk.Get( "KeyCountArray" ) );
	vector< unsigned int >* pTimeValueArray = static_cast< vector< unsigned int >* >( oChunk.Get( "TimeValueArray" ) );
	vector< float >* pMatrixArray = static_cast< vector< float >* >( oChunk.Get( "MatrixArray" ) );
	vector< float >* pAngleArray = static_cast< vector< float >* >( oChunk.Get( "AngleArray" ) );
	vector< float >* pAxisArray = static_cast< vector< float >* >( oChunk.Get( "AxisArray" ) );
	vector< CQuaternion >* pLocalQuatArray = static_cast< vector< CQuaternion >* >( oChunk.Get( "LocalQuatArray" ) );

	CDebugTool dt;
	dt.SetNumberWidth( 15 );
	dt.SetNumberPrecision( m_nAsciiExportPrecision );

	ostringstream oss;
	int i = 0;
	for ( int iBone = 0; iBone < *pBoneCount; iBone++ )
	{
		oss << "Bone " << pBonesIDArray->at( iBone ) << " : \n";
		for ( unsigned int iKey = 0; iKey < pKeyCountArray->at( iBone ); iKey++ )
		{
			oss << "\n\n\tclé " << iKey << " : ";
			oss << "\n\t\tTime value = " << pTimeValueArray->at( i );
			string sMatrix;
			CMatrix m( &pMatrixArray->at(16 * i) );
			//SerializeMatrix( m, sMatrix );
			dt.SerializeMatrix( m, 0.f, sMatrix );
			oss << "\n\t\tMatrice : \n" << sMatrix;
			oss << "\t\tAngle  = " << pAngleArray->at( i );
			oss << "\n\t\tAxe = ( " << pAxisArray->at( 3 * i ) << " , " << pAxisArray->at( 3 * i + 1 ) << " , " << pAxisArray->at( 3 * i + 2 ) << ")";
			oss << "\n\t\tLocal quaternion : Rotation = ( " << pLocalQuatArray->at( i ).m_x << " , " << pLocalQuatArray->at( i ).m_y << " , " << pLocalQuatArray->at( i ).m_z << " , " << pLocalQuatArray->at( i ).m_w <<  " )";
			oss << " , Position = ( " << pLocalQuatArray->at( i ).m_vPosition.m_x << " , " << pLocalQuatArray->at( i ).m_vPosition.m_y << " , " << pLocalQuatArray->at( i ).m_vPosition.m_z << " , " << pLocalQuatArray->at( i ).m_vPosition.m_w << " )";
			i++;
		}
		oss << "\n\n";
	}
	FILE* pFile = m_oFileSystem.OpenFile( sFileName, "w" );
	fwrite( oss.str().c_str(), sizeof( char ), oss.str().size(), pFile );
	fclose( pFile );
}


void CBKELoader::ExportAscii( const std::string& sFileName, const ILoader::CAnimationInfos& oInfos )
{
	ostringstream oss;
	int i = 0;
	CDebugTool dt;
	dt.SetNumberPrecision( m_nAsciiExportPrecision );
	dt.SetNumberWidth( 15 );
	
	for ( int iBone = 0; iBone < oInfos.m_nBoneCount; iBone++ )
	{
		oss << "Bone " << oInfos.m_vBonesIDArray.at( iBone ) << " : \n";
		for ( unsigned int iKey = 0; iKey < oInfos.m_vKeyCountArray.at( iBone ); iKey++ )
		{
			oss << "\n\n\tclé " << iKey << " : ";
			oss << "\n\t\tTime value = " << setprecision( m_nAsciiExportPrecision ) << oInfos.m_vTimeValueArray.at( i );
			string sMatrix;
			const CMatrix& m = oInfos.m_vLocalTMArray.at( i );
			dt.SerializeMatrix( m, 0.f, sMatrix );
			oss << "\n\t\tMatrice : \n" << sMatrix;
			oss << "\t\tAngle  = " << setprecision( m_nAsciiExportPrecision ) << oInfos.m_vAngleArray.at( i );
			oss << "\n\t\tAxe = ( " << setprecision( m_nAsciiExportPrecision ) << oInfos.m_vAxisArray.at( 3 * i ) << " , " << oInfos.m_vAxisArray.at( 3 * i + 1 ) << " , " << oInfos.m_vAxisArray.at( 3 * i + 2 ) << ")";
			oss << "\n\t\tLocal quaternion : Rotation = ( " << setprecision( m_nAsciiExportPrecision ) << oInfos.m_vLocalQuatArray.at( i ).m_x << " , " << oInfos.m_vLocalQuatArray.at( i ).m_y << " , " << oInfos.m_vLocalQuatArray.at( i ).m_z << " , " << oInfos.m_vLocalQuatArray.at( i ).m_w <<  " )";
			oss << " , Position = ( " << setprecision( m_nAsciiExportPrecision ) << oInfos.m_vLocalQuatArray.at( i ).m_vPosition.m_x << " , " << oInfos.m_vLocalQuatArray.at( i ).m_vPosition.m_y << " , " << oInfos.m_vLocalQuatArray.at( i ).m_vPosition.m_z << " , " << oInfos.m_vLocalQuatArray.at( i ).m_vPosition.m_w << " )";
			i++;
		}
		oss << "\n\n";
	}
	FILE* pFile = m_oFileSystem.OpenFile( sFileName, "w" );
	fwrite( oss.str().c_str(), sizeof( char ), oss.str().size(), pFile );
	fclose( pFile );
}

//void CBKELoader::SerializeMatrix( const CMatrix& m, string& sOut )
//{
//	ostringstream oss;
//	int w = 15;
//	oss << "\t\t" << setw( w ) << setprecision( m_nAsciiExportPrecision ) << m.m_00 << setw( w) << setprecision( m_nAsciiExportPrecision ) << m.m_01 << setw( w) << setprecision( m_nAsciiExportPrecision ) << m.m_02 << setw( w) << setprecision( m_nAsciiExportPrecision ) << m.m_03 << "\n";
//	oss << "\t\t" << setw( w ) << setprecision( m_nAsciiExportPrecision ) << m.m_10 << setw( w) << setprecision( m_nAsciiExportPrecision ) << m.m_11 << setw( w) << setprecision( m_nAsciiExportPrecision ) << m.m_12 << setw( w) << setprecision( m_nAsciiExportPrecision ) << m.m_13 << "\n";
//	oss << "\t\t" << setw( w ) << setprecision( m_nAsciiExportPrecision ) << m.m_20 << setw( w) << setprecision( m_nAsciiExportPrecision ) << m.m_21 << setw( w) << setprecision( m_nAsciiExportPrecision ) << m.m_22 << setw( w) << setprecision( m_nAsciiExportPrecision ) << m.m_23 << "\n";
//	oss << "\t\t" << setw( w ) << setprecision( m_nAsciiExportPrecision ) << m.m_30 << setw( w) << setprecision( m_nAsciiExportPrecision ) << m.m_31 << setw( w) << setprecision( m_nAsciiExportPrecision ) << m.m_32 << setw( w) << setprecision( m_nAsciiExportPrecision ) << m.m_33 << "\n";
//	sOut = oss.str();
//}

void CBKELoader::Export( string sFileName, const ILoader::IRessourceInfos& ri )
{
	string sExt;
	CStringUtils::GetExtension( sFileName, sExt );
	const CAnimationInfos* pData = dynamic_cast< const CAnimationInfos* >( &ri );
	if( sExt == "txt" )
		ExportAscii( sFileName, *pData );
}