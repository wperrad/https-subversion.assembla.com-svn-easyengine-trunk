#include "Aseloader.h"


// stl
#include <string>
#include <vector>

// Engine
#include "../Utils2/EasyFile.h"
#include "../Utils2/StringUtils.h"




using namespace std;


CAseLoader::CAseLoader(void)
{	

}

CAseLoader::~CAseLoader(void)
{
}






void CAseLoader::Export( const string& sFileName, const CChunk& chunk )
{
	
}




//	Charge le tableau d'index 
void CAseLoader::LoadIndexArray( int nNumFace, vector< unsigned int >& vIndexArray )
{
	m_CurrentFile.SetPointerNext("*MESH_FACE_LIST");
	for ( int i = 0; i < nNumFace; i++ )
	{
		m_CurrentFile.SetPointerNext("A:");
		string sBuffer;
		m_CurrentFile.GetLine( sBuffer );
		vector< float > vFloat;
		CStringUtils::ExtractFloatFromString( sBuffer, vFloat, 3 );
		vIndexArray.push_back( (unsigned int) ( vFloat[ 0 ] ) );
		vIndexArray.push_back( (unsigned int) ( vFloat[ 1 ] ) );
		vIndexArray.push_back( (unsigned int) ( vFloat[ 2 ] ) );
	}
}

