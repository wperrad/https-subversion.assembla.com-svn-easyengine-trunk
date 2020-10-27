#include "MaxExporter.h"
#include "Resource.h"

// stl
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>

// Max
#include "3dsmaxport.h"
#include "IGame.h"
#include "IGameModifier.h"
#include "Contextids.h"

// Engine
#include "../../Utils2/chunk.h"
#include "math/quaternion.h"
#include "../../Utils2/Node.h"


using namespace std;

HINSTANCE hInstance;

static BOOL showPrompts;
static BOOL exportSelected;


BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
{
   if( fdwReason == DLL_PROCESS_ATTACH )
   {
      hInstance = hinstDLL;
      DisableThreadLibraryCalls(hInstance);
   }

	return (TRUE);
}


__declspec( dllexport ) const TCHAR* LibDescription() 
{
	return  _T( "Lib description" );
}

__declspec( dllexport ) int LibNumberClasses() 
{
	return 1;
}


__declspec( dllexport ) ClassDesc* LibClassDesc(int i) 
{
	switch(i) {
	case 0: return GetAsciiExpDesc();
	default: return 0;
	}
}

__declspec( dllexport ) ULONG LibVersion() 
{
	return VERSION_3DSMAX;
}

__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}

class AsciiExpClassDesc:public ClassDesc {
public:
	int				IsPublic() { return 1; }
	void*			Create(BOOL loading = FALSE) { return new MaxExporter; } 
	const TCHAR*	ClassName() { return _T("AsciiExpClassDesc"); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; } 
	Class_ID		ClassID() { return EASYENGINEEXP_CLASS_ID; }
	const TCHAR*	Category() { return _T("Category"); }
};

static AsciiExpClassDesc AsciiExpDesc;

ClassDesc* GetAsciiExpDesc()
{
	return &AsciiExpDesc;
}


MaxExporter::MaxExporter():
m_nCurrentSmGroup( -1 ),
m_bMultipleSmGroup( false ),
m_bFlipNormal( false )
{
}

MaxExporter::~MaxExporter()
{
}

int MaxExporter::ExtCount()
{
	return 2;
}

const TCHAR * MaxExporter::Ext(int n)
{
	switch( n )
	{
	case 0:
		return _T( "BME" );
	case 1:
		return _T( "BKE" );
	//case 2:
	//	return _T( "BME2" );
	}
	return _T("");
}

const TCHAR * MaxExporter::LongDesc()
{
	return _T( "Binary mesh exporter with skinning informations (vertex weight). Compatible to EasyEngine (Wladimir Perrad)" );
}

const TCHAR * MaxExporter::ShortDesc()
{
	return _T( "Binary mesh export" );
}

const TCHAR * MaxExporter::AuthorName() 
{
	return _T("Wladimir Perrad");
}

const TCHAR * MaxExporter::CopyrightMessage() 
{
	return _T( "Copyright" );
}

const TCHAR * MaxExporter::OtherMessage1() 
{
	return _T("");
}

const TCHAR * MaxExporter::OtherMessage2() 
{
	return _T("");
}

unsigned int MaxExporter::Version()
{
	return 100;
}

void MaxExporter::ShowAbout(HWND hWnd)
{
}


DWORD WINAPI fn(LPVOID arg)
{
	return(0);
}


INT_PTR CALLBACK ExportOptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	static MaxExporter* pExporter;

	switch(message) {
		case WM_INITDIALOG:
			//DLSetWindowLongPtr( hDlg, lParam );
			//SetWindowContextHelpId(hDlg, idh_3dsexp_export);
            pExporter = reinterpret_cast< MaxExporter* >( lParam );
			CenterWindow(hDlg,GetParent(hDlg));
			SetFocus(hDlg); // For some reason this was necessary.  DS-3/4/96
			CheckDlgButton(hDlg, IDC_FLIPNORMALS, FALSE);
			break;
		case WM_DESTROY:
			return FALSE;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDOK:
					pExporter->m_bFlipNormal = ( bool )( IsDlgButtonChecked(hDlg, IDC_FLIPNORMALS) );
					EndDialog(hDlg, 1);
					return TRUE;
				case IDCANCEL:
					EndDialog(hDlg, 0);
					return TRUE;
			}
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_CONTEXTHELP) 
			{
				//DoHelp(HELP_CONTEXT, idh_3dsexp_export);
				return FALSE;
			}
		default:
			return FALSE;
		}
	return FALSE;
}


void MaxExporter::GetWeightTable( CWeightTable2& oWeightTable, const map< string, int >& mBoneID )
{
	IGameScene* pGameScene = GetIGameInterface();
	//bool bInitialise = pGameScene->InitialiseIGame();
	pGameScene->SetStaticFrame(0);
	for ( int i = 0; i < pGameScene->GetTopLevelNodeCount(); i++ )
	{
		IGameNode* pGameNode = pGameScene->GetTopLevelNode( i );
		IGameObject* pGameObject = pGameNode->GetIGameObject();
		IGameSkin* pGameSkin = pGameObject->GetIGameSkin();
		if ( pGameSkin )
		{
			for ( int iVertexIndex = 0; iVertexIndex < pGameSkin->GetNumOfSkinnedVerts(); iVertexIndex++ )
			{
				for ( int iBoneIndex = 0; iBoneIndex < pGameSkin->GetNumberOfBones( iVertexIndex ); iBoneIndex ++ )
				{
					INode* pBone = pGameSkin->GetBone( iVertexIndex, iBoneIndex );
					float fWeight = pGameSkin->GetWeight( iVertexIndex, iBoneIndex );
					map< string, int >::const_iterator itBone = mBoneID.find( pBone->GetName() );
					oWeightTable.Add( iVertexIndex, itBone->second, fWeight );
				}
			}
			break;
		}
	}
}

void MaxExporter::GetSkeleton( INode* pRoot, map< string, INode* >& mBone )
{
	for ( int iNode = 0; iNode < pRoot->NumberOfChildren(); iNode++ )
	{
		INode* pNode = pRoot->GetChildNode( iNode );
		Object* pObject = pNode->EvalWorldState( 0 ).obj;
		if ( pObject->CanConvertToType( Class_ID( BONE_CLASS_ID, 0 ) ) == TRUE || pObject->CanConvertToType( BONE_OBJ_CLASSID ) == TRUE )
		{
			//CNode* pEENode = new CNode;
			//MaxNodeToEngineNode( *pNode, *pEENode );
			mBone[ pNode->GetName() ] = pNode;
			GetSkeleton( pNode, mBone );
		}
	}
}

void MaxExporter::GetSkeleton( INode* pRoot, std::map< std::string, CNode* >& mBone )
{
}

void MaxExporter::MaxMatrixToEngineMatrix( const Matrix3& mMax, CMatrix& mEngine )
{
	Point3& row0 = mMax.GetRow( 0 );
	mEngine.m_00 = row0.x;
	mEngine.m_10 = row0.y;
	mEngine.m_20 = row0.z;

	Point3& row1 = mMax.GetRow( 1 );
	mEngine.m_01 = row1.x;
	mEngine.m_11 = row1.y;
	mEngine.m_21 = row1.z;

	Point3& row2 = mMax.GetRow( 2 );
	mEngine.m_02 = row2.x;
	mEngine.m_12 = row2.y;
	mEngine.m_22 = row2.z;

	Point3& row3 = mMax.GetRow( 3 );
	mEngine.m_03 = row3.x;
	mEngine.m_13 = row3.y;
	mEngine.m_23 = row3.z;
}

void MaxExporter::GetAnimation( Interface* pInterface, const map< int, INode* >& mBone, map< int, vector< CKey > >& mBones )
{
	bool bTCB = false;
	//IGameScene* pGameScene = GetIGameInterface();
	//bool bInitialise = pGameScene->InitialiseIGame();

	TimeValue iStart = pInterface->GetAnimRange().Start();
	TimeValue iEnd = pInterface->GetAnimRange().End();

	CMatrix oWorldNodeTM, oParentTM, oInverseParentTM;

	for ( map< int, INode* >::const_iterator itNode = mBone.begin(); itNode != mBone.end(); ++itNode )
	{
		INode* pNode = itNode->second;
		Control* pControl = pNode->GetTMController()->GetRotationController();
		IKeyControl* pKc = GetKeyControlInterface( pControl );
		if ( pKc )
		{
			int nNumKeys = pKc->GetNumKeys();
			for ( int iKey = 0; iKey < nNumKeys; iKey++ )
			{
				ITCBRotKey key;
				pKc->GetKey( iKey, &key );
				Matrix3 m = pNode->GetNodeTM( key.time );
				MaxMatrixToEngineMatrix( m, oWorldNodeTM );
				if ( pNode->GetParentNode() )
				{
					Matrix3 mParentTM = pNode->GetParentNode()->GetNodeTM( key.time );
					MaxMatrixToEngineMatrix( mParentTM, oParentTM );
					oParentTM.GetInverse( oInverseParentTM );
				}
				CKey oKey;
				oKey.m_nTime = key.time;
				oKey.m_oWorldTM = oWorldNodeTM;
				oKey.m_oLocalTM = oInverseParentTM * oWorldNodeTM;
				oKey.m_oAngleAxis = key.val;
				CQuaternion qLocal;
				oKey.m_oLocalTM.GetQuaternion( qLocal );
				qLocal.m_vPosition = oKey.m_oLocalTM.GetPosition();
				oKey.m_qLocal = qLocal;
				mBones[ itNode->first ].push_back( oKey );
			}
			bTCB = true;
		}
	}
	if ( bTCB == false )
		MessageBoxA( NULL, "Votre animation ne comporte pas de controleurs TCB, elle n'a pas été exportée", "", MB_ICONWARNING );
}


BOOL MaxExporter::TMNegParity( Matrix3 &m )
{
	return ( DotProd( CrossProd( m.GetRow(0), m.GetRow(1) ), m.GetRow(2)) < 0.0 ) ? 1 : 0;
}

void MaxExporter::GetGeometry( CChunk& oChunk, INode* pRoot )
{
	for ( int iNode = 0; iNode < pRoot->NumberOfChildren(); iNode++ )
	{
		INode* pNode = pRoot->GetChildNode( iNode );
		Matrix3 tm = pNode->GetObjTMAfterWSM(0);
		const char* pName = pNode->GetName();
		Object* pObject = pNode->EvalWorldState( 0 ).obj;
		if ( pObject->CanConvertToType( Class_ID( TRIOBJ_CLASS_ID, 0 ) ) == TRUE )
		{
			if ( pObject->CanConvertToType( Class_ID( BONE_CLASS_ID, 0 ) ) == TRUE )
				break;
			if ( pObject->CanConvertToType( BONE_OBJ_CLASSID ) == TRUE )
				break;
			TriObject* pTriObject = static_cast< TriObject* > ( pObject->ConvertToType( 0, Class_ID( TRIOBJ_CLASS_ID, 0 ) ) );
			Mesh& oMesh = pTriObject->GetMesh();
			oMesh.buildNormals();
			vector< float >* pVertexArray = new vector< float >;
			vector< float >* pFaceNormalArray = new vector< float >;
			vector< float >* pVertexNormalArray = new vector< float >;
			vector< int >* pIndexArray = new vector< int >;
			vector< float >* pTVertexArray = NULL;
			vector< int >* pTIndexArray = NULL;new vector< int >;

			string* pBitmapName = NULL;
			Mtl* pMaterial = pNode->GetMtl();
			if ( pMaterial )
			{
				vector< float >* pAmbiant = new vector< float >;
				for ( int iAmbiant = 0; iAmbiant < 3; iAmbiant++ )
					pAmbiant->push_back( ((float*)pMaterial->GetAmbient())[ iAmbiant ] );
				pAmbiant->push_back( 1.f );

				vector< float >* pDiffuse = new vector< float >;
				for ( int iDiffuse = 0; iDiffuse < 3; iDiffuse++ )
					pDiffuse->push_back( ((float*)pMaterial->GetDiffuse())[ iDiffuse ] );
				pDiffuse->push_back( 1.f );

				vector< float >* pSpecular = new vector< float >;
				float fShineStr = pMaterial->GetShinStr();
				for ( int iSpecular = 0; iSpecular < 3; iSpecular++ )
					pSpecular->push_back( fShineStr * ((float*)pMaterial->GetSpecular())[ iSpecular ]  / 9.99f );
				pSpecular->push_back( 1.f );

				vector< float >* pShininess = new vector< float >;
				pShininess->push_back( pMaterial->GetShininess() );

				string* pMatName = new string;
				*pMatName = pMaterial->GetName();
				Texmap* pTexmap = pMaterial->GetSubTexmap( 1 );
				if ( pTexmap )
				{
					BitmapTex* pTexture = ( BitmapTex* ) pTexmap;
					if ( pTexture )
					{
						pBitmapName = new string;
						*pBitmapName = pTexture->GetMapName();
						int nLastSlashPos = pBitmapName->find_last_of( "\\" );
						*pBitmapName = pBitmapName->substr( nLastSlashPos + 1, pBitmapName->size() - nLastSlashPos );
						oChunk.Add( pBitmapName, "BitmapName" );
					}
				}

				oChunk.Add( pAmbiant, "Ambient" );
				oChunk.Add( pDiffuse, "Diffuse" );
				oChunk.Add( pSpecular, "Specular" );
				oChunk.Add( pShininess, "Shininess" );
				oChunk.Add( pMatName, "ShaderName" );
			}

			for ( int iVertex = 0; iVertex < oMesh.getNumVerts(); iVertex ++ )
			{
				Point3& oVertex = tm * oMesh.verts[ iVertex ];
				pVertexArray->push_back( oVertex.x );
				pVertexArray->push_back( oVertex.y );
				pVertexArray->push_back( oVertex.z );
			}

			for ( int iFace = 0; iFace < oMesh.getNumFaces(); iFace++ )
				for ( int iIndex = 0; iIndex < 3; iIndex++ )
					pIndexArray->push_back( oMesh.faces[ iFace ].v[ iIndex ] );
			
			for ( int iFaceNormal = 0; iFaceNormal < oMesh.getNumFaces(); iFaceNormal++ )
			{
				Point3& oNormal = oMesh.getFaceNormal( iFaceNormal );
				pFaceNormalArray->push_back( oNormal.x );
				pFaceNormalArray->push_back( oNormal.y );
				pFaceNormalArray->push_back( oNormal.z );
				
				Face& f = oMesh.faces[ iFaceNormal ];
				for ( int vx = 0; vx < 3; vx++ )
				{
					Point3& oVertexNormal = GetVertexNormal( oMesh, iFaceNormal, oMesh.getRVertPtr( f.getVert( vx ) ) );
					pVertexNormalArray->push_back( oVertexNormal.x );
					pVertexNormalArray->push_back( oVertexNormal.y );
					pVertexNormalArray->push_back( oVertexNormal.z );
				}
			}
			
			if ( pBitmapName != NULL )
			{
				pTVertexArray = new vector< float >;
				for ( int iTVertex = 0; iTVertex < oMesh.getNumTVerts(); iTVertex++ )
				{					
					UVVert& oTVert = oMesh.getTVert( iTVertex );
					pTVertexArray->push_back( oTVert.x );
					pTVertexArray->push_back( oTVert.y );
					//pTVertexArray->push_back( oTVert.z );
				}				
				pTIndexArray = new vector< int >;
				for ( int iTFace = 0; iTFace < oMesh.getNumFaces(); iTFace++ )
					for ( int iTIndex = 0; iTIndex < 3; iTIndex++ )
						pTIndexArray->push_back( oMesh.tvFace[ iTFace ].t[ iTIndex ] );
				oChunk.Add( pTVertexArray, "TVertexArray" );
				oChunk.Add( pTIndexArray, "TIndexArray" );
			}
			
			try
			{
				oChunk.Add( pVertexArray, "VertexArray" );
				oChunk.Add( pFaceNormalArray, "FaceNormalArray" );
				oChunk.Add( pVertexNormalArray, "VertexNormalArray" );
				oChunk.Add( pIndexArray, "IndexArray" );
			}
			catch( exception& e )
			{
				MessageBoxA( NULL, e.what(), "", MB_ICONERROR );
			}
		}
	}
}

//void MaxExporter::GetBonesIDByName( const map< string, INode* > mBone, map< string, int >& mBoneIDByName ) const
//{
//	int nBoneID = 0;
//	for ( map< string, INode* >::const_iterator itNode = mBone.begin(); itNode!= mBone.end(); itNode++ )
//	{
//		mBoneIDByName[ itNode->first ] = nBoneID;
//		nBoneID++;
//	}
//}

void MaxExporter::GetBonesIDByName( INode* pRoot, map< string, int >& mBoneIDByName ) const
{
	Object* pObject = pRoot->EvalWorldState( 0 ).obj;
	if ( pObject && ( pObject->CanConvertToType( Class_ID( BONE_CLASS_ID, 0 ) ) == TRUE || pObject->CanConvertToType( BONE_OBJ_CLASSID ) == TRUE ) )
		mBoneIDByName[ pRoot->GetName() ] = mBoneIDByName.size();
	for( int i = 0; i < pRoot->NumberOfChildren(); i++ )
		GetBonesIDByName( pRoot->GetChildNode( i ), mBoneIDByName );
}


void GetMtlList( vector< Mtl* >& vMtl, INode* pNode )
{
	vMtl.push_back( pNode->GetMtl() );
	for ( int iChild = 0; iChild < pNode->NumChildren(); iChild++ )
		GetMtlList( vMtl, pNode->GetChildNode( iChild ) );
}

void MaxExporter::GetBoneByID( const map< string, INode* >& mBoneByName, const map< string, int >& mBoneIDByName, map< int, INode* >& mBoneByID )
{
	for ( map< string, int >::const_iterator itNameID = mBoneIDByName.begin(); itNameID != mBoneIDByName.end(); ++itNameID )
	{
		const map< string, INode* >::const_iterator itNode = mBoneByName.find( itNameID->first );
		mBoneByID[ itNameID->second ] = itNode->second;
	}
}
//
//int MaxExporter::GetRootCount( CNode* pNode )
//{
//	if ( pNode->GetParent() == NULL )
//	{
//		int nRootCount = 1;
//		for ( int i = 0; i < pNode->GetchildCount(); i++ )
//			nRootCount += GetRootCount( pNode->Getchild( i ) );
//		return nRootCount;
//	}
//	else
//		return 0;
//}

void MaxExporter::MaxNodeToEngineNode( INode& oMaxNode, CNode& oNode )
{
	CMatrix m;
	MaxMatrixToEngineMatrix( oMaxNode.GetNodeTM(0), m );
	oNode.SetWorldMatrix( m );
}

int MaxExporter::DoExport( const TCHAR *pName, ExpInterface *pExpInterface, Interface *pInterface, BOOL suppressPrompts, DWORD options )
{
	string sFileName = pName;
	int nDotPos = sFileName.find_last_of( "." );
	string sExtension = sFileName.substr( nDotPos + 1, sFileName.size() - nDotPos - 1 );
	string sExtensionLower = sExtension;
	transform( sExtension.begin(), sExtension.end(), sExtensionLower.begin(), tolower );
	sExtension = sExtensionLower;

	try
	{
		map< string, INode* > mBones;
		map< string, int > mBoneIDByName;
		map< int, INode* > mBoneByID;
		INode* pRoot = pInterface->GetRootNode();
		GetSkeleton( pRoot, mBones );
		GetBonesIDByName( pRoot, mBoneIDByName );
		GetBoneByID( mBones, mBoneIDByName, mBoneByID );
		if ( sExtension == "bme" )
		{
			DialogBoxParam( GetModuleHandle(NULL), MAKEINTRESOURCEA(IDD_DIALOGEXPORTEE), pInterface->GetMAXHWnd(), ExportOptionsDlgProc, (LPARAM)this );
			CWeightTable2 oWeightTable;
			GetWeightTable( oWeightTable, mBoneIDByName );
			CChunk oChunkGeom;
			GetGeometry( oChunkGeom, pInterface->GetRootNode() );
			DumpModel( pName, oChunkGeom, oWeightTable.m_mVertex, mBoneByID );
		}
		if ( sExtension == "bke" )
		{
			map< int, vector< CKey > > mBonesKey;
			GetAnimation( pInterface, mBoneByID, mBonesKey );
			DumpAnimation( sFileName, mBonesKey );
		}
	}
	
	catch( exception& e )
	{
		MessageBoxA( NULL, e.what(), "", MB_ICONERROR );
	}
	return TRUE;
}

Point3 MaxExporter::GetVertexNormal( Mesh& oMesh, int faceNo, RVertex* rv )
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
	else if ((numNormals = rv->rFlags & NORCT_MASK) != 0 && smGroup) {
		// If there is only one vertex is found in the rn member.
		if (numNormals == 1) {
			vertexNormal = rv->rn.getNormal();
		}
		else {
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (int i = 0; i < numNormals; i++) {
				if (rv->ern[i].getSmGroup() & smGroup) {
					vertexNormal = rv->ern[i].getNormal();
				}
			}
		}
	}
	else {
		// Get the normal from the Face if no smoothing groups are there
		vertexNormal = oMesh.getFaceNormal(faceNo);
	}
	return vertexNormal;
}

void MaxExporter::DumpMatrix( FILE* pFile, const Matrix3& m )
{
	vector< float > vMatrix;
	for ( int iRow = 0; iRow < 4; iRow++ )
	{
		Point3& row = m.GetRow( iRow );
		vMatrix.push_back( row.x );
		vMatrix.push_back( row.y );
		vMatrix.push_back( row.z );
		if ( iRow == 3 )
			vMatrix.push_back( 1.f );
		else
			vMatrix.push_back( 0.f );
	}
	fwrite( &vMatrix[ 0 ], sizeof(float), 16, pFile );
}

void MaxExporter::DumpMatrix( FILE* pFile, const CMatrix& m ) const
{
	float pArray[ 16 ];
	m.Get( pArray );
	fwrite( pArray, sizeof( float ), 16, pFile );
}

void MaxExporter::DumpQuaternion( FILE* pFile, const CQuaternion& q ) const
{
	fwrite( &q.m_x, sizeof( float ), 1, pFile );
	fwrite( &q.m_y, sizeof( float ), 1, pFile );
	fwrite( &q.m_z, sizeof( float ), 1, pFile );
	fwrite( &q.m_w, sizeof( float ), 1, pFile );
	fwrite( &q.m_vPosition.m_x, sizeof( float ), 1, pFile );
	fwrite( &q.m_vPosition.m_y, sizeof( float ), 1, pFile );
	fwrite( &q.m_vPosition.m_z, sizeof( float ), 1, pFile );
	fwrite( &q.m_vPosition.m_w, sizeof( float ), 1, pFile );
}

void MaxExporter::DumpSkeleton( FILE* pFile, const map< int, INode* >& mNodeID )
{
	int nBoneCount = mNodeID.size();
	fwrite( &nBoneCount, sizeof(int), 1, pFile );
	int iNode = 0;
	int nRootCount = 0;
	for ( map< int, INode* >::const_iterator itNode = mNodeID.begin(); itNode != mNodeID.end(); ++itNode,iNode++ )
	{
		fwrite( &itNode->first, sizeof(int), 1, pFile );
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
		if ( nParentID == -1 )
			nRootCount++;
		fwrite( &nParentID, sizeof(int), 1, pFile );
		Matrix3 m = itNode->second->GetNodeTM(0);
		DumpMatrix( pFile, m );
	}
	if ( nRootCount > 1 )
		MessageBox( NULL, "Attention, le squelette de votre modèle comporte plus de 1 bone racine.", "", MB_ICONWARNING );
}

void MaxExporter::DumpSkinning( FILE* pFile, const std::map< int, std::map< int, float > >& mWeightVertexID )
{
	int nWeightVertexCount = mWeightVertexID.size();
	fwrite( &nWeightVertexCount, sizeof(int), 1, pFile );
	for ( int iVertex = 0; iVertex < nWeightVertexCount; iVertex++ )
	{
		map< int, map< int, float > >::const_iterator itVertex = mWeightVertexID.find( iVertex );
		const map< int, float >& mBone = itVertex->second;
		int iBoneCount = mBone.size();
		fwrite( &iBoneCount, sizeof(int), 1, pFile );
		for ( map< int, float >::const_iterator itBone = mBone.begin(); itBone != mBone.end(); ++itBone )
		{
			int nID = itBone->first;
			fwrite ( &nID, sizeof(float), 1, pFile );
			fwrite ( &itBone->second, sizeof(float), 1, pFile );
		}
	}
}

void MaxExporter::DumpModel( const string& sFilePath, const CChunk& oChunk, 
							 const map< int, map< int, float > >& mWeightVertexID, const map< int, INode* >& mNodeID  )
{
	vector< float >* pAmbiant = reinterpret_cast< vector< float >* > ( oChunk.Get( "Ambient" ) );
	vector< float >* pDiffuse = reinterpret_cast< vector< float >* > ( oChunk.Get( "Diffuse" ) );
	vector< float >* pSpecular = reinterpret_cast< vector< float >* > ( oChunk.Get( "Specular" ) );
	vector< float >* pShininess = reinterpret_cast< vector< float >* > ( oChunk.Get( "Shininess" ) );
	string* pMatName = reinterpret_cast< string* > ( oChunk.Get( "ShaderName" ) );
	string* pBitmapName = reinterpret_cast< string* > ( oChunk.Get( "BitmapName" ) );

	vector< float >* pVertexArray = reinterpret_cast< vector< float >* > ( oChunk.Get( "VertexArray" ) );
	vector< int >* pIndexArray = reinterpret_cast< vector< int >* > ( oChunk.Get( "IndexArray" ) );
	vector< float >* pNormalFaceArray = reinterpret_cast< vector< float >* > ( oChunk.Get( "FaceNormalArray" ) );
	vector< float >* pNormalVertexArray = reinterpret_cast< vector< float >* > ( oChunk.Get( "VertexNormalArray" ) );
	vector< float >* pTVertexArray = reinterpret_cast< vector< float >* > ( oChunk.Get( "TVertexArray" ) );
	vector< int >* pTIndexArray = reinterpret_cast< vector< int >* > ( oChunk.Get( "TIndexArray" ) );

	FILE* pFile = NULL;
	fopen_s( &pFile, sFilePath.c_str(), "wb" );
	if ( !pFile )
	{
		string sMessage = "Erreur à l'ouverture du fichier " + sFilePath;
		exception e( sMessage.c_str() );
		throw e;
	}

	int nMat = 0;
	if ( pAmbiant == NULL || pDiffuse == NULL || pSpecular == NULL || pShininess == NULL )
		fwrite( &nMat, sizeof(int), 1, pFile );
	else
	{
		nMat = 1;
		fwrite( &nMat, sizeof(int), 1, pFile );
		fwrite( &( *pAmbiant )[ 0 ], sizeof(float), 4, pFile );
		fwrite( &( *pDiffuse )[ 0 ], sizeof(float), 4, pFile );
		fwrite( &( *pSpecular )[ 0 ], sizeof(float), 4, pFile );
		fwrite( &( *pShininess )[ 0 ], sizeof(float), 1, pFile );
		int nShaderNameSize = pMatName->size();
		fwrite( &nShaderNameSize, sizeof(int), 1, pFile );
		fwrite( pMatName->c_str(), sizeof(char), nShaderNameSize, pFile );
		int nTextureNameSize = 0;
		if ( pBitmapName )
		{
			nTextureNameSize = pBitmapName->size();
			fwrite( &nTextureNameSize, sizeof(int), 1, pFile );
			fwrite( pBitmapName->c_str(), sizeof(char), pBitmapName->size(), pFile );
		}
		else
			fwrite( &nTextureNameSize, sizeof(int), 1, pFile );
	}

	int iVertexSize = pVertexArray->size();
	fwrite( &iVertexSize, sizeof(int), 1, pFile ); 
	fwrite( &( *pVertexArray )[ 0 ], sizeof(float), pVertexArray->size(), pFile );

	int iIndexSize = pIndexArray->size();
	fwrite( &iIndexSize, sizeof(int), 1, pFile );
	fwrite( &( *pIndexArray )[ 0 ], sizeof(int), pIndexArray->size(), pFile );

	int iNormalFaceSize = pNormalFaceArray->size();
	fwrite( &iNormalFaceSize, sizeof(int), 1, pFile );
	fwrite( &( *pNormalFaceArray )[ 0 ], sizeof(float), pNormalFaceArray->size(), pFile );

	int iNormalVertexSize = pNormalVertexArray->size();
	fwrite( &iNormalVertexSize, sizeof(int), 1, pFile );
	fwrite( &( *pNormalVertexArray )[ 0 ], sizeof(float), pNormalVertexArray->size(), pFile );

	if ( pTVertexArray )
	{
		int iTVertexSize = pTVertexArray->size();
		fwrite( &iTVertexSize, sizeof(int), 1, pFile );
		fwrite( &( *pTVertexArray )[ 0 ], sizeof(float), pTVertexArray->size(), pFile );

		int iTIndexSize = pTIndexArray->size();
		fwrite( &iTIndexSize, sizeof(int), 1, pFile );
		fwrite( &( *pTIndexArray )[ 0 ], sizeof(float), pTIndexArray->size(), pFile );
	}

	DumpSkeleton( pFile, mNodeID );
	DumpSkinning( pFile, mWeightVertexID );

	int nCanBeIndexed = (int)( m_bMultipleSmGroup == false && pBitmapName == NULL );
	//if ( nBoneCount > 0 ) 
	//	nCanBeIndexed = 1;
	fwrite( &nCanBeIndexed, sizeof(int), 1, pFile );
	fclose( pFile );
}

void MaxExporter::DumpAngleAxis( FILE* pFile, const AngAxis& a ) const
{
	float fAngle = a.angle;
	fwrite( &fAngle, sizeof(float), 1, pFile );
	float x = a.axis.x;
	float y = a.axis.y;
	float z = a.axis.z;
	fwrite( &x, sizeof(float), 1, pFile );
	fwrite( &y, sizeof(float), 1, pFile );
	fwrite( &z, sizeof(float), 1, pFile );
}

void MaxExporter::DumpAnimation( const string& sFilePath, const map< int, vector< CKey > >& mBones )
{
	FILE* pFile = NULL;
	fopen_s( &pFile, sFilePath.c_str(), "wb" );
	if ( !pFile )
	{
		string sMessage = "Erreur à l'ouverture du fichier " + sFilePath;
		exception e( sMessage.c_str() );
		throw e;
	}
	int nBoneCount = mBones.size();
	fwrite( &nBoneCount, sizeof(int), 1, pFile );

	map< int, vector< CKey > >::const_iterator itBone = mBones.begin();
	// On crée un squelette qui contient tous les bones	
	for ( ; itBone != mBones.end(); ++itBone )
	{
		unsigned int nBoneID = itBone->first;
		fwrite( &nBoneID, sizeof(unsigned int), 1, pFile );
		const vector< CKey >& vKey = itBone->second;
		unsigned int nKeyCount = vKey.size();
		fwrite( &nKeyCount, sizeof(unsigned int), 1, pFile );
		vector< CKey >::const_iterator itKey = vKey.begin();
		for ( ; itKey != vKey.end(); ++itKey )
		{
			unsigned int nTimeValue = itKey->m_nTime;
			fwrite( &nTimeValue, sizeof(unsigned int), 1, pFile );
			DumpMatrix( pFile, itKey->m_oLocalTM );
			DumpAngleAxis( pFile, itKey->m_oAngleAxis );
			DumpQuaternion( pFile, itKey->m_qLocal );
		}
	}
	fclose( pFile );
}

BOOL MaxExporter::SupportsOptions( int ext, DWORD options )
{
	assert( ext == 0 );
	return( options == SCENE_EXPORT_SELECTED ) ? TRUE : FALSE;
}

CWeightTable::CWeightTable()
{
	IGameScene* pGameScene = GetIGameInterface();
	//bool bInitialise = pGameScene->InitialiseIGame();
	pGameScene->SetStaticFrame(0);
	for ( int i = 0; i < pGameScene->GetTopLevelNodeCount(); i++ )
	{
		IGameNode* pGameNode = pGameScene->GetTopLevelNode( i );
		IGameObject* pGameObject = pGameNode->GetIGameObject();
		IGameSkin* pGameSkin = pGameObject->GetIGameSkin();
		if ( pGameSkin )
		{
			for ( int iVertexIndex = 0; iVertexIndex < pGameSkin->GetNumOfSkinnedVerts(); iVertexIndex++ )
			{
				for ( int iBoneIndex = 0; iBoneIndex < pGameSkin->GetNumberOfBones( iVertexIndex ); iBoneIndex ++ )
				{
					INode* pBone = pGameSkin->GetBone( iVertexIndex, iBoneIndex );
					float fWeight = pGameSkin->GetWeight( iVertexIndex, iBoneIndex );
					//map< string, int >::const_iterator itBone = mBoneID.find( pBone->GetName() );
					m_mVertex[ iVertexIndex ][ pBone->GetName() ] = fWeight;
				}
			}
			break;
		}
	}
}


CWeightTable2::CWeightTable2()
{
}

CWeightTable2::~CWeightTable2()
{
}

void CWeightTable2::Add( int iVertexIndex, int nBoneID, float fBoneWeight )
{
	m_mVertex[ iVertexIndex ][ nBoneID ] = fBoneWeight;
}

void CWeightTable2::Get( int iVertexIndex, std::map< int, float >& mWeight ) const
{
	map< int, map< int, float > >::const_iterator itMapWeight = m_mVertex.find( iVertexIndex );
	mWeight = itMapWeight->second;
}

int CWeightTable2::GetVertexCount() const
{
	return m_mVertex.size();
}