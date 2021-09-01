#define LIGHT_CPP

#include "light.h"
#include "IRenderer.h"

unsigned int CLight::s_nCurrentLightID = GL_LIGHT0;
map< int, bool > CLight::s_mEnabledLight;

CLight::Desc::Desc( IRenderer& oRenderer, IShader* pShader ):
IRessource::Desc( oRenderer, pShader ),
fIntensity( 0 ),
fAttenuationConstant( 1 ),
fAttenuationLinear( 0 ),
fAttenuationQuadratic( 0 ),
type( CLight::OMNI ),
Color( 1,1,1,1 )
{
}

CLight::CLight( const Desc& oDesc ):
IRessource( oDesc ),
m_bIsEnabled( true ),
m_Specular( oDesc.Color ),
m_Diffuse( oDesc.Color ),
m_Type( oDesc.type ),
m_ID( s_nCurrentLightID ),
m_Ambient(0.f, 0.f, 0.f, 1.f)
{
	s_nCurrentLightID ++;	
	oDesc.m_oRenderer.SetLightAttenuation( m_ID, oDesc.fAttenuationConstant, oDesc.fAttenuationLinear, oDesc.fAttenuationQuadratic );
	SetIntensity( oDesc.fIntensity );

	if (m_Type != CLight::DIRECTIONAL)
		oDesc.m_oRenderer.SetLightAttenuation( m_ID, 0.01f, 0.0001f, 0.000001f );

	if (m_Type == CLight::SPOT)
		oDesc.m_oRenderer.SetLightSpotProp( m_ID, 60.f, 1000 );
	oDesc.m_oRenderer.EnableLight( m_ID );
	s_mEnabledLight[ m_ID ] = true;
	ostringstream ossName;
	ossName << "Light" << m_ID;
	m_sName = ossName.str();
}

CLight::~CLight(void)
{
}

void CLight::Update()
{	
	if ( m_bIsEnabled )
	{
		if (m_Type == CLight::DIRECTIONAL)
		{						
			GetRenderer().SetLightLocalPos(m_ID, 0.f, -1.f, 0.f, 0.f);
		}
		if (m_Type == CLight::OMNI)
		{			
			GetRenderer().SetLightLocalPos(m_ID, 0.f,0.f,0.f,1.f);		
		}
		if (m_Type == CLight::SPOT)
		{
			GetRenderer().SetLightLocalPos( m_ID, 0.f,0.f,0.f,1.f );
			GetRenderer().SetLightSpotDirection( m_ID, 0.f,0.f,1.f, 0.f );
		}
	}
}

void CLight::Disable()
{
	m_bIsEnabled = false;	
	GetRenderer().DisableLight(m_ID);
	SetIntensity( 0.f );
}

void CLight::Enable()
{
	m_bIsEnabled = true;	
	GetRenderer().EnableLight(m_ID);
}

CLight::TLight CLight::GetType()
{
	return m_Type;
}

void CLight::SetIntensity(float fIntensity)
{
	m_fIntensity = fIntensity;
	GLfloat diffuse[] = {m_fIntensity*m_Diffuse.m_x,m_fIntensity*m_Diffuse.m_y,m_fIntensity*m_Diffuse.m_z,m_Diffuse.m_w};
	GLfloat specular[] = {m_fIntensity*m_Specular.m_x,m_fIntensity*m_Specular.m_y,m_fIntensity*m_Specular.m_z,m_Specular.m_w};
	GLfloat ambient[] = {m_fIntensity*m_Ambient.m_x,m_fIntensity*m_Ambient.m_y,m_fIntensity*m_Ambient.m_z,1.f};			

	GetRenderer().SetLightAmbient( m_ID, ambient[0], ambient[1], ambient[2], ambient[3] );
	GetRenderer().SetLightDiffuse(m_ID, diffuse[0], diffuse[1], diffuse[2], diffuse[3] );
	GetRenderer().SetLightSpecular( m_ID, specular[0], specular[1], specular[2], specular[3] );
}

float CLight::GetIntensity()
{
	return m_fIntensity;
}

bool CLight::IsEnabled()
{
	return m_bIsEnabled;
}

void CLight::SetShader( IShader* pShader )
{
}

CVector CLight::GetColor()
{
	return m_Diffuse;
}