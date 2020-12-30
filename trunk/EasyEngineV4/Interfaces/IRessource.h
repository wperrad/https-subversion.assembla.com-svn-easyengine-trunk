#ifndef IRESSOURCE_H
#define IRESSOURCE_H

#include <string>
#include <vector>
#include <string>
#include "EEPlugin.h"
#include "../Utils2/Chunk.h"
#include "ILoader.h"
#include "IRenderer.h"

class IRenderer;
class IShaderManager;
class CMatrix;
class IDrawTool;
class CNode;
class IFileSystem;
class ILoaderManager;
class CQuaternion;
class CVector;
class IShader;
class ISystemsManager;
class IBone;
class IBox;
struct CKey;

using namespace std;

class IRessource
{	
	IRenderer&						m_oRenderer;

protected:
	string							m_sFileName;
	string							m_sName;
	IRenderer&						GetRenderer(){ return m_oRenderer; }

public:

	enum TLight
	{
		DIRECTIONAL,
		OMNI,
		SPOT
	};

	
	struct Desc
	{
		IRenderer&			m_oRenderer;
		IShader*			m_pShader;
		string				m_sName;
		string				m_sFileName;
		Desc( IRenderer& oRenderer, IShader* pShader ) : m_oRenderer( oRenderer ), m_pShader( pShader ){}
	};

	IRessource( const Desc& oDesc ) : m_oRenderer( oDesc.m_oRenderer ), m_sName( oDesc.m_sName ), m_sFileName( oDesc.m_sFileName ){}
	virtual				~IRessource() = 0{}

	virtual void 		Update() = 0;
	virtual void		SetShader( IShader* pShader ) = 0;
	virtual void		GetFileName( string& sFileName )
	{ 
		sFileName = m_sFileName; 
	}
	virtual void		SetFileName( string sFileName ){ m_sFileName = sFileName; }
	virtual IShader*	GetCurrentShader() const = 0;
	virtual void		GetName( string& sName ){ sName = m_sName; }
	virtual void		SetName( string sName ){ m_sName = sName; }
};

class ITexture : public IRessource
{
public:
	ITexture( const IRessource::Desc& oDesc ) : IRessource( oDesc ){}
	virtual void		GetDimension( int& nWidth, int& nHeight ) = 0;
};



class IAnimation : public IRessource
{
public:

	enum TEvent
	{
		ePlay = 0,
		eBeforeUpdate,
		eAfterUpdate,
		eBeginRewind

	};
	typedef void(*TCallback)(TEvent, void*);

	IAnimation( const IRessource::Desc& oDesc ) : IRessource( oDesc ){}
	virtual void		Play( bool bLoop ) = 0;
	virtual void		Pause( bool bPause ) = 0;
	virtual void		Stop() = 0;
	virtual void		SetSpeed( float fSpeed ) = 0;
	virtual bool		GetPause() = 0;
	virtual void		SetSkeleton( IBone* pBone ) = 0;
	virtual void		NextFrame() = 0;
	virtual void		SetAnimationTime( int nTime ) = 0;
	virtual int			GetAnimationTime() = 0;
	virtual void		AddCallback( TCallback pCallback, void* pData ) = 0;
	virtual void		RemoveCallback( TCallback pCallback ) = 0;
	virtual void		RemoveAllCallback() = 0;
	virtual int			GetStartAnimationTime() = 0;
	virtual int			GetEndAnimationTime() = 0;
	virtual void		GetBoneKeysMap( map< int, vector< CKey > >& mBoneKeys ) = 0;
};

class IMesh : public IRessource
{
public:
	IMesh( const IRessource::Desc& oDesc ) : IRessource( oDesc ){}
	virtual bool			operator==( const IMesh& w ) = 0;
	virtual					~IMesh() = 0{}
	virtual void			DrawBoundingBox( bool bDraw ) = 0;
	virtual IBox*			GetBBox() = 0;
	virtual int				GetParentBoneID()const = 0;
	virtual void			GetOrgWorldPosition( CVector& v ) = 0;
	virtual void			SetRenderingType( IRenderer::TRenderType t ) = 0;
	virtual IBox*			GetAnimationBBox( string sAnimation ) = 0;
	virtual void			DrawAnimationBoundingBox( bool bDraw ) = 0;
	virtual void			SetCurrentAnimationBoundingBox( string AnimationName ) = 0;
	virtual CVector&		GetOrgMaxPosition() = 0;
	virtual void			Colorize(float r, float g, float b, float a) = 0;
};

class IAnimatableMesh : public IRessource
{
public:
	IAnimatableMesh( const IRessource::Desc& oDesc ) : IRessource( oDesc ){}
	virtual ~IAnimatableMesh() = 0{}
	virtual IMesh*			GetMesh( int nIndex ) = 0;
	virtual IBone*			GetSkeleton() = 0;
	virtual unsigned int	GetMeshCount() = 0;
};


class ICollisionMesh : public IRessource
{
public:
	ICollisionMesh(const IRessource::Desc& oDesc) : IRessource(oDesc) {}
	virtual bool		IsCollide(IBox* pBox) = 0;
	virtual IGeometry*	GetGeometry(int index) = 0;
	virtual int			GetGeometryCount() const = 0;
};


class IRessourceManager : public CPlugin
{
protected:
	IRessourceManager( const Desc& oDesc ) : CPlugin( oDesc.m_pParent, oDesc.m_sName ){}

public:
	struct Desc : public CPlugin::Desc
	{
		IFileSystem&		m_oFileSystem;
		ILoaderManager&		m_oLoaderManager;
		ISystemsManager&	m_oSystemManager;
		Desc( IRenderer& oRenderer, IFileSystem& oFileSystem, ILoaderManager& oLoaderManager, ISystemsManager&	oSystemManager ):
			CPlugin::Desc( NULL, "" ),
			m_oFileSystem( oFileSystem ),
			m_oLoaderManager( oLoaderManager ),
			m_oSystemManager( oSystemManager) {}
	};

	virtual IRessource*			GetRessource( const std::string& sRessourceFileName, IRenderer& oRenderer, bool bDuplicate = false ) = 0;
	virtual IRessource*			CreateMaterial( ILoader::CMaterialInfos& mi, IRenderer& oRenderer, ITexture* pAlternative = NULL ) = 0;
	virtual IAnimatableMesh*	CreateMesh( ILoader::CAnimatableMeshData& mi, IRenderer& oRenderer, IRessource* pMaterial ) = 0;
	virtual int					GetLightCount() = 0;
	virtual void				SetDrawTool( IDrawTool* pDrawTool ) = 0;
	virtual ITexture*			CreateTexture2D( IRenderer& oRenderer, IShader* pShader, int nUnitTexture, vector< unsigned char >& vData, int nWidth, int nHeight, IRenderer::TPixelFormat eFormat ) = 0;
	virtual IRessource*			CreateLight( CVector Color, IRessource::TLight type, float fIntensity, IRenderer& oRenderer ) = 0;
	virtual void				SetLightIntensity( IRessource* pLight, float fIntensity ) = 0;
	virtual float				GetLightIntensity( IRessource* pRessource ) = 0;
	virtual CVector				GetLightColor( IRessource* pRessource ) = 0;
	virtual IRessource::TLight	GetLightType( IRessource* pRessource ) = 0;
	virtual void				DisableLight( IRessource* pRessource ) = 0;
	virtual void				EnableCatchingException( bool bEnable ) = 0;
	virtual bool				IsCatchingExceptionEnabled() = 0;
	virtual void				PopErrorMessage( string& sMessage ) = 0;
	virtual void				DestroyAllRessources() = 0;
};

#endif // IRESSOURCE_H