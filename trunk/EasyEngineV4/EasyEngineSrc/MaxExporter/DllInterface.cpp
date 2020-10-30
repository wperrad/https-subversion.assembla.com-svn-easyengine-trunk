#include "dllInterface.h"
#include "BinaryAnimationMaxExporter.h"
#include "BinaryMeshMaxExporter.h"

// MAX
#include "plugapi.h"

using namespace std;

vector<ClassDesc*>	CMaxExporter::m_vLibClassDesc;


BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
{
	CBinaryMeshMaxExporterClassDesc* pMeshDesc = new CBinaryMeshMaxExporterClassDesc;
	CBinaryAnimationMaxExporterClassDesc* pAnimationDesc = new CBinaryAnimationMaxExporterClassDesc;
	CMaxExporter::m_vLibClassDesc.push_back(pMeshDesc);
	CMaxExporter::m_vLibClassDesc.push_back(pAnimationDesc);
	

	if( fdwReason == DLL_PROCESS_ATTACH )
	{
		hInstance = hinstDLL;
		DisableThreadLibraryCalls(hInstance);
	}
	return ( TRUE );
}


extern "C" __declspec( dllexport ) const TCHAR* LibDescription() 
{
	return _T("Easy Engine exporter");
}

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS 
extern "C" __declspec( dllexport ) int LibNumberClasses() 
{
	//return 1;
	return (int)CMaxExporter::m_vLibClassDesc.size();
}


extern "C" __declspec( dllexport ) ClassDesc* LibClassDesc( int i )
{
	return CMaxExporter::m_vLibClassDesc[ i ];
}

extern "C" __declspec( dllexport ) ULONG LibVersion() 
{
	return VERSION_3DSMAX;
}

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}