#include "BinaryMeshMaxExporter.h"
#include "IFileSystem.h"
#include "EEPlugin.h"

// stl
#include <algorithm>

// MAX
#include "3dsmaxport.h"


#define BINARYMESHPLUGIN_CLASS_ID	Class_ID(0x85548e0c, 0x4a26450d)

CBinaryMeshMaxExporter* CBinaryMeshMaxExporter::s_pCurrentInstance = NULL;


void* CBinaryMeshMaxExporterClassDesc::Create( BOOL loading )
{
	return new CBinaryMeshMaxExporter;
}

Class_ID CBinaryMeshMaxExporterClassDesc::ClassID() 
{ 
	return BINARYMESHPLUGIN_CLASS_ID; 
}

CBinaryMeshMaxExporter::CBinaryMeshMaxExporter() :
m_bMultipleSmGroup( false ),
m_nCurrentSmGroup( -1 ),
m_bFlipNormals( false ),
m_bOpenglCoord( false ),
m_nMaterialCount( 0 ),
m_bLog( false ),
m_bExportSkinning( true ),
m_pLogFile( NULL ),
m_bExportBoundingBox( true ),
m_bExportBBoxAtKey( false )
{
	m_vExtension.push_back( "bme" );
	s_pCurrentInstance = this;
}

CBinaryMeshMaxExporter::~CBinaryMeshMaxExporter()
{
	s_pCurrentInstance = NULL;
}

int CBinaryMeshMaxExporter::ExtCount()
{
	return (int)m_vExtension.size();;
}

const TCHAR* CBinaryMeshMaxExporter::Ext( int n )
{
	return m_vExtension[ n ].c_str();
}

INT_PTR CALLBACK CBinaryMeshMaxExporter::ExportDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int nExportSkinningCheck = -1;
	int nExportBoundingBox = -1;
	int nExportBBoxAtKey = -1;
	switch (msg) 
	{
	case WM_INITDIALOG:
		CenterWindow( hWnd, GetParent( hWnd ) );
		nExportSkinningCheck = s_pCurrentInstance->m_bExportSkinning ? BST_CHECKED : BST_UNCHECKED;
		SendMessageA( GetDlgItem(hWnd, IDC_CHECKSKINNING), BM_SETCHECK, nExportSkinningCheck, 0 );
		nExportBoundingBox = s_pCurrentInstance->m_bExportBoundingBox ? BST_CHECKED : BST_UNCHECKED;
		nExportBBoxAtKey = s_pCurrentInstance->m_bExportBBoxAtKey ? BST_CHECKED : BST_UNCHECKED;
		SendMessageA( GetDlgItem( hWnd, IDC_CHECKBOUNDINGBOX ), BM_SETCHECK, nExportBoundingBox, 0 );
		SendMessageA( GetDlgItem( hWnd, IDC_CHECKEXPORTBBOXATKEY ), BM_SETCHECK, nExportBBoxAtKey, 0 );
		break;
	case WM_CLOSE:
		EndDialog( hWnd, 0 );
		break;
	case WM_COMMAND:
		switch( wParam )
		{
		case IDEXPORT:
			s_pCurrentInstance->m_bFlipNormals = SendMessageA( GetDlgItem(hWnd, IDC_CHECKFLIPNORMALS), BM_GETCHECK, 0, 0 );
			s_pCurrentInstance->m_bOpenglCoord = SendMessageA( GetDlgItem(hWnd, IDC_CHECKOPENGLCOORD), BM_GETCHECK, 0, 0 );
			s_pCurrentInstance->m_bExportSkinning = SendMessageA( GetDlgItem(hWnd, IDC_CHECKSKINNING), BM_GETCHECK, 0, 0 );
			s_pCurrentInstance->m_bLog = SendMessageA( GetDlgItem(hWnd, IDC_CHECK_LOG), BM_GETCHECK, 0, 0 );
			s_pCurrentInstance->m_bExportBoundingBox = SendMessageA( GetDlgItem(hWnd, IDC_CHECKBOUNDINGBOX ), BM_GETCHECK, 0, 0 );
			s_pCurrentInstance->m_bExportBBoxAtKey = SendMessageA( GetDlgItem(hWnd, IDC_CHECKEXPORTBBOXATKEY ), BM_GETCHECK, 0, 0 );
			if( s_pCurrentInstance->m_bLog )
			{
				s_pCurrentInstance->m_pLogFile = fopen( "c:\\easyEngineExport.log", "a" );
				SYSTEMTIME st;
				GetSystemTime( &st );
				ostringstream oss;
				oss << "\n-------------------------------------------------------------------------------------\n";
				oss << st.wDay << "/" << st.wMonth << "/" << st.wYear << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << "\n";
				fwrite( s_pCurrentInstance->m_pLogFile, sizeof( char ), oss.str().size(), s_pCurrentInstance->m_pLogFile );
			}
			EndDialog( hWnd, 1 );
			break;
		case IDCANCEL:
			EndDialog( hWnd, 0 );
			break;
		}
		break;
	}
	return 0;
}       

int	CBinaryMeshMaxExporter::DoExport(const TCHAR *pName, ExpInterface *ei, Interface *pInterface, BOOL suppressPrompts, DWORD options )
{
	string sFileName = pName;
	int nDotPos = (int)sFileName.find_last_of( "." );
	string sExtension = sFileName.substr( nDotPos + 1, sFileName.size() - nDotPos - 1 );
	string sExtensionLower = sExtension;
	transform( sExtension.begin(), sExtension.end(), sExtensionLower.begin(), tolower );
	sExtension = sExtensionLower;

	try
	{
		map< string, INode* > mBones;
		
		map< int, INode* > mBoneByID;
		INode* pRoot = pInterface->GetRootNode();
		GetSkeleton( pRoot, mBones );
		GetBonesIDByName( pRoot, m_mBoneIDByName );
		GetBoneByID( mBones, m_mBoneIDByName, mBoneByID );
		HWND hMaxDlg = pInterface->GetMAXHWnd();
		INT_PTR iRet = DialogBoxParam( hInstance, MAKEINTRESOURCE( IDD_EXPORT ), hMaxDlg, ExportDlgProc, (LPARAM)this );
		if( iRet == 1 )
		{
			//vector< ILoader::CMeshInfos > vMeshInfos;
			ILoader::CAnimatableMeshData mi;
			StoreSkeletonToSkeletonMap( mBoneByID, mi );
			GetGeometry( pInterface, mi.m_vMeshes, pInterface->GetRootNode() );
			mi.m_mBonesBoundingBoxes = m_mBoneBox;
			if( DumpModels( pName, mi ) )
				MessageBox( NULL, "Export terminé", "Export", MB_OK );
			if( m_pLogFile )
				fclose( m_pLogFile );
		}
	}	
	catch( exception& e )
	{
		MessageBoxA( NULL, e.what(), "", MB_ICONERROR );
	}
	return TRUE;
}

void CBinaryMeshMaxExporter::GetSkeleton( INode* pRoot, map< string, INode* >& mBone )
{
	for ( int iNode = 0; iNode < pRoot->NumberOfChildren(); iNode++ )
	{
		INode* pNode = pRoot->GetChildNode( iNode );
		Object* pObject = pNode->EvalWorldState( 0 ).obj;
		if ( IsBone( pObject ) )
		{
			mBone[ pNode->GetName() ] = pNode;
			GetSkeleton( pNode, mBone );
		}
	}
}

void CBinaryMeshMaxExporter::GetBonesIDByName( INode* pRoot, map< string, int >& mBoneIDByName ) const
{
	Object* pObject = pRoot->EvalWorldState( 0 ).obj;
	if ( pObject && IsBone( pObject ) ) //( pObject->CanConvertToType( Class_ID( BONE_CLASS_ID, 0 ) ) == TRUE || pObject->CanConvertToType( BONE_OBJ_CLASSID ) == TRUE ) )
		mBoneIDByName[ pRoot->GetName() ] = (int)mBoneIDByName.size();
	for( int i = 0; i < pRoot->NumberOfChildren(); i++ )
		GetBonesIDByName( pRoot->GetChildNode( i ), mBoneIDByName );
}

void CBinaryMeshMaxExporter::GetBoneByID( const map< string, INode* >& mBoneByName, const map< string, int >& mBoneIDByName, map< int, INode* >& mBoneByID )
{
	for ( map< string, int >::const_iterator itNameID = mBoneIDByName.begin(); itNameID != mBoneIDByName.end(); ++itNameID )
	{
		const map< string, INode* >::const_iterator itNode = mBoneByName.find( itNameID->first );
		mBoneByID[ itNameID->second ] = itNode->second;
	}
}

void CBinaryMeshMaxExporter::GetWeightTable( IWeightTable& oWeightTable, const map< string, int >& mBoneID, string sObjectName )
{
	WriteLog( "\nCBinaryMeshMaxExporter::GetWeightTable() : debut" );
	
	IGameScene* pGameScene = GetIGameInterface();
	bool bInitialise = pGameScene->InitialiseIGame();
	pGameScene->SetStaticFrame(0);
	for ( int i = 0; i < pGameScene->GetTopLevelNodeCount(); i++ )
	{
		IGameNode* pGameNode = pGameScene->GetTopLevelNode( i );
		string sNodeName = pGameNode->GetName();
		if( sNodeName != sObjectName )
			continue;
		IGameObject* pGameObject = pGameNode->GetIGameObject();
		IGameSkin* pGameSkin = pGameObject->GetIGameSkin();
		if ( pGameSkin )
		{
			for ( int iVertexIndex = 0; iVertexIndex < pGameSkin->GetNumOfSkinnedVerts(); iVertexIndex++ )
			{
				int nBoneCount = 0;
				for ( int iBoneIndex = 0; iBoneIndex < pGameSkin->GetNumberOfBones( iVertexIndex ); iBoneIndex ++ )
				{
					INode* pBone = pGameSkin->GetBone( iVertexIndex, iBoneIndex );
					float fWeight = pGameSkin->GetWeight( iVertexIndex, iBoneIndex );
					if( fWeight > 0.f )
					{
						map< string, int >::const_iterator itBone = mBoneID.find( pBone->GetName() );
						oWeightTable.Add( iVertexIndex, itBone->second, fWeight );
						nBoneCount++;
					}
				}
				if( nBoneCount > 4 )
				{
					ostringstream oss;
					oss << "Erreur : le vertex " << iVertexIndex << " est influencé par plus de 4 bones";
					CEException e( oss.str() );
					throw e;
				}
			}
			break;
		}
	}
	WriteLog( "\nCBinaryMeshMaxExporter::GetWeightTable() : fin" );
}

void CBinaryMeshMaxExporter::WriteLog( string sMessage )
{
	if( m_bLog )
		fwrite( sMessage.c_str(), sizeof( char ), sMessage.size(), m_pLogFile );
}

/*
void CBinaryMeshMaxExporter::GetBoundingBoxAtKey( Interface *pInterface, const vector< float >& vVertexArray, const vector< unsigned int >& vIndexArray, const IWeightTable& wt, map< int, CBox >& mBox )
{
	map< string, INode* > mBones;
	map< string, int > mBoneIDByName;
	map< int, INode* > mBoneByID;

	GetSkeleton( pInterface->GetRootNode(), mBones );
	GetBonesIDByName( pInterface->GetRootNode(), mBoneIDByName );
	GetBoneByID( mBones, mBoneIDByName, mBoneByID );

	map< int, vector < CKey > > mBoneKeys;
	GetAnimation( pInterface, mBoneByID, mBoneKeys );

	map< int, int > mKeys;
	for( map< int, vector < CKey > >::iterator itBone = mBoneKeys.begin(); itBone != mBoneKeys.end(); itBone++ )
	{
		vector< CKey >& vKey = itBone->second;
		for( int i = 0; i < vKey.size(); i++ )
			mKeys[ vKey[ i ].m_nTimeValue ] = i;
	}

	for( map< int, int >::iterator itKey = mKeys.begin(); itKey != mKeys.end(); itKey++ )
	{
		CBox oBox;
		for( int i = 0; i < vIndexArray.size(); i++ )
		{
			int iIndex = vIndexArray[ i ];
			CVector v = CVector( vVertexArray[ 3 * iIndex ], vVertexArray[ 3 * iIndex + 1 ], vVertexArray[ 3 * iIndex + 2 ] );
			map< int, float > mWeight;
			wt.Get( iIndex, mWeight );
			CMatrix oBoneMatrixInit, oBoneMatrixFinal, oBoneMatrixInitInv;
			for( map< int, float >::iterator itBone = mWeight.begin(); itBone != mWeight.end(); itBone++ )
			{
				oBoneMatrixInit = oBoneMatrixInit * mBoneKeys[ itBone->first ][ 0 ].m_oWorldTM * itBone->second;
				int nKeyIndex = 0;
				for( int iKey = 0; iKey < mBoneKeys[ itBone->first ].size(); iKey++ )
				{
					if( mBoneKeys[ itBone->first ][ iKey ].m_nTimeValue == itKey->first )
					{
						nKeyIndex = iKey;
						break;
					}
				}
				oBoneMatrixFinal = oBoneMatrixFinal * mBoneKeys[ itBone->first ][ nKeyIndex ].m_oWorldTM * itBone->second;
			}
			oBoneMatrixInit.GetInverse( oBoneMatrixInitInv );
			CMatrix oBoneMatrixPassage = oBoneMatrixFinal * oBoneMatrixInitInv;
			CVector vTransform = oBoneMatrixPassage * v;
			oBox.AddPoint( vTransform );
		}
		mBox[ itKey->first ] = oBox;
	}
}
*/

void CBinaryMeshMaxExporter::GetGeometry( Interface* pInterface, vector< ILoader::CMeshInfos >& vMeshInfos, INode* pRoot )
{
	for ( int iNode = 0; iNode < pRoot->NumberOfChildren(); iNode++ )
	{
		INode* pNode = pRoot->GetChildNode( iNode );
		string sTest = pNode->GetName();
		Object* pObject = pNode->EvalWorldState( 0 ).obj;
		if( IsBone( pObject ) )
		{
			GetMeshesIntoHierarchy( pInterface, pNode, vMeshInfos );
			continue;
		}
		if ( pObject->CanConvertToType( Class_ID( TRIOBJ_CLASS_ID, 0 ) ) == TRUE )
		{
			ILoader::CMeshInfos mi;
			StoreMeshToMeshInfos( pInterface, pNode, mi );
			vMeshInfos.push_back( mi );
		}
	}
}

void CBinaryMeshMaxExporter::GetMeshesIntoHierarchy( Interface* pInterface, INode* pNode, vector< ILoader::CMeshInfos >& vMeshInfos )
{
	Object* pObject = pNode->EvalWorldState( 0 ).obj;
	if( IsBone( pObject ) )
	{
		for( int iBone = 0; iBone < pNode->NumberOfChildren(); iBone++ )
			GetMeshesIntoHierarchy( pInterface, pNode->GetChildNode( iBone ), vMeshInfos );
	}
	else
	{
		if ( pObject->CanConvertToType( Class_ID( TRIOBJ_CLASS_ID, 0 ) ) == TRUE )
		{
			ILoader::CMeshInfos mi;
			StoreMeshToMeshInfos( pInterface, pNode, mi );
			vMeshInfos.push_back( mi );
		}
	}

}

void CBinaryMeshMaxExporter::GetBonesBoundingBoxes( const Mesh& oMesh, const IWeightTable& oWeightTable, const Matrix3& oModelTM, map< int, CBox >& mBoneBox )
{
	for ( int iVertex = 0; iVertex < oMesh.getNumVerts(); iVertex ++ )
	{
		Point3& oVertex = oModelTM * oMesh.verts[ iVertex ];
		map< int, float > mBoneWeights;
		oWeightTable.Get( iVertex, mBoneWeights );
		for( map< int, float >::const_iterator itBone = mBoneWeights.begin(); itBone != mBoneWeights.end(); itBone++ )
		{
			if( itBone->second > 0.5f )
			{
				CVector v( oVertex.x, oVertex.y, oVertex.z );
				mBoneBox[ itBone->first ].AddPoint( v );
			}
		}
	}
}

void CBinaryMeshMaxExporter::StoreMeshToMeshInfos( Interface* pInterface, INode* pMesh, ILoader::CMeshInfos& mi )
{	
	WriteLog( "\nCBinaryMeshMaxExporter::StoreMeshToChunk() : debut" );
	Object* pObject = pMesh->EvalWorldState( 0 ).obj;
	TriObject* pTriObject = static_cast< TriObject* > ( pObject->ConvertToType( 0, Class_ID( TRIOBJ_CLASS_ID, 0 ) ) );
	Mesh& oMesh = pTriObject->GetMesh();
	mi.m_sName = pMesh->GetName();
	bool bIsTextured = false;

	if( m_bLog )
	{
		string sMessage = "\nCBinaryMeshMaxExporter::StoreMeshToChunk() : objet \"" + mi.m_sName + "\"";
		WriteLog( sMessage );
	}

	Matrix3 oTM = pMesh->GetObjTMAfterWSM(0);

	IWeightTable* pWeightTable = NULL;
	if( m_bExportSkinning )
	{
		IGeometryManager::Desc oGMDesc( NULL, "" );
		IGeometryManager* pGeometryManager = static_cast< IGeometryManager* >( CPlugin::Create( oGMDesc, "Geometry.dll", "CreateGeometryManager" ) );
		pWeightTable =  pGeometryManager->CreateWeightTable();
		GetWeightTable( *pWeightTable, m_mBoneIDByName, mi.m_sName );
		pWeightTable->GetArrays( mi.m_vWeightVertex, mi.m_vWeigtedVertexID );
	}


	if( m_bExportBoundingBox && pWeightTable->GetVertexCount() > 0 )
		GetBonesBoundingBoxes( oMesh, *pWeightTable, oTM, m_mBoneBox );
	MaxMatrixToEngineMatrix( oTM, mi.m_oWorldTM );
	oMesh.buildNormals();
	mi.m_nParentBoneID = -1;
	INode* pParent = pMesh->GetParentNode();
	if( pParent && IsBone( pParent ) )
	{
		map< string, int >::iterator itBone = m_mBoneIDByName.find( pParent->GetName() );
		if( itBone == m_mBoneIDByName.end() )
		{
			CEException e( "Erreur dans l'exporteur, tous les nodes de la hiérarchy ne sont pas récupérés correctement" );
			throw e;
		}
		mi.m_nParentBoneID = itBone->second;
	}

	Mtl* pMaterial = pMesh->GetMtl();
	if ( pMaterial )
	{
		StoreMaxMaterialToMaterialInfos( pMaterial, mi.m_oMaterialInfos );
		if( m_nMaterialCount > 1 ) 
			mi.m_bMultiMaterial = true;
		mi.m_oMaterialInfos.m_bExists = true;
		string sDiffuseMapName;
		bIsTextured = ( mi.m_oMaterialInfos.m_sDiffuseMapName != "NONE" );
	}
	else
		mi.m_oMaterialInfos.m_bExists = false;

	float xMin = 0, xMax = 0, yMin = 0, yMax = 0, zMin = 0, zMax = 0;
	CMatrix mTransform( -1, 0, 0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1 );
	
	for ( int iVertex = 0; iVertex < oMesh.getNumVerts(); iVertex ++ )
	{
		Point3& oVertex = oTM * oMesh.verts[ iVertex ];
		if( m_bOpenglCoord )
		{
			CVector v( oVertex.x, oVertex.y, oVertex.z );
			CVector v2 = mTransform * v;
			oVertex.x = v2.m_x;
			oVertex.y = v2.m_y;
			oVertex.z = v2.m_z;
		}
		mi.m_vVertex.push_back( oVertex.x );
		mi.m_vVertex.push_back( oVertex.y );
		mi.m_vVertex.push_back( oVertex.z );
		if( oVertex.x > xMax )xMax = oVertex.x;
		if( oVertex.x < xMin )xMin = oVertex.x;
		if( oVertex.y > yMax )yMax = oVertex.y;
		if( oVertex.y < yMin )yMin = oVertex.y;
		if( oVertex.z > zMax )zMax = oVertex.z;
		if( oVertex.z < zMin )zMin = oVertex.z;
	}
	mi.m_oBoundingBox.Set( CVector( xMin, yMin, zMin ), CVector( xMax - xMin, yMax - yMin, zMax - zMin ) );

	for ( int iFace = 0; iFace < oMesh.getNumFaces(); iFace++ )
	{
		if( m_bFlipNormals )
		{
			mi.m_vIndex.push_back( oMesh.faces[ iFace ].v[ 1 ] );
			mi.m_vIndex.push_back( oMesh.faces[ iFace ].v[ 0 ] );
			mi.m_vIndex.push_back( oMesh.faces[ iFace ].v[ 2 ] );
		}
		else
		{
			for ( int iIndex = 0; iIndex < 3; iIndex++ )
			{
				if( m_bOpenglCoord )
					mi.m_vIndex.push_back( oMesh.faces[ iFace ].v[ 2 - iIndex ] );
				else
					mi.m_vIndex.push_back( oMesh.faces[ iFace ].v[ iIndex ] );
			}
		}
	}
	if( m_bExportBBoxAtKey && pWeightTable && pWeightTable->GetVertexCount() > 0 )
		//GetBoundingBoxAtKey( pInterface, mi.m_vVertex, mi.m_vIndex, *pWeightTable, mi.m_oKeyBoundingBoxes );
	{
		CEException e( "L'export des bounding box par clé d'animation n'est pas gérée parl'exporteur max, vous pouvez utiliser cette fonctionnalité dans EasyEngine." );
		throw e;
	}
	GetFacesMtlArray( oMesh, mi.m_vFaceMaterialID );
	GetNormals( oMesh, mi.m_vNormalFace, mi.m_vNormalVertex );
	if ( bIsTextured )
	{
		for ( int iTVertex = 0; iTVertex < oMesh.getNumTVerts(); iTVertex++ )
		{					
			UVVert& oTVert = oMesh.getTVert( iTVertex );
			mi.m_vUVVertex.push_back( oTVert.x );
			mi.m_vUVVertex.push_back( oTVert.y );
		}				
		for ( int iTFace = 0; iTFace < oMesh.getNumFaces(); iTFace++ )
			for ( int iTIndex = 0; iTIndex < 3; iTIndex++ )
				mi.m_vUVIndex.push_back( oMesh.tvFace[ iTFace ].t[ iTIndex ] );
	}
	
	mi.m_bCanBeIndexed = (int)( ( !m_bMultipleSmGroup ) && ( !bIsTextured ) && ( m_nMaterialCount <= 1 ) );
	m_nMaterialCount = 0;
	m_bMultipleSmGroup = false;
	WriteLog( "\nCBinaryMeshMaxExporter::StoreMeshToChunk() : fin" );
}

void CBinaryMeshMaxExporter::GetFacesMtlArray( Mesh& oMesh, std::vector< unsigned short >& vMtlIDArray )
{
	MtlID id0 = oMesh.faces[ 0 ].getMatID();
	for ( int iFace = 0; iFace < oMesh.getNumFaces(); iFace++ )
	{
		int nID = oMesh.faces[ iFace ].getMatID();
		vMtlIDArray.push_back( nID );
	}
}

void CBinaryMeshMaxExporter::GetNormals( Mesh& oMesh, std::vector< float >& vFaceNormal, std::vector< float >& vVertexNormal )
{
	vector<int> vIndex;
	float fNormalFactor = 1.f;
	if( m_bFlipNormals )
	{
		fNormalFactor = -1;
		vIndex.push_back( 1 );
		vIndex.push_back( 0 );
		vIndex.push_back( 2 );
	}
	else
	{
		vIndex.push_back( 0 );
		vIndex.push_back( 1 );
		vIndex.push_back( 2 );
	}
	CMatrix mTransform( -1, 0, 0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1 );
	for ( int iFaceNormal = 0; iFaceNormal < oMesh.getNumFaces(); iFaceNormal++ )
	{
		Point3& oNormal = oMesh.getFaceNormal( iFaceNormal );
		if( m_bOpenglCoord )
		{
			CVector n( oNormal.x, oNormal.y, oNormal.z );
			CVector n2 = mTransform * n;
			oNormal.x = n2.m_x;
			oNormal.y = n2.m_y;
			oNormal.z = n2.m_z;
		}
		vFaceNormal.push_back( fNormalFactor * oNormal.x );
		vFaceNormal.push_back( fNormalFactor * oNormal.y );
		vFaceNormal.push_back( fNormalFactor * oNormal.z );
		Face& f = oMesh.faces[ iFaceNormal ];
		for ( int vx = 0; vx < 3; vx++ )
		{
			Point3& oVertexNormal = GetVertexNormal( oMesh, iFaceNormal, oMesh.getRVertPtr( f.getVert( vIndex[ vx ] ) ) );
			if( m_bOpenglCoord )
			{
				CVector n( oVertexNormal.x, oVertexNormal.y, oVertexNormal.z );
				CVector n2 = mTransform * n;
				oVertexNormal.x = n2.m_x;
				oVertexNormal.y = n2.m_y;
				oVertexNormal.z = n2.m_z;
			}
			vVertexNormal.push_back( fNormalFactor * oVertexNormal.x );
			vVertexNormal.push_back( fNormalFactor * oVertexNormal.y );
			vVertexNormal.push_back( fNormalFactor * oVertexNormal.z );
		}
	}
}

void CBinaryMeshMaxExporter::GetMaterialTextureName( Mtl* pMaterial, string& sTextureName, int nMapIndex ) const
{
	Texmap* pTexmap = pMaterial->GetSubTexmap( nMapIndex );
	if ( pTexmap )
	{
		BitmapTex* pTexture = ( BitmapTex* ) pTexmap;
		if ( pTexture )
		{
			sTextureName = pTexture->GetMapName();
			int nLastSlashPos = (int)sTextureName.find_last_of( "\\" );
			sTextureName = sTextureName.substr( nLastSlashPos + 1, sTextureName.size() - nLastSlashPos );
		}
	}
	if ( sTextureName.size() == 0 )
		sTextureName = "NONE";
}

void CBinaryMeshMaxExporter::UpdateVersionFile( string sVersion )
{	
	string sFileVersionPath = "version.ver";
	FILE* pVersionFile = NULL;
	int nDepth = 0;
	do
	{
		pVersionFile = fopen( sFileVersionPath.c_str(), "w" );
		sFileVersionPath = string( "..\\" ) + sFileVersionPath;
		nDepth++;
	}
	while( !pVersionFile && nDepth < 10 );
 	fwrite( sVersion.c_str(), sizeof( char ), sVersion.size(), pVersionFile );
	fclose( pVersionFile );
}

bool CBinaryMeshMaxExporter::DumpModels( const string& sFilePath, ILoader::CAnimatableMeshData& ami )
{
	bool bRet = true;
	string sVersion = OtherMessage1();
	UpdateVersionFile( sVersion );

	ami.m_sFileVersion = sVersion;

	IFileSystem::Desc oFSDesc( NULL, "" );
	IFileSystem* pFileSystem = static_cast< IFileSystem* >( CPlugin::Create( oFSDesc, "FileUtils.dll", "CreateFileSystem" ) );
	ILoaderManager::Desc oLDesc( *pFileSystem );
	ILoaderManager* pLoaderManager = static_cast< ILoaderManager* >(  CPlugin::Create( oLDesc, "Loader2.dll", "CreateLoaderManager" ) );
	pLoaderManager->Export( sFilePath, ami );
	return bRet;
}

void CBinaryMeshMaxExporter::StoreMaxMaterialToMaterialInfos( Mtl* pMaterial, ILoader::CMaterialInfos& mi )
{
	m_nMaterialCount++;
	StoreMaxColorToVector( pMaterial->GetAmbient(), mi.m_vAmbient );
	StoreMaxColorToVector( pMaterial->GetDiffuse(), mi.m_vDiffuse );
	StoreMaxColorToVector( pMaterial->GetSpecular(), mi.m_vSpecular );
	mi.m_fShininess = pMaterial->GetShininess() * 128.f;
	mi.m_sName = pMaterial->GetName();
	GetMaterialTextureName( pMaterial, mi.m_sDiffuseMapName, 1 );
	int nMtlCount = 0, iMtl = 0;
	Mtl* pSubMtl = NULL;
	while( pSubMtl = pMaterial->GetSubMtl( iMtl++ ) )nMtlCount++;
	mi.m_vSubMaterials.resize( nMtlCount );
	for( iMtl = 0; iMtl < nMtlCount; iMtl++ )
	{
		MSTR sSlotName = pMaterial->GetSubMtlSlotName( iMtl );
		int nID = (int)( sSlotName[ 1 ] - '0' );
		pSubMtl = pMaterial->GetSubMtl( iMtl );
		StoreMaxMaterialToMaterialInfos( pSubMtl, mi.m_vSubMaterials[ iMtl ] );
	}
}

void CBinaryMeshMaxExporter::StoreMaxColorToVector( const Color c, vector< float >& v )
{
	v.push_back( c.r );
	v.push_back( c.g );
	v.push_back( c.b );
	v.push_back( 1 );
}

Point3 CBinaryMeshMaxExporter::GetVertexNormal( Mesh& oMesh, int faceNo, RVertex* rv )
{
	Face& f = oMesh.faces[faceNo];
	DWORD smGroup = f.smGroup;
	if ( m_nCurrentSmGroup == -1 )
		m_nCurrentSmGroup = smGroup;
	else
		if ( m_nCurrentSmGroup != smGroup )
			m_bMultipleSmGroup = true;
	int numNormals = 0;
	Point3 vertexNormal;
	
	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if ( rv->rFlags & SPECIFIED_NORMAL) 
		vertexNormal = rv->rn.getNormal();

	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if ( ( numNormals = rv->rFlags & NORCT_MASK) != 0 && smGroup ) 
	{
		// If there is only one vertex is found in the rn member.
		if ( numNormals == 1 ) 
		{
			vertexNormal = rv->rn.getNormal();
		}
		else 
		{
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (int i = 0; i < numNormals; i++) 
			{
				if (rv->ern[i].getSmGroup() & smGroup) 
				{
					vertexNormal = rv->ern[i].getNormal();
				}
			}
		}
	}
	else 
	{
		// Get the normal from the Face if no smoothing groups are there
		vertexNormal = oMesh.getFaceNormal(faceNo);
	}
	return vertexNormal;
}

void CBinaryMeshMaxExporter::StoreSkeletonToSkeletonMap( const map< int, INode* >& mNodeID, ILoader::CAnimatableMeshData& ami )
{
	int nRootCount = 0;
	for ( map< int, INode* >::const_iterator itNode = mNodeID.begin(); itNode != mNodeID.end(); ++itNode )
	{
		ami.m_mBones.insert( ILoader::TSkeletonMap::value_type( itNode->first, pair< string, CMatrix >() ) );
		
		int nParentID = -1;
		INode* pParent = itNode->second->GetParentNode();
		CMatrix mLocal, mParentLocal;

		for ( map< int, INode* >::const_iterator itParent = mNodeID.begin(); itParent != mNodeID.end(); ++itParent )
		{
			if ( pParent == itParent->second )
			{
				nParentID = itParent->first;
				break;
			}
		}

		ami.m_mHierarchyBones[ itNode->first ] = nParentID;
		if ( nParentID == -1 )
			nRootCount++;


		ami.m_mBones[ itNode->first ].first = itNode->second->GetName();
		Matrix3 oMaxMatrix = itNode->second->GetNodeTM(0);
		CMatrix oMatrix;
		MaxMatrixToEngineMatrix( oMaxMatrix, ami.m_mBones[ itNode->first ].second );

	}

	if ( nRootCount > 1 )
		MessageBox( NULL, "Attention, le squelette de votre modèle comporte plus de 1 bone racine.", "", MB_ICONWARNING );
}

void CBinaryMeshMaxExporter::StoreSkinningToMeshInfos( const IWeightTable& wt, ILoader::CMeshInfos& mi )
{
	int nWeightVertexCount = wt.GetVertexCount();
	for ( int iVertex = 0; iVertex < nWeightVertexCount; iVertex++ )
	{
		map< int, float > mBone;
		if( wt.Get( iVertex, mBone ) )
		{
			for ( map< int, float >::const_iterator itBone = mBone.begin(); itBone != mBone.end(); ++itBone )
			{
				mi.m_vWeigtedVertexID.push_back( itBone->first );
				mi.m_vWeightVertex.push_back( itBone->second );
			}
			for ( int iSupBone = (int)mBone.size(); iSupBone < 4; iSupBone++ )
			{
				mi.m_vWeigtedVertexID.push_back( -1.f );
				mi.m_vWeightVertex.push_back( 0.f );
			}
		}
		else
		{
			CEException e("Erreur : Tous les vertex de votre modèle ne sont pas skinnés");
			throw e;
		}
	}
}