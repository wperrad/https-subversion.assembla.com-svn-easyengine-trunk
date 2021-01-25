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
	vector<float>				m_vAdditionalColor;
	bool						m_bUseAdditionalColor;

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
	IShader*					GetShader() const { return m_pShader; }
	void						SetAdditionalColor(float r, float g, float b, float a);
	ITexture*					GetTexture();
	void						SetTexture(ITexture* pTexture);
};


#endif	//MATERIAL_H