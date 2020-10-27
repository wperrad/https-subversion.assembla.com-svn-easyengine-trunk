#include <vector>
#include "../Utils2/StringUtils.h"
#include "../Utils2/EasyFile.h"
#include "../Utils2/Chunk.h"
#include "lightloader.h"

using namespace std;

CLightLoader::CLightLoader(void)
{
}

CLightLoader::~CLightLoader(void)
{
}

void CLightLoader::Load( string sFileName, ILoader::IRessourceInfos& ri, IFileSystem& oFileSystem )
{
	CLightInfos* pLightInfo = static_cast< CLightInfos* >( &ri );

	CEasyFile file;
	file.Open( sFileName, oFileSystem );
	file.SetPointerNext("*LIGHTOBJECT");
	file.SetPointerNext("*LIGHT_TYPE");
	string sLightType;
	file.GetLine( sLightType );
	if( sLightType.find( "Direction" ) != -1 )
		pLightInfo->m_eLightType = CLightInfos::eDirectionnelle;
	else if( sLightType.find( "Omni" ) != -1 )
		pLightInfo->m_eLightType = CLightInfos::eOmni;
	else if( sLightType.find( "Target" ) != -1 )
		pLightInfo->m_eLightType = CLightInfos::eTarget;
	file.SetPointerNext("*NODE_TM");
	string sPosition;
	file.GetLineNext( "TM_POS", sPosition );
	vector< float > vPosition;
	CStringUtils::ExtractFloatFromString( sPosition, vPosition, 3 );
	vPosition.push_back( 0.f );
	pLightInfo->m_vPosition.m_x = vPosition[ 0 ];
	pLightInfo->m_vPosition.m_y = vPosition[ 1 ];
	pLightInfo->m_vPosition.m_z = vPosition[ 2 ];
	file.SetPointerNext( "*LIGHT_SETTINGS" );
	file.SetPointerNext( "*LIGHT_COLOR" );
	string sLine;
	file.GetLine( sLine );
	vector< float > vFloat;
	CStringUtils::ExtractFloatFromString( sLine, vFloat, 3 );	
	vFloat.push_back( 1.f );
	pLightInfo->m_oColor.m_x = vFloat[ 0 ];
	pLightInfo->m_oColor.m_y = vFloat[ 1 ];
	pLightInfo->m_oColor.m_z = vFloat[ 2 ];
	
	file.SetPointerNext("*LIGHT_INTENS");		
	file.GetLine( sLine);
	vector< float > vIntensity;
	CStringUtils::ExtractFloatFromString( sLine, vIntensity, 1 );
	pLightInfo->m_fIntensity = vIntensity[ 0 ];
	file.Close();
}

void CLightLoader::Export( const string& sFileName, const CChunk& chunk)
{

}
