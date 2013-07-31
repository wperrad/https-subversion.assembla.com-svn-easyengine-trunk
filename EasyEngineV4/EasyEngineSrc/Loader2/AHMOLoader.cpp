#ifdef _TEST_

#include "AHMOLoader.h"
#include "../utils2/StringUtils.h"
#include "../Utils2/EasyFile.h"
#include "../Utils2/Chunk.h"
#include <map>

using namespace std;

//----------------------------------------------------------------------------------
//									Constructor
//----------------------------------------------------------------------------------
CAHMOLoader::CAHMOLoader(void)
{
}



//----------------------------------------------------------------------------------
//									Destructor
//----------------------------------------------------------------------------------
CAHMOLoader::~CAHMOLoader(void)
{
}


//----------------------------------------------------------------------------------
//									Load
//----------------------------------------------------------------------------------
bool CAHMOLoader::Load( const string& sFileName, CChunk& chunk, IFileSystem& oFileSystem )
{
	CEasyFile oFile;
	oFile.Open( sFileName, oFileSystem );

	LoadHierarchy(chunk);
	unsigned int nBonesCount=0;

	unsigned int** ppIndex = new unsigned int*[nBonesCount];
	float** ppVertex = new float*[nBonesCount];
	for (unsigned int iBone =0 ; iBone<nBonesCount; iBone++)
	{
		oFile.SetPointerNext("*MESH_BONE");
		string sFaceCount;		
		oFile.GetLineNext( "*MESH_FACE_COUNT", sFaceCount );
		vector< float > vFloat;
		CStringUtils::ExtractFloatFromString( sFaceCount, vFloat, 1 );
		const unsigned int nFaceCount = static_cast< unsigned int > ( vFloat[ 0 ] );
		oFile.SetPointerNext( "*MESH_FACE_LIST" );		
		ppIndex[ iBone ] = new unsigned int[ nFaceCount * 3 ];
		for ( unsigned int iFace = 0; iFace < nFaceCount; iFace++ )
		{
			string sIndexLine;
			oFile.GetLineNext( "MESH_FACE", sIndexLine );
			vector< float > vIndex;
			CStringUtils::ExtractFloatFromString( sIndexLine, vIndex, 4 );
			for ( int iIndex = 0; iIndex < 3; iIndex++ )
				ppIndex[ iBone ][ iFace * 3 + iIndex ] = static_cast<unsigned int> ( vIndex[ iIndex + 1 ] );
		}
		string sVertexCount;
		oFile.GetLineNext( "*MESH_VERTEX_COUNT", sVertexCount );
		vector< float > vNumVertex;
		CStringUtils::ExtractFloatFromString( sVertexCount, vNumVertex, 1 );
		const unsigned int nNumVertex = static_cast<unsigned int> ( vNumVertex[ 0 ] );
		oFile.SetPointerNext( "*MESH_VERTEX_LIST" );
		ppVertex[ iBone ] = new float[ nNumVertex ];
		for ( unsigned int iVertex = 0; iVertex < nNumVertex; iVertex++ )
		{
			//float* pVertexArray = new float[ nNumVertex ];
			string sVertexLine;
			oFile.GetLineNext( "*MESH_VERTEX", sVertexLine );
			vector< float > vFloatCoord;
			CStringUtils::ExtractFloatFromString( sVertexLine, vFloatCoord, 4 );
			for ( int iCoord = 0; iCoord < 3; iCoord++ )
				ppVertex[ iBone ][ iVertex * 3 + iCoord ] = vFloatCoord[ iCoord + 1 ];
		}
	}
	oFile.Close();
	return true;
}
//
//
////----------------------------------------------------------------------------------
////									GetVertexCount
////----------------------------------------------------------------------------------
//float* CAHMOLoader::GetVertexCount(int nNumVertexArray)
//{
//	return ppVertex[nNumVertexArray];
//}
//
//
//
////----------------------------------------------------------------------------------
////									GetIndexCount
////----------------------------------------------------------------------------------
//unsigned int* CAHMOLoader::GetIndexCount(int nNumIndexArray)
//{
//	return ppIndex[nNumIndexArray];
//}
	
//
////----------------------------------------------------------------------------------
////									GetHierarchyRoot
////----------------------------------------------------------------------------------
//CNode* CAHMOLoader::GetHierarchyRoot()
//{
//	return m_pHierarchy;
//}


//----------------------------------------------------------------------------------
//									LoadHierarchy
//----------------------------------------------------------------------------------
void CAHMOLoader::LoadHierarchy(CChunk& chunk)
{	
	//char szBuffer[32];
	//m_CurrentFile.GetLineNext( "*BONES_COUNT", szBuffer);
	//float pFloatCount[1];
	//CStringUtils::ExtractFloatFromString(szBuffer, pFloatCount, 1);	
	//unsigned int nBonesCount = pFloatCount[0];
	//unsigned int* pBonesCount = new unsigned int;
	//*pBonesCount = nBonesCount;
	//chunk.Add(pBonesCount, "BonesCount");

	//
	//map<int,CNode*> mapIDNode;
	//for (int i = 0 ; i < m_nBonesCount ; i++)
	//{
	//	CNode* pNode = new CNode;
	//	char szBufferName[32];
	//	file.GetLineNext("*BONE_NAME ", szBufferName);
	//	pNode->SetName(szBufferName);
	//	char szBufferID[32];
	//	float pFloatID[1];
	//	file.GetLineNext("*BONE_ID", szBufferID);
	//	CStringUtils::ExtractFloatFromString(szBufferID, pFloatID, 1);
	//	int nID = pFloatID[0];
	//	pNode->m_nID = nID;
	//	char szParentID[32];		
	//	file.GetLineNext("*PARENT_ID", szParentID);
	//	CStringUtils::ExtractFloatFromString(szParentID, pFloatID, 1);
	//	int nParentID = pFloatID[0];
	//	pNode->m_nParentID = nParentID;
	//	float pMatValues[4][4] = {0.f};
	//	for (int j=0 ; j < 3 ; j++)
	//	{
	//		char szBufferMatrixLine[256];
	//		file.GetLine(szBufferMatrixLine);			
	//		CStringUtils::ExtractFloatFromString(szBufferMatrixLine, pMatValues[j], 4);			
	//	}
	//	pMatValues[3][3] = 1.f;
	//	CMatrix XForm(pMatValues);
	//	pNode->SetXFormTM(XForm);				
	//	mapIDNode.insert( map<int, CNode*>::value_type(nID, pNode));
	//}

	//for (int i=0 ; i<mapIDNode.size() ; i++)
	//{
	//	CNode* pNode = mapIDNode[i];
	//	
	//	if (mapIDNode.count(pNode->m_nParentID) > 0)
	//	{
	//		map<int , CNode*>::iterator itNode = mapIDNode.find(pNode->m_nParentID);
	//		CNode* pParent = itNode->second;
	//		pNode->Link(pParent);
	//	}
	//}		
	//CNode* pTest;
	//map<int , CNode*>::iterator itNode = mapIDNode.find(0);
	//CNode* pRoot = itNode->second;
	//return pRoot;
}




////----------------------------------------------------------------------------------
////									Constructeur
////----------------------------------------------------------------------------------
//void CAHMOLoader::Export(const char* szFileName, const CChunk& chunk)
//{		
//
//	typedef vector<unsigned int> VectorInt;
//	vector<VectorInt*> vBonesIndexArray;
//	vector<unsigned int> vComplexeIndexArray;
//	unsigned int* pBonesCount = NULL;
//	chunk.Get("BonesCount", pBonesCount);
//	unsigned int nBonesCount = pBonesCount[0];
//	unsigned int* pIndexArray = static_cast<unsigned int* > (chunk.Get("Index array"));
//
//	for (unsigned int i=0 ; i<nBonesCount; i++)
//	{
//		VectorInt* pVector = new VectorInt;
//		vBonesIndexArray.push_back(pVector);
//	}
//
//	CSkin* pSkin = NULL;
//	chunk.Get("Skin", pSkin);	
//	CSkin& Skin = *pSkin;
//
//	unsigned int nVertexCount = *static_cast<unsigned int*> (chunk.Get("Vertex count"));
//	for (unsigned int iVertex=0 ; iVertex<nVertexCount ; iVertex++)
//	{
//		Map_BoneID_Weight boneInfo = Skin[iVertex];						
//		for (unsigned int i=0 ; i<nBonesCount ; i++)
//		{
//			if (boneInfo.count(i) > 0 )
//			{
//				if (boneInfo.size()<2)
//				{
//					Map_BoneID_Weight::iterator itFind = boneInfo.find(i);
//					float fBoneWeight = itFind->second;
//					unsigned int nBoneID = i;						
//					vBonesIndexArray[nBoneID]->push_back(iVertex);
//				}
//				else
//				{
//					vComplexeIndexArray.push_back(iVertex);						
//				}
//			}			
//		}	
//	}		
//	
//	// Création des tableaux de face
//	vector<VectorInt*> vFaceArray;
//	unsigned int nIndexCount = *static_cast<unsigned int*> (chunk.Get("Index count"));
//	unsigned int nFaceCount = nIndexCount / 3;
//	
//	for (unsigned int i=0 ; i<nFaceCount; i++)
//	{
//		unsigned int nIndex0,nIndex1,nIndex2;
//		unsigned int nIndex0BoneID=-1,nIndex1BoneID=-2,nIndex2BoneID=-3;
//		nIndex0 = pIndexArray[i*3];
//		nIndex1 = pIndexArray[i*3+1];
//		nIndex2 = pIndexArray[i*3+2];
//		unsigned int nVertexInSameFaceCount = 0;
//
//
//		// On cherche à quel bone est lié chaque index
//		for (unsigned int j=0 ; j<nBonesCount ; j++)
//		{
//			if (nVertexInSameFaceCount != 0)
//				break;
//			VectorInt* pFaceArray = new VectorInt;
//			vFaceArray.push_back(pFaceArray);
//			VectorInt* pIndex = vBonesIndexArray[j];
//			size_t nIndexCount = pIndex->size();
//			for (size_t k=0 ; k<nIndexCount ; k++)
//			{
//				unsigned int nValue = pIndex->at(k);
//				if (nIndex0 == nValue)	
//				{
//					nIndex0BoneID = j;
//					nVertexInSameFaceCount++;
//				}
//				
//				if (nIndex1 == nValue)		
//				{
//					nIndex1BoneID = j;						
//					nVertexInSameFaceCount++;
//				}
//				
//				if (nIndex2 == nValue)		
//				{
//					nIndex2BoneID = j;
//					nVertexInSameFaceCount++;
//				}
//				if ( nVertexInSameFaceCount >= 3 )
//				{
//					vFaceArray[j]->push_back(i);
//					break;
//				}					
//			}
//			if ( nVertexInSameFaceCount >= 3 )
//				break;
//		}
//	}
//	
//
//	// création du tableau de faces dynamiques
//	vector<unsigned int> vDynamicsFaces;
//	for (unsigned int i=0 ; i<nFaceCount ; i++)
//	{
//		bool bAppartient = true;
//		for (unsigned int j=0 ; j<nBonesCount ; j++)
//		{
//			VectorInt* pFaceArray = vFaceArray[j];
//			for (unsigned int k=0 ; k<pFaceArray->size() ; k++)
//			{
//				unsigned int nValue = pFaceArray->at(k);
//				if (nValue == i)
//				{
//					bAppartient = false;
//					break;
//				}
//			}
//			if (!bAppartient)
//				break;				
//		}
//		if (bAppartient)
//			vDynamicsFaces.push_back(i);
//	}	
//	FILE* pFile = fopen(szFileName, "w");
//	
//	for (unsigned int i=0 ; i< nBonesCount ; i++)
//	{
//		char szNumBone[8];
//		itoa(i, szNumBone, 10);
//		string str = string("Bone ") + string(szNumBone) + string("\n");
//		fwrite(str.c_str() , strlen(str.c_str()),1,pFile);
//		for (unsigned j=0 ; j<vFaceArray[i]->size() ; j++)
//		{
//			char szNumFace[8];
//			int nNumFace = vFaceArray[i]->at(j);
//			itoa(nNumFace, szNumFace, 10);
//			char szText[256];
//			strcpy(szText, "*MESH_FACE ");
//			char szNewNumFace[8];
//			itoa(j, szNewNumFace, 10);
//			strcat(szText, szNewNumFace);
//			strcat(szText, "	");
//			strcat(szText, szNumFace);
//			
//			unsigned int index0, index1, index2;
//			index0 = pIndexArray[3*nNumFace];
//			index1 = pIndexArray[3*nNumFace+1];
//			index2 = pIndexArray[3*nNumFace+2];
//			char szIndex0[8], szIndex1[8], szIndex2[8];
//			itoa(index0, szIndex0, 10);
//			itoa(index1, szIndex1, 10);
//			itoa(index2, szIndex2, 10);
//			strcat(szText, "		");
//			strcat(szText, szIndex0);
//			strcat(szText, "	");
//			strcat(szText, szIndex1);
//			strcat(szText, "	");
//			strcat(szText, szIndex2);
//			strcat(szText, "\n");
//			
//			size_t len = strlen(szText);
//			fwrite(szText, len, 1, pFile);
//		}
//		fwrite("\n\n\n", 3, 1, pFile);
//	}
//	
//	fclose(pFile);
//}
//
//




////----------------------------------------------------------------------------------
////									Export
////----------------------------------------------------------------------------------
//void CAHMOLoader::Export(const char* szFileName, const CChunk& chunk)
//{
//	typedef vector<unsigned int> VectorInt;
//	vector<VectorInt*> vBonesIndexArray;		
//	unsigned int * pBonesCount = static_cast<unsigned int*> (chunk.Get("BonesCount"));
//	unsigned int nBonesCount = pBonesCount[0];
//
//	for (unsigned int i=0 ; i<nBonesCount; i++)
//	{
//		VectorInt* pVector = new VectorInt;
//		vBonesIndexArray.push_back(pVector);
//	}
//
//
//	unsigned int nVertexCount = *static_cast<unsigned int*> (chunk.Get("VertexCount"));
//	CSkin* pSkin = static_cast<CSkin*> (chunk.Get("Skin"));
//	for (unsigned int iVertex=0 ; iVertex<nVertexCount ; iVertex++)
//	{
//		Map_BoneID_Weight boneInfo = (*pSkin)[iVertex];						
//		for (unsigned int i=0 ; i<nBonesCount ; i++)
//		{
//			if (boneInfo.count(i) > 0 )
//			{
//				if (boneInfo.size()<2)
//				{
//					Map_BoneID_Weight::iterator itFind = boneInfo.find(i);
//					float fBoneWeight = itFind->second;
//					unsigned int nBoneID = i;						
//					vBonesIndexArray[nBoneID]->push_back(iVertex);
//				}
//				else
//				{
//					// on cherche le poids le plus fort
//					float fMaxWeight=0.f;
//					unsigned int nMaxID = 9999;
//					for (unsigned int j=0 ; j<boneInfo.size() ; j++)
//					{
//						Map_BoneID_Weight::iterator itFind = boneInfo.find(j);
//						float fBoneWeight = itFind->second;
//						if (fBoneWeight > fMaxWeight)
//						{
//							fMaxWeight = fBoneWeight;
//							nMaxID = j;
//						}
//					}
//					vBonesIndexArray[nMaxID]->push_back(iVertex);					
//				}
//			}			
//		}	
//	}		
//
//	// Nettoyage des 2 tableaux (pour éviter les redondances)
//	vector<VectorInt*> vStaticVertex;
//	for (unsigned int i=0 ; i<nBonesCount; i++)
//	{
//		VectorInt* pVector = new VectorInt;
//		vStaticVertex.push_back(pVector);
//	}
//	
//	for (unsigned int iNumBone=0 ; iNumBone<nBonesCount ; iNumBone++)
//	{
//		for (unsigned int iNumVertex=0 ; iNumVertex<vBonesIndexArray[iNumBone]->size()-1 ; iNumVertex++)
//		{
//			unsigned nNumVertex = vBonesIndexArray[iNumBone]->at(iNumVertex);
//			if (nNumVertex != vBonesIndexArray[iNumBone]->at(iNumVertex+1))
//				vStaticVertex[iNumBone]->push_back(nNumVertex);			
//		}
//		int nLastVertexNumber = vBonesIndexArray[iNumBone]->at(vBonesIndexArray[iNumBone]->size()-1);
//		vStaticVertex[iNumBone]->push_back(nLastVertexNumber);         
//	}	
//
//
//	// création des maps (une par bone) associant les anciens numéros de vertex avec les nouveaux
//
//	typedef map<unsigned int, unsigned int> MapOldNewIndex;
//	vector<MapOldNewIndex*> vMapOldNewIndex;
//
//	for (unsigned int iNumBone=0 ; iNumBone<nBonesCount ; iNumBone++)
//	{
//		MapOldNewIndex * pMapOldNewIndex = new MapOldNewIndex;		
//
//		for (unsigned int iNumVertex=0 ; iNumVertex<vStaticVertex[iNumBone]->size() ; iNumVertex++)
//		{
//			int nNumVertex = vStaticVertex[iNumBone]->at(iNumVertex);	// ancien indice (nouvel indice = iNumVertex)			
//			pMapOldNewIndex->insert(MapOldNewIndex::value_type(nNumVertex, iNumVertex));
//		}		
//		vMapOldNewIndex.push_back(pMapOldNewIndex);
//	}
//
//	// Création des tableaux de faces statiques
//	vector<VectorInt*> vFaceArray;	
//	unsigned int nIndexCount = *static_cast<unsigned int*> (chunk.Get("IndexCount"));
//	unsigned int nFaceCount = nIndexCount / 3;
//	unsigned int* pIndexArray = static_cast<unsigned int*> (chunk.Get("IndexArray"));
//	for (unsigned int i=0 ; i<nFaceCount; i++)
//	{
//		unsigned int nIndex0,nIndex1,nIndex2;
//		unsigned int nIndex0BoneID=-1,nIndex1BoneID=-2,nIndex2BoneID=-3;
//		nIndex0 = pIndexArray[i*3];
//		nIndex1 = pIndexArray[i*3+1];
//		nIndex2 = pIndexArray[i*3+2];
//		unsigned int nVertexInSameFaceCount = 0;
//
//		// On cherche à quel bone est lié chaque index
//		for (unsigned int j=0 ; j<nBonesCount ; j++)
//		{
//			if (nVertexInSameFaceCount != 0)
//				break;
//			VectorInt* pFaceArray = new VectorInt;
//			VectorInt* pNewFaceArray = new VectorInt;
//			vFaceArray.push_back(pFaceArray);			
//			VectorInt* pIndex = vStaticVertex[j];
//			size_t nIndexCount = pIndex->size();
//			for (size_t k=0 ; k<nIndexCount ; k++)
//			{
//				unsigned int nValue = pIndex->at(k);
//				if (nIndex0 == nValue)	
//				{
//					nIndex0BoneID = j;
//					nVertexInSameFaceCount++;
//				}
//				
//				if (nIndex1 == nValue)		
//				{
//					nIndex1BoneID = j;						
//					nVertexInSameFaceCount++;
//				}
//				
//				if (nIndex2 == nValue)		
//				{
//					nIndex2BoneID = j;
//					nVertexInSameFaceCount++;
//				}
//				if ( nVertexInSameFaceCount >= 3 )
//				{					
//					vFaceArray[j]->push_back(i);					
//					break;
//				}					
//			}
//			if ( nVertexInSameFaceCount >= 3 )
//				break;
//		}
//	}
//	
//	FILE* pFile = fopen(szFileName, "w");
//	
//	
//	for (unsigned int i=0 ; i< nBonesCount ; i++)
//	{
//		char szNumBone[8];
//		itoa(i, szNumBone, 10);
//		string str = string("*MESH_BONE ") + string(szNumBone) + string("\n\n");
//		fwrite(str.c_str() , strlen(str.c_str()),1,pFile);
//
//		// Ecriture des nouvelles faces 
//		fwrite("*MESH_FACE_LIST\n", strlen("*MESH_FACE_LIST\n"),1,pFile);
//		for (unsigned int j=0 ; j<vFaceArray[i]->size() ; j++)
//		{
//			char szNumFace[8];
//			int nNumFace = vFaceArray[i]->at(j);
//			itoa(nNumFace, szNumFace, 10);
//			char szText[256];
//			strcpy(szText, "*MESH_FACE ");
//			char szNewNumFace[8];
//			itoa(j, szNewNumFace, 10);
//			strcat(szText, szNewNumFace);
//			
//			unsigned int OldIndex0 = pIndexArray[3*nNumFace];
//			unsigned int OldIndex1 = pIndexArray[3*nNumFace+1];
//			unsigned int OldIndex2 = pIndexArray[3*nNumFace+2];
//			
//			MapOldNewIndex::iterator itFind0 = vMapOldNewIndex[i]->find(OldIndex0);
//			MapOldNewIndex::iterator itFind1 = vMapOldNewIndex[i]->find(OldIndex1);
//			MapOldNewIndex::iterator itFind2 = vMapOldNewIndex[i]->find(OldIndex2);
//			unsigned int NewIndex0 = itFind0->second;
//			unsigned int NewIndex1 = itFind1->second;
//			unsigned int NewIndex2 = itFind2->second;
//
//			char szIndex0[8], szIndex1[8], szIndex2[8];
//			itoa(NewIndex0, szIndex0, 10);
//			itoa(NewIndex1, szIndex1, 10);
//			itoa(NewIndex2, szIndex2, 10);
//			strcat(szText, "	");
//			strcat(szText, szIndex0);
//			strcat(szText, "	");
//			strcat(szText, szIndex1);
//			strcat(szText, "	");
//			strcat(szText, szIndex2);
//			strcat(szText, "\n");
//			
//			size_t len = strlen(szText);
//			fwrite(szText, len, 1, pFile);
//		}
//		fwrite("\n\n\n", 3, 1, pFile);
//
//		fwrite("*MESH_VERTEX_LIST\n\n", strlen("*MESH_VERTEX_LIST\n"), 1, pFile);
//
//
//		// Ecriture des coordonnées des nouveaux vertex		
//		float* pVertexArray = static_cast<float*> (chunk.Get("VertexArray"));
//		for (unsigned int iNumVertex=0 ; iNumVertex<vStaticVertex[i]->size() ; iNumVertex++)
//		{
//			int nNumVertex = vStaticVertex[i]->at(iNumVertex);
//			float x = pVertexArray [nNumVertex*3];
//			float y = pVertexArray [nNumVertex*3+1];
//			float z = pVertexArray [nNumVertex*3+2];
//			char szx[32], szy[23], szz[32];
//			sprintf(szx, "%f", x);
//			sprintf(szy, "%f", y);
//			sprintf(szz, "%f", z);			
//			char szNumVertex[16];			
//			itoa(iNumVertex, szNumVertex, 10);
//			char szText[256];
//			strcpy(szText, "*MESH_VERTEX ");
//			strcat(szText, szNumVertex);
//			strcat(szText, "\t\t");
//			strcat(szText, szx);
//			strcat(szText, "\t");
//			strcat(szText, szy);
//			strcat(szText, "\t");
//			strcat(szText, szz);
//			strcat(szText, "\n");
//			fwrite(szText, strlen(szText), 1, pFile);		
//		}		
//		fwrite("\n\n\n", 3, 1, pFile);
//	}	 
//	fclose(pFile);
//	return;
//}



//----------------------------------------------------------------------------------
//									Export
//----------------------------------------------------------------------------------
void CAHMOLoader::Export( const string& sFileName, const CChunk& chunk)
{
	typedef vector<unsigned int> VectorInt;
	vector<VectorInt*> vBonesIndexArray;		
	//unsigned int * pBonesCount = static_cast<unsigned int*> (chunk.Get("BonesCount"));
	//unsigned int nBonesCount = pBonesCount[0];
	unsigned int nVertexCount = *static_cast<unsigned int*> (chunk.Get("VertexCount"));	

	// On crée un vector de maps contenant les infos sur chaque vertex (nom des bones et poids associés)
	typedef map<string, float> String2Float;
	vector<String2Float> vBonesInfos;
	vector<string>* pvBoneNames = NULL;
	for (unsigned int iVertex=0 ; iVertex<nVertexCount ; iVertex++)
	{	
		// On récupère le nombre de poids du vertex iVertex
		char sziVertex[8];
		_itoa_s(iVertex, sziVertex, 10);
		string strWeightVertexBonesCountDesc = string("WeightVertexBonesCount_") + sziVertex;
		unsigned int* pWeightVertexBonesCount = static_cast<unsigned int*> (chunk.Get(strWeightVertexBonesCountDesc.c_str()) );
		unsigned int nWeightVertexBonesCount = *pWeightVertexBonesCount;

		// On récupère le tableau de noms des bones pondérés du vertex iVertex
		string strWeightVertexDesc = string("BoneWeight_") + sziVertex;
		float* pWeightVertex = static_cast<float*> (chunk.Get(strWeightVertexDesc.c_str()));
		string strBoneNameDesc = string("BoneName_") + sziVertex;
		pvBoneNames = static_cast<vector<string>*> (chunk.Get(strBoneNameDesc.c_str()));
		String2Float mapWeightVertex;
		for ( unsigned int iWeight=0 ; iWeight<nWeightVertexBonesCount ; iWeight++)
			mapWeightVertex.insert(String2Float::value_type((*pvBoneNames)[iWeight], pWeightVertex[iWeight]));
		vBonesInfos.push_back(mapWeightVertex);
	}

	// On crée un tableau par bone et on stocke dans chaque tableau les vertex correspondant 
	// (si un vertex possède plusieurs poids, on le lie au bone de poids le plus fort
	typedef multimap<string, unsigned int> String2Int;
	String2Int mapBonesVertex;	
	for (unsigned int iVertex=0 ; iVertex<nVertexCount; iVertex++)
	{
		String2Float BoneInfo = vBonesInfos[iVertex];	
		float fMaxWeight = 0.f;
		string strMaxWeightBoneName = "none";
		unsigned int nNumMaxWeightVertex = 999999;
		for (String2Float::iterator itBoneInfo = BoneInfo.begin() ; itBoneInfo != BoneInfo.end() ; ++itBoneInfo)
		{			
			string strBoneName = itBoneInfo->first;
			float fWeight = itBoneInfo->second;
			if (fWeight > fMaxWeight)
			{
				fMaxWeight = fWeight;
				strMaxWeightBoneName = strBoneName;
				nNumMaxWeightVertex = iVertex;
			}
		}
		mapBonesVertex.insert(String2Int::value_type(strMaxWeightBoneName, nNumMaxWeightVertex));
	}
	
	
	CChunk* pBonesNameArray = static_cast<CChunk*> (chunk.Get("BonesNameArray"));


	typedef vector<unsigned int> VectorInt;
	vector<VectorInt*> vNewIndexArray;
	
	for ( unsigned int iVertex=0 ; iVertex<pBonesNameArray->GetSize(); iVertex++)
	{
		VectorInt* pNewIndexArray = new VectorInt;
		char szVertex[8];
		_itoa_s(iVertex, szVertex, 10);
		char* szBoneName = static_cast<char*> (pBonesNameArray->Get(szVertex));		
		pair<String2Int::iterator, String2Int::iterator> pairIt = mapBonesVertex.equal_range(szBoneName);
		for (String2Int::iterator itMap = pairIt.first; itMap!=pairIt.second; ++itMap)			
		{
			map<string,int>::iterator it;			
			unsigned int nNumVertex = itMap->second;
			pNewIndexArray->push_back(nNumVertex);
		}
		vNewIndexArray.push_back(pNewIndexArray);
	}

	FILE* pFile = NULL;
	fopen_s( &pFile, sFileName.c_str() , "w");	
	float* pVertexArray = static_cast<float*> (chunk.Get("VertexArray"));
	map<unsigned int, unsigned int> MapNewOldIndex;
	for ( unsigned int iBone=0 ; iBone<vNewIndexArray.size() ; iBone++)
	{
		char szBone[8];
		_itoa_s(iBone, szBone, 10);
		string strBoneName = string("*BONE ") + szBone + " \n";
		fwrite(strBoneName.c_str() , strBoneName.size() , 1, pFile);
		VectorInt* pNewIndexArray = vNewIndexArray[iBone];
		for (unsigned int iIndex=0 ; iIndex<pNewIndexArray->size() ; iIndex++)
		{			
			unsigned int nNewIndex = pNewIndexArray->at(iIndex);
			MapNewOldIndex.insert(map<unsigned int, unsigned int>::value_type( nNewIndex, iIndex) );
			float x = pVertexArray[3*nNewIndex]  ;
			float y = pVertexArray[3*nNewIndex+1];
			float z = pVertexArray[3*nNewIndex+2];

			char szIndex[16], szX[16], szY[16], szZ[16];
			_itoa_s(nNewIndex, szIndex, 10);
			sprintf_s(szX, "%f", x);
			sprintf_s(szY, "%f", y);
			sprintf_s(szZ, "%f", z);

			string strText = string("*MESH_VERTEX ") + szIndex + "\t" + szX + "  " + szY + "  "+ szZ + "\n";			
			fwrite(strText.c_str(), strText.size(), 1, pFile);
		}
		fwrite("\n", strlen("\n"), 1, pFile);
	}


	
	fclose(pFile);
    


	return;



	//	for (unsigned int i=0 ; i<nBonesCount ; i++)
	//	{
	//		
	//		if (nWeightVertexBonesCount > 0 )
	//		{
	//			if (nWeightVertexBonesCount < 2)
	//			{
	//				Map_BoneID_Weight::iterator itFind = boneInfo.find(i);
	//				float fBoneWeight = itFind->second;
	//				unsigned int nBoneID = i;						
	//				vBonesIndexArray[nBoneID]->push_back(iVertex);
	//			}
	//			else
	//			{
	//				// on cherche le poids le plus fort
	//				float fMaxWeight=0.f;
	//				unsigned int nMaxID = 9999;
	//				for (unsigned int j=0 ; j<boneInfo.size() ; j++)
	//				{
	//					Map_BoneID_Weight::iterator itFind = boneInfo.find(j);
	//					float fBoneWeight = itFind->second;
	//					if (fBoneWeight > fMaxWeight)
	//					{
	//						fMaxWeight = fBoneWeight;
	//						nMaxID = j;
	//					}
	//				}
	//				vBonesIndexArray[nMaxID]->push_back(iVertex);					
	//			}
	//		}			
	//	}	
	//

	//// Nettoyage des 2 tableaux (pour éviter les redondances)
	//vector<VectorInt*> vStaticVertex;
	//for (unsigned int i=0 ; i<nBonesCount; i++)
	//{
	//	VectorInt* pVector = new VectorInt;
	//	vStaticVertex.push_back(pVector);
	//}
	//
	//for (unsigned int iNumBone=0 ; iNumBone<nBonesCount ; iNumBone++)
	//{
	//	for (unsigned int iNumVertex=0 ; iNumVertex<vBonesIndexArray[iNumBone]->size()-1 ; iNumVertex++)
	//	{
	//		unsigned nNumVertex = vBonesIndexArray[iNumBone]->at(iNumVertex);
	//		if (nNumVertex != vBonesIndexArray[iNumBone]->at(iNumVertex+1))
	//			vStaticVertex[iNumBone]->push_back(nNumVertex);			
	//	}
	//	int nLastVertexNumber = vBonesIndexArray[iNumBone]->at(vBonesIndexArray[iNumBone]->size()-1);
	//	vStaticVertex[iNumBone]->push_back(nLastVertexNumber);         
	//}	


	//// création des maps (une par bone) associant les anciens numéros de vertex avec les nouveaux

	//typedef map<unsigned int, unsigned int> MapOldNewIndex;
	//vector<MapOldNewIndex*> vMapOldNewIndex;

	//for (unsigned int iNumBone=0 ; iNumBone<nBonesCount ; iNumBone++)
	//{
	//	MapOldNewIndex * pMapOldNewIndex = new MapOldNewIndex;		

	//	for (unsigned int iNumVertex=0 ; iNumVertex<vStaticVertex[iNumBone]->size() ; iNumVertex++)
	//	{
	//		int nNumVertex = vStaticVertex[iNumBone]->at(iNumVertex);	// ancien indice (nouvel indice = iNumVertex)			
	//		pMapOldNewIndex->insert(MapOldNewIndex::value_type(nNumVertex, iNumVertex));
	//	}		
	//	vMapOldNewIndex.push_back(pMapOldNewIndex);
	//}

	//// Création des tableaux de faces statiques
	//vector<VectorInt*> vFaceArray;	
	//unsigned int nIndexCount = *static_cast<unsigned int*> (chunk.Get("IndexCount"));
	//unsigned int nFaceCount = nIndexCount / 3;
	//unsigned int* pIndexArray = static_cast<unsigned int*> (chunk.Get("IndexArray"));
	//for (unsigned int i=0 ; i<nFaceCount; i++)
	//{
	//	unsigned int nIndex0,nIndex1,nIndex2;
	//	unsigned int nIndex0BoneID=-1,nIndex1BoneID=-2,nIndex2BoneID=-3;
	//	nIndex0 = pIndexArray[i*3];
	//	nIndex1 = pIndexArray[i*3+1];
	//	nIndex2 = pIndexArray[i*3+2];
	//	unsigned int nVertexInSameFaceCount = 0;

	//	// On cherche à quel bone est lié chaque index
	//	for (unsigned int j=0 ; j<nBonesCount ; j++)
	//	{
	//		if (nVertexInSameFaceCount != 0)
	//			break;
	//		VectorInt* pFaceArray = new VectorInt;
	//		VectorInt* pNewFaceArray = new VectorInt;
	//		vFaceArray.push_back(pFaceArray);			
	//		VectorInt* pIndex = vStaticVertex[j];
	//		size_t nIndexCount = pIndex->size();
	//		for (size_t k=0 ; k<nIndexCount ; k++)
	//		{
	//			unsigned int nValue = pIndex->at(k);
	//			if (nIndex0 == nValue)	
	//			{
	//				nIndex0BoneID = j;
	//				nVertexInSameFaceCount++;
	//			}
	//			
	//			if (nIndex1 == nValue)		
	//			{
	//				nIndex1BoneID = j;						
	//				nVertexInSameFaceCount++;
	//			}
	//			
	//			if (nIndex2 == nValue)		
	//			{
	//				nIndex2BoneID = j;
	//				nVertexInSameFaceCount++;
	//			}
	//			if ( nVertexInSameFaceCount >= 3 )
	//			{					
	//				vFaceArray[j]->push_back(i);					
	//				break;
	//			}					
	//		}
	//		if ( nVertexInSameFaceCount >= 3 )
	//			break;
	//	}
	//}
	//
	//FILE* pFile = fopen(szFileName, "w");
	//
	//
	//for (unsigned int i=0 ; i< nBonesCount ; i++)
	//{
	//	char szNumBone[8];
	//	itoa(i, szNumBone, 10);
	//	string str = string("*MESH_BONE ") + string(szNumBone) + string("\n\n");
	//	fwrite(str.c_str() , strlen(str.c_str()),1,pFile);

	//	// Ecriture des nouvelles faces 
	//	fwrite("*MESH_FACE_LIST\n", strlen("*MESH_FACE_LIST\n"),1,pFile);
	//	for (unsigned int j=0 ; j<vFaceArray[i]->size() ; j++)
	//	{
	//		char szNumFace[8];
	//		int nNumFace = vFaceArray[i]->at(j);
	//		itoa(nNumFace, szNumFace, 10);
	//		char szText[256];
	//		strcpy(szText, "*MESH_FACE ");
	//		char szNewNumFace[8];
	//		itoa(j, szNewNumFace, 10);
	//		strcat(szText, szNewNumFace);
	//		
	//		unsigned int OldIndex0 = pIndexArray[3*nNumFace];
	//		unsigned int OldIndex1 = pIndexArray[3*nNumFace+1];
	//		unsigned int OldIndex2 = pIndexArray[3*nNumFace+2];
	//		
	//		MapOldNewIndex::iterator itFind0 = vMapOldNewIndex[i]->find(OldIndex0);
	//		MapOldNewIndex::iterator itFind1 = vMapOldNewIndex[i]->find(OldIndex1);
	//		MapOldNewIndex::iterator itFind2 = vMapOldNewIndex[i]->find(OldIndex2);
	//		unsigned int NewIndex0 = itFind0->second;
	//		unsigned int NewIndex1 = itFind1->second;
	//		unsigned int NewIndex2 = itFind2->second;

	//		char szIndex0[8], szIndex1[8], szIndex2[8];
	//		itoa(NewIndex0, szIndex0, 10);
	//		itoa(NewIndex1, szIndex1, 10);
	//		itoa(NewIndex2, szIndex2, 10);
	//		strcat(szText, "	");
	//		strcat(szText, szIndex0);
	//		strcat(szText, "	");
	//		strcat(szText, szIndex1);
	//		strcat(szText, "	");
	//		strcat(szText, szIndex2);
	//		strcat(szText, "\n");
	//		
	//		size_t len = strlen(szText);
	//		fwrite(szText, len, 1, pFile);
	//	}
	//	fwrite("\n\n\n", 3, 1, pFile);

	//	fwrite("*MESH_VERTEX_LIST\n\n", strlen("*MESH_VERTEX_LIST\n"), 1, pFile);


	//	// Ecriture des coordonnées des nouveaux vertex		
	//	float* pVertexArray = static_cast<float*> (chunk.Get("VertexArray"));
	//	for (unsigned int iNumVertex=0 ; iNumVertex<vStaticVertex[i]->size() ; iNumVertex++)
	//	{
	//		int nNumVertex = vStaticVertex[i]->at(iNumVertex);
	//		float x = pVertexArray [nNumVertex*3];
	//		float y = pVertexArray [nNumVertex*3+1];
	//		float z = pVertexArray [nNumVertex*3+2];
	//		char szx[32], szy[23], szz[32];
	//		sprintf(szx, "%f", x);
	//		sprintf(szy, "%f", y);
	//		sprintf(szz, "%f", z);			
	//		char szNumVertex[16];			
	//		itoa(iNumVertex, szNumVertex, 10);
	//		char szText[256];
	//		strcpy(szText, "*MESH_VERTEX ");
	//		strcat(szText, szNumVertex);
	//		strcat(szText, "\t\t");
	//		strcat(szText, szx);
	//		strcat(szText, "\t");
	//		strcat(szText, szy);
	//		strcat(szText, "\t");
	//		strcat(szText, szz);
	//		strcat(szText, "\n");
	//		fwrite(szText, strlen(szText), 1, pFile);		
	//	}		
	//	fwrite("\n\n\n", 3, 1, pFile);
	//}	 
	//fclose(pFile);
	//return;
}


#endif // _TEST_