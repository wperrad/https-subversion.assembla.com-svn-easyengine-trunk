#include "Renderer.h"

#define attribute
#define uniform

class CSoftRenderer : public CRenderer
{
	typedef CVector vec2;
	typedef CVector vec3;
	typedef CVector vec4;
	typedef CMatrix mat4;
	enum TProgramID
	{
		T_PER_PIXEL_LIGHTING_PROGRAM = 1,
		T_SKINNING_PROGRAM
	};

	struct CVertexShaderInput
	{
		CVector		m_vVertex;
		CVector		m_vNormal;
		CVector		m_oColor;
	};

	struct CVertexShaderOutput
	{
		CVector		m_oPosition;
		CVector		m_oColor;
	};

	struct CFrontMaterial
	{
		CVector		ambient;
		CVector		diffuse;
		CVector		specular;
		CVector		emissive;
		float		shininess;
		CFrontMaterial():shininess(0){}
	};

	typedef void (*Program)( CSoftRenderer&, const CVertexShaderInput&, CVertexShaderOutput&, int );

	bool		m_bUseTransposeMatrix;
	void		GetAttributeVertex( int nProgram, std::string sVarName, int nIndex, int nVertexSize, CVector& oVertex );
	CMatrix*	m_pMatBones;

	float		m_fRot;
	float		m_fRotSpeed;
	std::map< std::string, int >		m_mFrameNumberByModel;	

protected:

	std::vector< std::vector< float > >							m_vVertex;
	std::vector< std::vector< int > >							m_vIndex;
	CMatrix														m_oModelViewMatrix;
	std::map< int, Program >									m_mProgramID;
	int															m_nCurrentProgram;
	int															m_nBindedVertexBuffer;

	std::map< int, std::map< std::string, int > >				m_mAttributeID;
	std::map< int, std::map< int, bool > >						m_mEnabledAttribute;
	std::map< int, std::map< int, int > >						m_mAttributeToBufferPointer;

	std::map< int, std::map< std::string, int > >				m_mUniformID;
	std::map< int, std::map< int, std::vector< CMatrix > > >	m_mUniformMatrix4v;

	CFrontMaterial												m_oFrontMaterial;


	

	bool			IsAttributeEnabled( unsigned int nAttributeID ) const;
	int				CreateIndexBuffer( const std::vector< unsigned int >& vData );

	static void		VertexShaderPerPixelLighting( CSoftRenderer&, const CVertexShaderInput&, CVertexShaderOutput&, int);
	static void		VertexShaderSkinning( CSoftRenderer&, const CVertexShaderInput& oInput, CVertexShaderOutput& oOutput, int );
	static void		DisplayDebugInfos( CSoftRenderer& oRenderer, const CMatrix& matWeight, const CVector& gl_Vertex );

public:
	CSoftRenderer( const Desc& oDesc );
	IBuffer*	CreateGeometry(	const std::vector< float >&	vVertexArray, const std::vector< unsigned int >& vIndexArray, 
								const std::vector< float >& vUVVertexArray, const std::vector< unsigned int >& vUVIndexArray, 
								const std::vector< float >& vNormalFaceArray, const std::vector< float >& vNormalVertexArray );
	IBuffer*	CreateIndexedGeometry( const std:: vector< float >&	vVertexArray, const std::vector< unsigned int >& vIndexArray, 
										const std::vector< float >& vUVVertexArray, const std::vector< float >& vOldNormalVertexArray );


	void		DrawGeometry( const IBuffer* pBuffer );
	void		DrawIndexedGeometry( const IBuffer* pBuffer, TDrawStyle style = T_TRIANGLES );
	void		DrawBase( const CMatrix& mBase, float fSize );
	void		SetModelViewMatrix( const CMatrix& m );
	void		SetProjectionMatrix( const CMatrix& oMatrix );


	int				CreateBuffer( int nElementCount );
	void			FillBuffer( const std::vector< float >& vData, int nBufferID, int nOffset = 0 );
	void			DrawArray( int nBufferID );
	unsigned int	GetAttributeID( unsigned int nProgramID, const std::string& sVariableName ) const;
	void			EnableVertexAttribArray( unsigned int nAttributeID );
	void			DisableVertexAttribArray( unsigned int nAttributeID );
	void			BindVertexBuffer( int nBufferID );
	void			VertexAttribPointer( unsigned int nAttributeID, int nSize, TNumberType Type, int nPos );
	unsigned int	GetUniformID( unsigned int nProgramID, const std::string& sVariableName ) const;
	void			SendUniformMatrix4fv( unsigned int nUniformID, int nCount, bool bTranspose, std::vector< float >& vValues );
	void 			SetMaterialAmbient( float* fAmbient );
	void 			SetMaterialDiffuse( float* fDiffuse );
	void 			SetMaterialSpecular( float* fSpecular );
	void 			SetMaterialEmissive( float* fEmissive );
	void 			SetMaterialShininess( float fShininess );
};

extern "C" _declspec(dllexport) IRenderer* CreateSoftRenderer( IRenderer::Desc& oDesc );