#include "BinaryAnimationMaxExporter.h"
#include "Resource.h"

// stl
#include <string>
#include <sstream>
#include "Utils/IStorage.h"

using namespace std;

#define BINARYANIMATIONPLUGIN_CLASS_ID	Class_ID(0x85548e0e, 0x4a26450f)

//CBinaryAnimationMaxExporterClassDesc oDesc;

void* CBinaryAnimationMaxExporterClassDesc::Create( BOOL loading )
{
	return new CBinaryAnimationMaxExporter;
}

Class_ID CBinaryAnimationMaxExporterClassDesc::ClassID() 
{ 
	return BINARYANIMATIONPLUGIN_CLASS_ID; 
}

int CBinaryAnimationMaxExporter::ExtCount()
{
	return 1;
}

const TCHAR* CBinaryAnimationMaxExporter::Ext( int n )
{
	if( n == 0 )
		return L"bke";
	return L"";
}

CBinaryAnimationMaxExporter::CBinaryAnimationMaxExporter()
{
}

CBinaryAnimationMaxExporter* g_pExporter = NULL;

INT_PTR CALLBACK CBinaryAnimationMaxExporter::OnExportAnim(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	wchar_t pFirst[ 16 ], pLast[ 16 ];

	wstring wFirst, wLast, wText, wText2;
	string sFirst, sLast, sText, sText2;

	HWND hFirst = GetDlgItem( hWnd, ID_EDITFIRST );
	HWND hLast = GetDlgItem( hWnd, ID_EDITLAST );
	ostringstream oss;

	switch( msg )
	{
	case WM_INITDIALOG:
		g_pExporter = (CBinaryAnimationMaxExporter*)lParam;
		CenterWindow( hWnd, GetParent( hWnd ) );
		oss << g_pExporter->m_nAnimationStart;
		sText = oss.str().c_str();
		wText.assign(sText.begin(), sText.end());
		SetWindowText( hFirst, wText.c_str());
		oss.str( "" );
		oss << g_pExporter->m_nAnimationEnd;
		sText2.assign(oss.str().c_str());
		wText2.assign(sText2.begin(), sText2.end());
		SetWindowText( hLast, wText2.c_str() );
		break;
	case WM_CLOSE:
		EndDialog( hWnd, 0 );
		break;
	case WM_COMMAND:
		switch( wParam )
		{
		case IDEXPORTANIM:
			GetWindowText( hFirst, pFirst, 16 );
			GetWindowText( hLast, pLast, 16 );

			wFirst = pFirst;
			wLast = pLast;
			sFirst.assign(wFirst.begin(), wFirst.end());
			sLast.assign(wLast.begin(), wLast.end());
			g_pExporter->m_nExportAnimationStart = atoi(sFirst.c_str());
			g_pExporter->m_nExportAnimationEnd = atoi(sLast.c_str());
			EndDialog( hWnd, 1 );
			break;
		case IDCANCELANIM:
			//g_pExporter->m_bCancelExport = true;
			EndDialog( hWnd, 0 );
			break;
		}
		break;
	}
	return 0;
}

int CBinaryAnimationMaxExporter::DoExport(const TCHAR* pName,ExpInterface *ei, Interface *pInterface, BOOL suppressPrompts, DWORD options )
{
	try
	{
		wstring wFileName = pName;
		map< int, INode* > mBoneByID;
		map< string, INode* > mBones;
		map< string, int > mBoneIDByName;
		
		GetSkeleton( pInterface->GetRootNode(), mBones );
		GetBonesIDByName( pInterface->GetRootNode(), mBoneIDByName );
		GetBoneByID( mBones, mBoneIDByName, mBoneByID );

		
		m_nExportAnimationStart = m_nAnimationStart = pInterface->GetAnimRange().Start() / g_nTickPerFrame;
		m_nExportAnimationEnd = m_nAnimationEnd = pInterface->GetAnimRange().End() / g_nTickPerFrame;

		if( DialogBoxParam( hInstance, MAKEINTRESOURCE( IDD_ANIMEXPORT ), pInterface->GetMAXHWnd(), OnExportAnim, (LPARAM)this ) == 1 )
		{
			map< int, vector< CKey > > mBonesKey;
			GetAnimation( pInterface, mBoneByID, mBonesKey );
			string sFileName(wFileName.begin(), wFileName.end());
			DumpAnimation(sFileName, mBonesKey );

			if( m_vNonTCBBoneNames.size() > 0 )
			{
				wstring wWarningMessage = L"Les bones suivants contiennent des clés non TCB, elles ne peuvent donc pas être exportées : \n ";
				for (unsigned int i = 0; i < m_vNonTCBBoneNames.size(); i++) {
					wWarningMessage += m_vNonTCBBoneNames[i] + L"\n";
				}
				MessageBox( NULL, wWarningMessage.c_str(), L"Avertissement", MB_ICONWARNING );
			}
			MessageBox( NULL, L"Export terminé", L"Export", MB_OK );
		}
	}
	catch( exception& e )
	{
		string msg(e.what());
		wstring wmsg(msg.begin(), msg.end());
		MessageBox( NULL, wmsg.c_str(), L"", MB_ICONERROR );
	}

	return TRUE;
}


void CBinaryAnimationMaxExporter::GetSkeleton( INode* pRoot, map< string, INode* >& mBone )
{
	for ( int iNode = 0; iNode < pRoot->NumberOfChildren(); iNode++ )
	{
		INode* pNode = pRoot->GetChildNode( iNode );
		Object* pObject = pNode->EvalWorldState( 0 ).obj;
		if  ( IsBone( pObject ) ) //( pObject->CanConvertToType( Class_ID( BONE_CLASS_ID, 0 ) ) == TRUE || pObject->CanConvertToType( BONE_OBJ_CLASSID ) == TRUE )
		{
			wstring wName(pNode->GetName());
			string sName(wName.begin(), wName.end());
			mBone[sName] = pNode;
			GetSkeleton( pNode, mBone );
		}
	}
}

void CBinaryAnimationMaxExporter::GetBonesIDByName( INode* pRoot, map< string, int >& mBoneIDByName ) const
{
	Object* pObject = pRoot->EvalWorldState( 0 ).obj;
	if (pObject && IsBone(pObject)) {
		wstring wName(pRoot->GetName());
		string sName(wName.begin(), wName.end());
		mBoneIDByName[sName] = (int)mBoneIDByName.size();
	}
	for( int i = 0; i < pRoot->NumberOfChildren(); i++ )
		GetBonesIDByName( pRoot->GetChildNode( i ), mBoneIDByName );
}

void CBinaryAnimationMaxExporter::GetBoneByID( const map< string, INode* >& mBoneByName, const map< string, int >& mBoneIDByName, map< int, INode* >& mBoneByID )
{
	for ( map< string, int >::const_iterator itNameID = mBoneIDByName.begin(); itNameID != mBoneIDByName.end(); ++itNameID )
	{
		const map< string, INode* >::const_iterator itNode = mBoneByName.find( itNameID->first );
		mBoneByID[ itNameID->second ] = itNode->second;
	}
}

void CBinaryAnimationMaxExporter::DumpAnimation( string sFilePath, const map< int, vector< CKey > >& mBones )
{
#pragma message ("Exporter de la même maniere que les mesh et gérer les versions de fichiers")
	int iStartTime = m_nExportAnimationStart * g_nTickPerFrame;
	int iEndTime = m_nExportAnimationEnd * g_nTickPerFrame;

	CBinaryFileStorage fs;
	
	if ( !fs.OpenFile( sFilePath, IFileStorage::eWrite ) )
	{
		string sMessage = string( "Erreur à l'ouverture du fichier " ) + sFilePath;
		exception e( sMessage.c_str() );
		throw e;
	}
	fs << iStartTime << iEndTime << (int)mBones.size();

	map< int, vector< CKey > >::const_iterator itBone = mBones.begin();
	// On crée un squelette qui contient tous les bones	
	for ( ; itBone != mBones.end(); ++itBone )
	{
		const vector< CKey >& vKey = itBone->second;
		fs << (unsigned int)itBone->first << (unsigned int)vKey.size();
		vector< CKey >::const_iterator itKey = vKey.begin();
		for ( ; itKey != vKey.end(); ++itKey )
		{
			fs << (unsigned int)itKey->m_nTimeValue << (int)itKey->m_eType;
			fs << itKey->m_oLocalTM << itKey->m_oWorldTM;
			DumpAngleAxis( fs, itKey->m_oAngleAxis );
			fs << itKey->m_qLocal;
		}
	}
	fs.CloseFile();
}

#pragma message( "supprimer l'appel à CBinaryAnimationMaxExporter::DumpAngleAxis" )
void CBinaryAnimationMaxExporter::DumpAngleAxis( IFileStorage& fs, const AngAxis& a )
{
	fs << (float)a.angle << (float)a.axis.x << (float)a.axis.y << (float)a.axis.z;
}
