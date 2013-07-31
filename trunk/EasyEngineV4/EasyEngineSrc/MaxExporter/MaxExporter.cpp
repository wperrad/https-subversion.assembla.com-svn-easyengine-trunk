#include "MaxExporter.h"
#include "3dsmaxport.h"
#include <sstream>

using namespace std;

HINSTANCE hInstance;


// Class ID. These must be unique and randomly generated!!
// If you use this as a sample project, this is the first thing
// you should change!
//#define TESTINTERFACEPLUGIN_CLASS_ID	Class_ID(0x85548e0c, 0x4a26450d)


CMaxExporterClassDesc::CMaxExporterClassDesc()
{
}

int CMaxExporterClassDesc::IsPublic() 
{ 
	return 1; 
}

const TCHAR* CMaxExporterClassDesc::ClassName() 
{ 
	return "MaxExporter"; 
}

SClass_ID CMaxExporterClassDesc::SuperClassID() 
{ 
	return SCENE_EXPORT_CLASS_ID; 
}

const TCHAR* CMaxExporterClassDesc::Category() 
{ 
	return _T("Category"); 
}

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;

	return NULL;
}

CMaxExporter::CMaxExporter()
{
}

CMaxExporter::~CMaxExporter()
{
}

const TCHAR * CMaxExporter::LongDesc()
{
	return "Easy engine export (BME for geometry and BKE for animation)";
}

const TCHAR * CMaxExporter::ShortDesc()
{
	return "Easy engine export";
}

const TCHAR * CMaxExporter::AuthorName() 
{
	return _T("Wladimir Perrad");
}

const TCHAR * CMaxExporter::CopyrightMessage() 
{
	return "Anti propriété intellectuelle";
}

const TCHAR * CMaxExporter::OtherMessage1()
{
	ostringstream ossVersion;
	ossVersion << __DATE__ << __TIME__;
	m_sVersion = ossVersion.str();
	return m_sVersion.c_str();
}

const TCHAR * CMaxExporter::OtherMessage2() 
{
	return _T("");
}

unsigned int CMaxExporter::Version()
{
	return 100;
}

static INT_PTR CALLBACK AboutBoxDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch (msg) {
	case WM_INITDIALOG:
		CenterWindow(hWnd, GetParent(hWnd)); 
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			EndDialog(hWnd, 1);
			break;
		}
		break;
		default:
			return FALSE;
	}
	return TRUE;
}       

void CMaxExporter::ShowAbout(HWND hWnd)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutBoxDlgProc, 0);
}

bool CMaxExporter::IsBone( Object* pObject )
{
	return ( pObject->CanConvertToType( Class_ID( BONE_CLASS_ID, 0 ) ) == TRUE || 
		pObject->CanConvertToType( BONE_OBJ_CLASSID ) == TRUE ||
		pObject->CanConvertToType( Class_ID( DUMMY_CLASS_ID, 0 ) ) == TRUE );
}

bool CMaxExporter::IsBone( INode* pNode )
{
	Object* pObject = pNode->EvalWorldState( 0 ).obj;
	if( !pObject )
		return false;
	return IsBone( pObject );
}

   

// Dummy function for progress bar
DWORD WINAPI fn(LPVOID arg)
{
	return(0);
}


void CMaxExporter::MaxMatrixToEngineMatrix( const Matrix3& mMax, CMatrix& mEngine )
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

void CMaxExporter::EngineMatrixToMaxMatrix( const CMatrix& oEngine, Matrix3& mMax )
{
	mMax.SetRow( 0, Point3( oEngine.m_00, oEngine.m_10, oEngine.m_20 ) );
	mMax.SetRow( 1, Point3( oEngine.m_01, oEngine.m_11, oEngine.m_21 ) );
	mMax.SetRow( 2, Point3( oEngine.m_02, oEngine.m_12, oEngine.m_22 ) );
	mMax.SetRow( 3, Point3( oEngine.m_03, oEngine.m_13, oEngine.m_23 ) );
}


void CMaxExporter::GetAnimation( Interface* pInterface, const map< int, INode* >& mBone, map< int, vector< CKey > >& mBones )
{
	bool bTCB = false;
	
	//IGameScene* pGameScene = GetIGameInterface();
	//bool bInitialise = pGameScene->InitialiseIGame();

	CMatrix oWorldNodeTM, oParentTM, oInverseParentTM;
	//int nExportAnimationTimeStart = m_nExportAnimationStart * g_nTickPerFrame;
	//int nExportAnimationTimeEnd = m_nExportAnimationEnd * g_nTickPerFrame;

	for ( map< int, INode* >::const_iterator itNode = mBone.begin(); itNode != mBone.end(); ++itNode )
	{
		INode* pNode = itNode->second;
		Control* pControl = pNode->GetTMController()->GetRotationController();
		IKeyControl* pKc = GetKeyControlInterface( pControl );
		if ( pKc )
		{
			for ( int iKey = 0; iKey < pKc->GetNumKeys(); iKey++ )
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
				oKey.m_eType = CKey::eRotKey;
				oKey.m_nTimeValue = key.time;
				oKey.m_oWorldTM = oWorldNodeTM;
				oKey.m_oLocalTM = oInverseParentTM * oWorldNodeTM;
				oKey.m_oAngleAxis = key.val;
				oKey.m_oLocalTM.GetQuaternion( oKey.m_qLocal );
				mBones[ itNode->first ].push_back( oKey );
			}
			bTCB = true;
		}
		else
		{
			MSTR sClassName;
			pControl->GetClassName( sClassName );
			char* sName = pNode->GetName();
			m_vNonTCBBoneNames.push_back( sName );
		}
		pControl = pNode->GetTMController()->GetPositionController();
		pKc = GetKeyControlInterface( pControl );
		if( pKc )
		{
			for ( int iKey = 0; iKey < pKc->GetNumKeys(); iKey++ )
			{
				ITCBPoint3Key key;
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
				oKey.m_eType = CKey::ePosKey;
				oKey.m_nTimeValue = key.time;
				oKey.m_oWorldTM = oWorldNodeTM;
				oKey.m_oLocalTM = oInverseParentTM * oWorldNodeTM;
				oKey.m_oLocalTM.GetQuaternion( oKey.m_qLocal );
				map< int, vector< CKey > >::iterator itBone = mBones.find( itNode->first );
				bool bKeyExists = false;
				//if( itBone != mBones.end() )
				//{
				//	if( mBones[ itNode->first ].size() > iKey )
				//	{
				//		CMatrix& oLocal = mBones[ itNode->first ][ iKey ].m_oLocalTM;
				//		oLocal.SetAffinePart( 0.f, 0.f, 0.f );
				//		oLocal *= oKey.m_oLocalTM;
				//		bKeyExists = true;
				//	}
				//}
				//if( !bKeyExists )
					mBones[ itNode->first ].push_back( oKey );
			}
		}

	}
	if ( bTCB == false )
		MessageBoxA( NULL, "Votre animation ne comporte pas de controleurs TCB, elle n'a pas été exportée", "", MB_ICONWARNING );
}