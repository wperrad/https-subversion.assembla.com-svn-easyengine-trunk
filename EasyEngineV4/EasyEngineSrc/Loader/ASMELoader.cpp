#include "ASMEloader.h"
#include "../utils2/EasyFile.h"
#include "../utils2/StringUtils.h"
#include <shlwapi.h>
#include "../Utils2/Chunk.h"

using namespace std;


//----------------------------------------------------------------------------------
//									Constructeur
//----------------------------------------------------------------------------------
CASMELoader::CASMELoader(void)
{
}


//----------------------------------------------------------------------------------
//									Constructeur
//----------------------------------------------------------------------------------
CASMELoader::~CASMELoader(void)
{
}


void CASMELoader::Export( const string& sFileName, const CChunk& chunk)
{
	
}



void CASMELoader::ExportHierarchyNode( CEasyFile& file )
{
	
}



//----------------------------------------------------------------------------------
//									Constructeur
//----------------------------------------------------------------------------------
bool CASMELoader::Load( const string& sFileName, CChunk& oChunk, IFileSystem& oFileSystem )
{
	m_CurrentFile.Open( sFileName, oFileSystem );
	LoadMeshInfos( oChunk, oFileSystem );
	LoadHierarchy( oChunk );
	LoadSkin( oChunk );
	m_CurrentFile.Close();
	return true;
}


//
////----------------------------------------------------------------------------------
////									Constructeur
////----------------------------------------------------------------------------------
//void CASMELoader::LoadSkin(CChunk& chunk)
//{
//	m_CurrentFile.SetPointerNext("*MESH_SKIN");
//	unsigned int nVertexCount = *static_cast<unsigned int*> (chunk.Get("VertexCount"));
//	for (unsigned int i=0 ; i<nVertexCount ; i++)
//	{
//		char szBuffer[256], szBonesCount[5];
//		m_CurrentFile.GetLineNext("BONE_COUNT", szBuffer);		
//		CStringUtils::GetWordByIndex(szBuffer, 0, szBonesCount);
//		float pFloat[1];
//		CStringUtils::ExtractFloatFromString(szBonesCount, pFloat, 1);
//		unsigned int nBonesCount = static_cast<unsigned int> (pFloat[0]);
//		Map_BoneID_Weight m;
//		for (unsigned int j=0 ; j<nBonesCount; j++)
//		{
//			char szboneID[32], szWeight[32];
//			CStringUtils::GetWordByIndex(szBuffer, 2+3*j, szboneID);
//			CStringUtils::GetWordByIndex(szBuffer, 3+3*j, szWeight);
//			CStringUtils::ExtractFloatFromString(szboneID, pFloat, 1);
//			unsigned int nBoneID = static_cast<unsigned int> (pFloat[0]);
//			CStringUtils::ExtractFloatFromString(szWeight, pFloat, 1);
//			float fWeight = pFloat[0];
//			m.insert(Map_BoneID_Weight::value_type(nBoneID, fWeight));
//		}
//		m_Skin.push_back(m);
//	}
//}




//----------------------------------------------------------------------------------
//									Constructeur
//----------------------------------------------------------------------------------
void CASMELoader::LoadSkin( CChunk& chunk )
{
	m_CurrentFile.SetPointerNext( "*MESH_SKIN" );
	unsigned int nVertexCount = *static_cast<unsigned int*> ( chunk.Get( "VertexCount" ) );
	for ( unsigned int i = 0; i < nVertexCount; i++ )
	{
		string sBuffer, sBonesCount;
		m_CurrentFile.GetLineNext( "BONE_COUNT", sBuffer );
		CStringUtils::GetWordByIndex( sBuffer, 0, sBonesCount );
		vector< float > vFloat;
		CStringUtils::ExtractFloatFromString( sBonesCount, vFloat, 1 );
		unsigned int nBonesCount = static_cast<unsigned int> ( vFloat[ 0 ] );
		unsigned int* pBonesCount = new unsigned int;
		*pBonesCount = nBonesCount;
		char szi[8];
		_itoa_s(i, szi, 10);
		string strPropertyName = string("WeightVertexBonesCount_") + szi;
		chunk.Add(pBonesCount,strPropertyName.c_str());		
		float* pWeightArray = new float[nBonesCount];
		vector<string>* pvBonesNameArray = new vector<string>;
		for ( unsigned int j = 0; j < nBonesCount; j++ )
		{
			string sboneName, sWeight;
			CStringUtils::GetWordByIndex( sBuffer, 1+3*j, sboneName );
			CStringUtils::GetWordByIndex( sBuffer, 3+3*j, sWeight );
			CStringUtils::ExtractFloatFromString( sWeight, vFloat, 1 );
			pWeightArray[ j ] = vFloat[0];
			pvBonesNameArray->push_back( sboneName );
		}
		string strBoneWeightArrayDesc = string("BoneWeight_") + szi;
		string strBonesNameArrayDesc = string("BoneName_") + szi;
		chunk.Add(pWeightArray , strBoneWeightArrayDesc.c_str());
		chunk.Add(pvBonesNameArray, strBonesNameArrayDesc.c_str());
	}
}




//----------------------------------------------------------------------------------
//									Constructeur
//----------------------------------------------------------------------------------
void CASMELoader::LoadIndexArray( int nFaceCount, CChunk& chunk )
{
	unsigned int* pIndexArray = new unsigned int[nFaceCount*3];	
	m_CurrentFile.SetPointerNext("*MESH_FACE_LIST");			
	for ( int i = 0; i < nFaceCount; i++ )
	{				
		m_CurrentFile.SetPointerNext( "*MESH_FACE" );
		string sBuffer;
		m_CurrentFile.GetLine( sBuffer );
		vector< float > vFloat;
		CStringUtils::ExtractFloatFromString( sBuffer, vFloat, 4 );
		pIndexArray[ i * 3 ] = static_cast<unsigned int> ( vFloat[ 1 ] );
		pIndexArray[ i * 3 + 1 ] = static_cast< unsigned int > ( vFloat[ 2 ] );
		pIndexArray[ i * 3 + 2] = static_cast< unsigned int > ( vFloat[ 3 ] );
	}	
	chunk.Add( pIndexArray, "IndexArray" );
}



////----------------------------------------------------------------------------------
////									Constructeur
////----------------------------------------------------------------------------------
//CSkin& CASMELoader::GetSkin()
//{
//	return m_Skin;
//}
//


////----------------------------------------------------------------------------------
////									Constructeur
////----------------------------------------------------------------------------------
//unsigned int CASMELoader::GetBonesCount()
//{
//	return m_nBonesCount;
//}
//
//
//
////----------------------------------------------------------------------------------
////									LoadHierarchy
////----------------------------------------------------------------------------------
//CNode* CASMELoader::LoadHierarchy()
//{	
//	char szBuffer[32];
//	file.GetLineNext( "*BONES_COUNT", szBuffer);
//	float pFloatCount[1];
//	CStringUtils::ExtractFloatFromString(szBuffer, pFloatCount, 1);	
//	m_nBonesCount = pFloatCount[0];
//	
//	map<int,CNode*> mapIDNode;
//	for (int i = 0 ; i < m_nBonesCount ; i++)
//	{
//		CNode* pNode = new CNode;
//		char szBufferName[32];
//		file.GetLineNext("*BONE_NAME ", szBufferName);
//		pNode->SetName(szBufferName);
//		char szBufferID[32];
//		float pFloatID[1];
//		file.GetLineNext("*BONE_ID", szBufferID);
//		CStringUtils::ExtractFloatFromString(szBufferID, pFloatID, 1);
//		int nID = pFloatID[0];
//		pNode->m_nID = nID;
//		char szParentID[32];		
//		file.GetLineNext("*PARENT_ID", szParentID);
//		CStringUtils::ExtractFloatFromString(szParentID, pFloatID, 1);
//		int nParentID = pFloatID[0];
//		pNode->m_nParentID = nParentID;
//		float pMatValues[4][4] = {0.f};
//		for (int j=0 ; j < 3 ; j++)
//		{
//			char szBufferMatrixLine[256];
//			file.GetLine(szBufferMatrixLine);			
//			CStringUtils::ExtractFloatFromString(szBufferMatrixLine, pMatValues[j], 4);			
//		}
//		pMatValues[3][3] = 1.f;
//		CMatrix XForm(pMatValues);
//		pNode->SetXFormTM(XForm);				
//		mapIDNode.insert( map<int, CNode*>::value_type(nID, pNode));
//	}
//
//	for (int i=0 ; i<mapIDNode.size() ; i++)
//	{
//		CNode* pNode = mapIDNode[i];
//		
//		if (mapIDNode.count(pNode->m_nParentID) > 0)
//		{
//			map<int , CNode*>::iterator itNode = mapIDNode.find(pNode->m_nParentID);
//			CNode* pParent = itNode->second;
//			pNode->Link(pParent);
//		}
//	}		
//	CNode* pTest;
//	map<int , CNode*>::iterator itNode = mapIDNode.find(0);
//	CNode* pRoot = itNode->second;
//	return pRoot;
//}




//----------------------------------------------------------------------------------
//									LoadHierarchy
//----------------------------------------------------------------------------------
void CASMELoader::LoadHierarchy(CChunk& chunk)
{
	m_CurrentFile.SetPointerNext("*MESH_BONES");
	string sBuffer;
	m_CurrentFile.GetLineNext( "*BONES_COUNT", sBuffer );
	vector< float > vFloatCount;
	CStringUtils::ExtractFloatFromString( sBuffer, vFloatCount, 1 );
	unsigned int nBonesCount = (unsigned int)vFloatCount[ 0 ];
	unsigned int* pBonesCount = new unsigned int;
	*pBonesCount = nBonesCount;
	chunk.Add( pBonesCount, "BonesCount" );

	CChunk* pBonesNameArray = new CChunk;
	CChunk* pParentsNameArray = new CChunk;

	for ( unsigned int i = 0; i < nBonesCount; i++ )
	{
		string* pBoneName = new string;
		m_CurrentFile.GetLineNext( "*BONE_NAME ", *pBoneName );
		size_t len = pBoneName->size() - 1;
		while ( (*pBoneName)[ len ] == ' ' )
			len--;
		pBoneName->resize( len );
		unsigned int nSpaceBegin = 0;
		while ( ( *pBoneName )[ nSpaceBegin ] == ' ' )
			nSpaceBegin++;
		string sTempBoneName;
		for ( unsigned int j = 0; j <= ( *pBoneName ).size(); j++ )
			sTempBoneName.push_back( ( *pBoneName )[ j + nSpaceBegin ] );
		( *pBoneName ) = sTempBoneName;
		char szNum[10];
		_itoa_s(i, szNum, 10);
		pBonesNameArray->Add( pBoneName, szNum );		
		
		string* pParentName = new string;
		m_CurrentFile.GetLineNext( "*PARENT_NAME", *pParentName );

		len = pParentName->size() - 1;
		while ( ( *pParentName )[ len ] == ' ' )
			len--;
		pParentName->resize( len );
		
		nSpaceBegin = 0;
		while ( ( *pParentName )[ nSpaceBegin ] == ' ')				
			nSpaceBegin++;
		char szTempParentName[32];
		for ( unsigned int j = 0; j <= ( *pParentName ).size(); j++ )
			szTempParentName[ j ] = ( *pParentName )[ j + nSpaceBegin ];
		( *pParentName ) = szTempParentName;

		pParentsNameArray->Add( pParentName, szNum );
		
		//float pMatValues[4][4] = {0.f};
		vector< vector< float > > vvMatValues;
		for (int j=0 ; j < 3 ; j++)
		{
			string sBufferMatrixLine;
			m_CurrentFile.GetLine( sBufferMatrixLine );
			vector< float > vMatValues;
			CStringUtils::ExtractFloatFromString( sBufferMatrixLine, vMatValues, 4 );
			vvMatValues.push_back( vMatValues );
		}
		vvMatValues[3].push_back( 1.f );
		float* pExportMatValues = new float[16];
		for (int j=0 ; j<16 ; j++)
		{
			unsigned int nIndiceI, nIndiceJ;
			nIndiceI = j/4;
			nIndiceJ = j%4;
			pExportMatValues[j] = vvMatValues[ nIndiceI ][ nIndiceJ ];
		}
		string strMatrixPropName = string("XForm") + szNum;
		chunk.Add( pExportMatValues, strMatrixPropName.c_str() );
	}
	chunk.Add( pParentsNameArray, "ParentsNameArray");
	chunk.Add( pBonesNameArray, "BonesNameArray");
}
