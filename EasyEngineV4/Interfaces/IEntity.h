#ifndef IENTITY_H
#define IENTITY_H

// stl
#include <string>

// Engine
#include "INode.h"
#include "../Utils2/Node.h"
#include "EEPlugin.h"
#include "ILoader.h"
#include "Math/matrix.h"
#include "IRessource.h"
#include "IPathFinder.h"


class IRessourceManager;
class IRenderer;
class ICameraManager;
class IAnimation;
class IShader;
class IRessource;
class ICollisionManager;
class IGeometryManager;
class ISphere;
class IAEntity;
class IFighterEntity;
class IGUIManager;

using namespace std;

#define orgEpsilonError 0.001f

class CBody
{
	static float	s_fEpsilonError;
	static float	s_fZCollisionError;
public:
	float		m_fWeight;
	CVector		m_oSpeed;
				CBody();
	void		Update();

	//static float		GetEpsilonError(){ return 0.001f; }
	static float		GetEpsilonError(){ return s_fEpsilonError; }
	static void			SetZCollisionError( float e );
	static float		GetZCollisionError();
};

float GetGravity();

class IEntity : public CNode
{

public:
	enum TAnimation
	{
		eNone = 0,
		eWalk,
		eStand,
		eRun,
		eHitLeftFoot,
		eHitReceived,
		eJump,
		eDying,
		eAnimationCount
	};

	enum TLinkType
	{
		ePreserveChildRelativeTM = 0,
		eSetChildToParentTM
	};

	virtual void				Update() = 0;
	virtual void				DrawBoundingBox( bool bDraw ) = 0;
	virtual void				SetShader( IShader* pShader ) = 0;
	virtual IGeometry*			GetBoundingGeometry() = 0;
	virtual IRessource*			GetRessource() = 0;
	virtual void				SetWeight( float fWeight ) = 0;
	virtual float				GetWeight() = 0;
	virtual void				SetRessource( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, bool bDuplicate = false ) = 0;
	virtual void				AddAnimation( std::string sAnimationFile ) = 0;
	virtual void				SetCurrentAnimation( std::string sAnimation ) = 0;
	virtual IAnimation*			GetCurrentAnimation() = 0;
	virtual bool				HasAnimation( string sAnimationName ) = 0;
	virtual void				DetachCurrentAnimation() = 0;
	virtual IBone*				GetSkeletonRoot() = 0;
	virtual void				Hide( bool bHide ) = 0;
	virtual void				RunAction( string sAction, bool bLoop ) = 0;
	virtual void				LinkEntityToBone( IEntity* pChild, IBone* pParentBone, TLinkType = ePreserveChildRelativeTM ) = 0;
	virtual void				SetAnimationSpeed( TAnimation eAnimationType, float fSpeed ) = 0;
	virtual TAnimation			GetCurrentAnimationType() const = 0;
	virtual void				GetTypeName( string& sName ) = 0;
	virtual void				SetScaleFactor( float x, float y, float z ) = 0;
	virtual void				SetRenderingType( IRenderer::TRenderType t ) = 0;
	virtual void				DrawBoundingSphere( bool bDraw ) = 0;
	virtual void				DrawBoneBoundingSphere( int nID, bool bDraw ) = 0;
	virtual void				DrawAnimationBoundingBox( bool bDraw ) = 0;
	virtual float				GetBoundingSphereRadius() const = 0;
	virtual void				Goto( const CVector& oPosition, float fSpeed ) = 0;
	virtual void				GetEntityName(string& sName) = 0;
	virtual void				SetEntityName( string sName ) = 0;
	virtual void				Colorize(float r, float g, float b, float a) = 0;
};

class IEntityManager : public CPlugin
{
public:
	struct Desc : public CPlugin::Desc
	{
		IRessourceManager&		m_oRessourceManager;
		IRenderer&				m_oRenderer;
		IFileSystem&			m_oFileSystem;
		ICollisionManager&		m_oCollisionManager;
		IGeometryManager&		m_oGeometryManager;
		IPathFinder&			m_oPathFinder;
		Desc( IRessourceManager& oRessourceManager, IRenderer& oRenderer, IFileSystem& oFileSystem, ICollisionManager& oCollisionManager, 
			IGeometryManager& oGeometryManager, IPathFinder& oPathFinder ) : 
			CPlugin::Desc( NULL, "" ),
			m_oRessourceManager( oRessourceManager ),
			m_oRenderer(oRenderer),
			m_oFileSystem( oFileSystem ),
			m_oCollisionManager( oCollisionManager ),
			m_oGeometryManager( oGeometryManager ),
			m_oPathFinder(oPathFinder){}
	};
	IEntityManager( const Desc& oDesc ) : CPlugin( NULL, "" ){}
	virtual IEntity*			CreateEntity( std::string sFileName, string sTypeName, IRenderer& oRenderer, bool bDuplicate = false ) = 0;
	virtual IEntity*			CreateEntity( string sFileName = "noname" ) = 0;
	virtual IEntity*			CreateRepere( IRenderer& oRenderer ) = 0;
	virtual IEntity*			CreateMobileEntity( string sFileName, IFileSystem* pFileSystem ) = 0;
	virtual IEntity*			CreateNPC( string sFileName, IFileSystem* pFileSystem ) = 0;
	virtual IEntity*			GetEntity( int nEntityID ) = 0;
	virtual IEntity*			GetEntity( string sEntityName ) = 0;
	virtual int					GetEntityID( IEntity* pEntity ) = 0;
	virtual int					GetEntityCount() = 0;
	virtual IEntity*			CreateLightEntity( CVector Color, IRessource::TLight type, float fIntensity ) = 0;
	virtual void				SetLightIntensity( int nID, float fIntensity ) = 0;
	virtual void				DestroyEntity( IEntity* pEntity ) = 0;
	virtual void				DestroyAll() = 0;
	virtual void				Clear() = 0;
	virtual void				AddEntity( IEntity* pEntity, string sEntityName = "noname", int nID = -1 ) = 0;
	virtual void				SetPerso( IEntity* pPerso ) = 0;
	virtual IEntity*			GetPerso() = 0;
	virtual IEntity*			CreateSphere( float fSize ) = 0;
	virtual IEntity*			CreateBox( IRenderer& oRenderer, const CVector& oDimension ) = 0;
	virtual ISphere&			GetSphere( IEntity* pSphereEntity ) = 0;
	virtual IBox&				GetBox( IEntity* pBoxEntity ) = 0;
	virtual void				AddCollideEntity( IEntity* pEntity ) = 0;
	virtual void				RemoveCollideEntity( IEntity* pEntity ) = 0;
	virtual void				SetZCollisionError( float e ) = 0;
	virtual IAEntity*			GetFirstIAEntity() = 0;
	virtual IAEntity*			GetNextIAEntity() = 0;
	virtual IFighterEntity*		GetFirstFighterEntity() = 0;
	virtual IFighterEntity*		GetNextFighterEntity() = 0;
	virtual IEntity*			GetFirstMobileEntity() = 0;
	virtual IEntity*			GetNextMobileEntity() = 0;
	virtual IEntity*			CreateLineEntity( const CVector& first, const CVector& last ) = 0;
	virtual IEntity*			CreateCylinder( float fRadius, float fHeight ) = 0;
	virtual void				Kill(int entityId) = 0;

	virtual IGUIManager* 		GetGUIManager() = 0;
	virtual void				SetGUIManager(IGUIManager* pGUIManager) = 0;
};

class ISceneManager : public CPlugin
{
protected:
	ISceneManager( const Desc& oDesc ) : CPlugin( oDesc.m_pParent, oDesc.m_sName ){}

public:
	struct Desc : public CPlugin::Desc
	{
		IRessourceManager&	m_oRessourceManager;
		IRenderer&			m_oRenderer;
		ICameraManager&		m_oCameraManager;
		IEntityManager&		m_oEntityManager;
		ILoaderManager&		m_oLoaderManager;
		ICollisionManager&	m_oCollisionManager;

		Desc(	IRessourceManager& oRessourceManager, IRenderer& oRenderer, ICameraManager& oCameraManager, 
				IEntityManager& oEntityManager, ILoaderManager& oLoaderManager, ICollisionManager& oCollisionManager ):
			CPlugin::Desc( NULL, "" ),
			m_oRessourceManager( oRessourceManager ),
			m_oRenderer( oRenderer ),
			m_oCameraManager( oCameraManager ),
			m_oEntityManager ( oEntityManager ),
			m_oLoaderManager( oLoaderManager ),
			m_oCollisionManager( oCollisionManager ){}
	};

	virtual IEntity*	CreateScene( std::string sSceneName, std::string sMeshFileName, IGeometryManager& oGeometryManager, IPathFinder& oPathFinder ) = 0;
	virtual IEntity*	GetScene( std::string sSceneName ) = 0;
	virtual IEntity*	Merge( IEntity* pScene, const std::string& sRessourceName, string sEntityType, float x = 0, float y = 0, float z = 0, bool bIsAnimated = false ) = 0;
	virtual IEntity*	Merge( IEntity* pScene, string sRessourceName, string sEntityType, CMatrix& oXForm ) = 0;
	virtual void		Load( IEntity* pScene, string sFileName ) = 0;
	virtual void		Export( IEntity* pScene, string sFileName ) = 0;
	virtual void		ClearScene( IEntity* pScene ) = 0;
	virtual void		CreateCollisionMap(IEntity* pScene) = 0;
};

#endif // IENTITY_H