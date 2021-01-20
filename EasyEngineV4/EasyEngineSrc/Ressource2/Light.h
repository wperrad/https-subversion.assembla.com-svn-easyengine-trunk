#ifndef LIGHT_CPP
#ifndef RESSOURCEMANAGER_CPP
#error
#endif
#endif

#ifndef LIGHT_H
#define LIGHT_H

#include "IRessource.h"
#include "Math/Vector.h"

#include <windows.h>
#include <gl/gl.h>
#include <gl/GLU.h>


class CLight : public IRessource
{	
	static unsigned int			s_nCurrentLightID;
	static map< int, bool >		s_mEnabledLight;

	CVector						m_Ambient;
	CVector						m_Diffuse;
	CVector						m_Specular;		
	TLight						m_Type;
	float						m_fIntensity;
	unsigned int				m_ID;
	bool						m_bIsEnabled;
	GLUquadricObj*				m_pDebugQuadricObj;	
	

public:
	struct Desc : public IRessource::Desc
	{
		CVector				Color;
		IRessource::TLight	type;
		float 				fIntensity;
		float 				fAttenuationConstant;
		float 				fAttenuationLinear;
		float 				fAttenuationQuadratic;
		Desc( IRenderer& oRenderer, IShader* pShader );
	};



								CLight( const Desc& oDesc );
	virtual 					~CLight();
	void 						Update();
	TLight						GetType();
	bool						IsEnabled();	
	void						Disable();
	void						Enable();
	void						SetShader( IShader* pShader );
	void						SetIntensity( float fIntensity );
	float						GetIntensity();
	CVector						GetColor();
	IShader*					GetShader() const { return NULL; }
};

#endif //LIGHT_H
