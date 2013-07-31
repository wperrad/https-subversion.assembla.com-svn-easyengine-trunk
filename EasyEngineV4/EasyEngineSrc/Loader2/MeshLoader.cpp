#include "MeshLoader.h"

// Engine
#include "../Utils2/StringUtils.h"
#include "Exception.h"
#include "../Utils2/Chunk.h"

using namespace std;

//----------------------------------------------------------------------------
//								Constructor
//----------------------------------------------------------------------------
IMeshLoader::IMeshLoader(void)
{
}


//----------------------------------------------------------------------------
//								Destructor
//----------------------------------------------------------------------------
IMeshLoader::~IMeshLoader(void)
{
}




//----------------------------------------------------------------------------
//								LoadMeshInfos
//----------------------------------------------------------------------------
void IMeshLoader::LoadMeshInfos( CChunk& oChunk, IFileSystem& oFileSystem )
{
	try
	{
		LoadMaterial( oChunk );
	}
	catch ( CEOFException& e )
	{
		m_CurrentFile.Reopen( oFileSystem );
		string* pTextureName = new string;
		*pTextureName = "NONE";
		oChunk.Add( pTextureName, "TextureName" )	;	
	}

	unsigned int nVertexCount = LoadNumElement( "*MESH_NUMVERTEX " );
	unsigned int nFaceCount  = LoadNumElement( "*MESH_NUMFACES " );

	vector< float >* pVertexArray = new vector< float >;
	vector< unsigned int >* pIndexArray = new vector< unsigned int >;
	LoadVertexArray( nVertexCount, *pVertexArray );
	LoadIndexArray( nFaceCount, *pIndexArray );	
	oChunk.Add( pVertexArray, "VertexArray" );
	oChunk.Add( pIndexArray, "IndexArray" );

	try
	{
		unsigned int nUVVertexCount = LoadNumElement("*MESH_NUMTVERTEX ");
		vector< float >* pUVVertexArray = new vector< float >;
		LoadUVVertexArray( nUVVertexCount, *pUVVertexArray );
		unsigned int nTFaceCount = LoadNumElement( "*MESH_NUMTVFACES" );
		//unsigned int nUVIndexCount = 3 * nTFaceCount;
		vector< unsigned int >* pUVIndexArray = new vector< unsigned int >;
		LoadUVIndexArray( nTFaceCount, *pUVIndexArray );
		oChunk.Add(pUVVertexArray, "UVVertexArray");
		oChunk.Add(pUVIndexArray, "UVIndexArray");
	}
	catch (CEOFException& e)
	{
		m_CurrentFile.Reopen( oFileSystem );
	}
	LoadNormalArray( nFaceCount, oChunk );
}



//----------------------------------------------------------------------------
//								Load
//----------------------------------------------------------------------------
bool IMeshLoader::Load( const string& sFileName, CChunk& chunk, IFileSystem& oFileSystem )
{
	m_CurrentFile.Open( sFileName, oFileSystem );
	LoadMeshInfos( chunk, oFileSystem );
	m_CurrentFile.Close();
	return true;
}


//----------------------------------------------------------------------------
//								LoadMaterial
//----------------------------------------------------------------------------
void IMeshLoader::LoadMaterial( CChunk& chunk )
{
	m_CurrentFile.SetPointerNext("*MATERIAL_LIST");
	string sVectorValues;

	m_CurrentFile.GetLineNext( "MATERIAL_AMBIENT", sVectorValues );
	vector< float >* pAmbient = new vector< float >;
	CStringUtils::ExtractFloatFromString( sVectorValues, *pAmbient, 3 );
	pAmbient->push_back( 1.f );
	chunk.Add( pAmbient, "Ambient" );

	m_CurrentFile.GetLineNext( "MATERIAL_DIFFUSE", sVectorValues );	
	vector< float >* pDiffuse = new vector< float >;
	CStringUtils::ExtractFloatFromString( sVectorValues, *pDiffuse, 3 );
	pDiffuse->push_back( 1.f );
	chunk.Add( pDiffuse, "Diffuse" );

	m_CurrentFile.GetLineNext( "MATERIAL_SPECULAR", sVectorValues );
	vector< float >* pSpec = new vector< float >;
	CStringUtils::ExtractFloatFromString( sVectorValues, *pSpec, 3 );
	pSpec->push_back( 1.f );

	m_CurrentFile.GetLineNext( "MATERIAL_SHINE", sVectorValues );
	vector< float >* pShininess = new vector< float >;
	CStringUtils::ExtractFloatFromString( sVectorValues, *pShininess, 1 );
	( *pShininess )[ 0 ] *= 128.f;
	chunk.Add( pShininess, "Shininess" );

	m_CurrentFile.GetLineNext( "MATERIAL_SHINESTRENGTH", sVectorValues );
	vector< float > vFloat;
	CStringUtils::ExtractFloatFromString( sVectorValues, vFloat, 1 );
	for ( unsigned int i = 0; i < 3; i++ )
		( *pSpec )[ i ] *= vFloat[ 0 ] / 9.99f;
	chunk.Add(pSpec, "Specular");

	string sTextureName;
	m_CurrentFile.GetLineNext( "BITMAP ", sTextureName );	
	string sTexturePath = sTextureName.substr( 1, sTextureName.size() - 2 );	
	size_t nBegin = sTexturePath.find_last_of( "\\" );
	string* pTextureName = new string;
	*pTextureName = sTexturePath.substr( nBegin + 1, sTexturePath.size() - nBegin - 2 );
	chunk.Add( pTextureName, "TextureName" );
}





//----------------------------------------------------------------------------
//								LoadVertexArray
//----------------------------------------------------------------------------
void IMeshLoader::LoadVertexArray( int nNumVertex, vector< float >& vVertexArray )
{
	m_CurrentFile.SetPointerNext( "*MESH_VERTEX_LIST" );
	for ( int i = 0; i < nNumVertex; i++ )
	{
		string sBuffer;
		m_CurrentFile.GetLineNext( "MESH_VERTEX", sBuffer );
		vector< float > vFloat;
		CStringUtils::ExtractFloatFromString( sBuffer, vFloat, 4 );
		vVertexArray.push_back( vFloat[ 1 ] );
		vVertexArray.push_back( vFloat[ 2 ] );
		vVertexArray.push_back( vFloat[ 3 ] );
	}
}

//----------------------------------------------------------------------------
//								IMeshLoader
//----------------------------------------------------------------------------
void IMeshLoader::LoadNormalArray( int nFaceCount, CChunk& oChunk )
{
	vector< float >* pNormalFaceArray = new vector< float >;
	vector< float >* pNormalVertexArray = new vector< float >;
	m_CurrentFile.SetPointerNext("*MESH_NORMALS");

	for ( int cmp = 0; cmp < nFaceCount; cmp++ )
	{
		string sBuffer;
		m_CurrentFile.GetLineNext( "MESH_FACENORMAL", sBuffer );
		vector< float > vFloat;
		CStringUtils::ExtractFloatFromString( sBuffer, vFloat, 4 );
		pNormalFaceArray->push_back( vFloat[ 1 ] );
		pNormalFaceArray->push_back( vFloat[ 2 ] );
		pNormalFaceArray->push_back( vFloat[ 3 ] );

		for ( int cmp2 = 0; cmp2 < 3; cmp2 ++ )
		{
			m_CurrentFile.GetLineNext( "MESH_VERTEXNORMAL", sBuffer );			
			CStringUtils::ExtractFloatFromString( sBuffer, vFloat, 4 );	 
			pNormalVertexArray->push_back( vFloat[ 1 ] );
			pNormalVertexArray->push_back( vFloat[ 2 ] );
			pNormalVertexArray->push_back( vFloat[ 3 ] );			
		}
	}
	oChunk.Add( pNormalFaceArray, "NormalFaceArray" );
	oChunk.Add( pNormalVertexArray, "NormalVertexArray" );
}

//----------------------------------------------------------------------------
//								LoadNumElement
//----------------------------------------------------------------------------
int IMeshLoader::LoadNumElement( const string& sWord )
{	
	string sBuffer;
	m_CurrentFile.GetLineNext( sWord, sBuffer );
	vector< float > vFloat;	
	CStringUtils::ExtractFloatFromString( sBuffer, vFloat, 1 );
	unsigned int ret = static_cast<unsigned int> ( vFloat[ 0 ] );
	return ret;
}



//----------------------------------------------------------------------------
//								LoadUVVertexArray
//----------------------------------------------------------------------------
void IMeshLoader::LoadUVVertexArray( int nVertexCount, vector< float >& vUVVertexArray )
{
	m_CurrentFile.SetPointerNext( "*MESH_TVERTLIST" );
	for ( int i = 0; i < nVertexCount; i++ )
	{		
		m_CurrentFile.SetPointerNext("*MESH_TVERT");		
		string sBuffer;	
		m_CurrentFile.GetLine( sBuffer );
		vector< float > vFloat;
		CStringUtils::ExtractFloatFromString( sBuffer, vFloat, 4 );
		vUVVertexArray.push_back( vFloat[1] );
		vUVVertexArray.push_back( vFloat[2] );
		vUVVertexArray.push_back( vFloat[3] );		
	}
}


//----------------------------------------------------------------------------
//								LoadUVIndexArray
//----------------------------------------------------------------------------
void IMeshLoader::LoadUVIndexArray( int nTFaceCount, vector< unsigned int >& vUVIndexArray )
{	
	m_CurrentFile.SetPointerNext( "*MESH_TFACELIST" );
	for ( int i = 0; i < nTFaceCount; i++ )
	{
		string  sBuffer;
		m_CurrentFile.GetLineNext( "MESH_TFACE", sBuffer );
		vector< float > vFloat;
		CStringUtils::ExtractFloatFromString( sBuffer, vFloat, 4 );
		for ( unsigned int j = 0; j < 3; j++ )
			vUVIndexArray.push_back( (unsigned int) ( vFloat[ j + 1 ] ) );
	}	
}
	