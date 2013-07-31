#ifndef MESH_CPP
#ifndef MATERIAL_CPP
#ifndef RESSOURCEMANAGER_CPP
#error
#endif
#endif
#endif

#ifndef MATERIAL_H
#define MATERIAL_H

#include "IRessource.h"
#include "IRenderer.h"

class ITexture;
class IRenderer;

class CMaterial : public IRessource
{
	std::vector< float >		m_vAmbient;
	std::vector< float >		m_vDiffuse;	
	std::vector< float >		m_vSpecular;
	std::vector< float >		m_vEmissive;
	float						m_fShininess;
	ITexture*					m_pTexture;
	IShader*					m_pShader;

public:
	enum TMaterialType
	{
		AMBIENT = 0,
		DIFFUSE,
		SPECULAR,
		EMISSIVE
	};


	struct Desc : public IRessource::Desc
	{
		std::vector< float >	m_vAmbient;
		std::vector< float >	m_vDiffuse;	
		std::vector< float >	m_vSpecular;
		std::vector< float >	m_vEmissive;
		float					m_fShininess;
		ITexture*				m_pTexture;
		Desc( IRenderer& oRenderer, IShader* pShader );
	};
								CMaterial( const Desc& oDesc );
	virtual						~CMaterial();
	void						Update();
	void						GetMaterialMatrix( CMatrix& );
	void						SetShader( IShader* pShader );
	IShader*					GetCurrentShader() const { return m_pShader; }
};


#endif	//MATERIAL_H