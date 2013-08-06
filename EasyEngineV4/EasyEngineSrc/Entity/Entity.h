#ifndef ENTITY_H
#define ENTITY_H

#include "../Utils2/Node.h"
#include "IEntity.h"

class CRessource;
class IRessourceManager;
class IRenderer;
class IRessource;
class IAnimation;
class IGeometryManager;
class CScene;

typedef std::map< std::string, std::map< int, const CNode* > > AnimationBonesMap;

class CEntity : public IEntity
{
protected:

	typedef void (*TCollisionCallback)( IEntity*);

	IRessource*								m_pRessource;
	IRenderer&								m_oRenderer;
	IRessourceManager&						m_oRessourceManager;
	IEntityManager*							m_pEntityManager;
	IGeometryManager&						m_oGeometryManager;
	ICollisionManager&						m_oCollisionManager;
	CBody									m_oBody;
	IAnimation*								m_pCurrentAnimation;
	std::map< std::string, IAnimation* >	m_mAnimation;
	std::string								m_sCurrentAnimation;
	bool									m_bDrawBoundingBox;
	IBone*									m_pOrgSkeletonRoot;
	IBone*									m_pSkeletonRoot;
	bool									m_bHidden;
	CEntity*								m_pEntityRoot;
	IBox*									m_pBBox;
	float									m_fBoundingSphereRadius;
	CMatrix									m_oFirstAnimationFrameSkeletonMatrixInv;
	bool									m_bUsePositionKeys;
	string									m_sTypeName;
	CMatrix									m_oScaleMatrix;
	IRenderer::TRenderType					m_eRenderType;
	IEntity*								m_pBoundingSphere;
	map< int, IEntity* >					m_mBonesBoundingSphere;
	bool									m_bDrawAnimationBoundingBox;
	TCollisionCallback						m_pfnCollisionCallback;
	string									m_sEntityName;
	CScene*									m_pScene;


	void				SetNewBonesMatrixArray( std::vector< CMatrix >& vMatBones );
	void				GetBonesMatrix( CNode* pInitRoot, CNode* pCurrentRoot, std::vector< CMatrix >& vMatrix );
	void				UpdateGroundCollision();
	IEntity*			GetEntityCollision();
	static void			OnAnimationCallback( IAnimation::TEvent e, void* );

public:
	CEntity( IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IGeometryManager& oGeometryManager, ICollisionManager& oCollisionManager );
	CEntity( const std::string& sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IGeometryManager& IGeometryManager, ICollisionManager& oCollisionManager, bool bDuplicate = false );
	virtual				~CEntity();
	void				Update();
	void				DrawBoundingBox( bool bDraw );
	void				SetShader( IShader* pShader );
	void				CenterToworld();
	int					CreateLightEntity( IRessource* pLight );
	IBox*				GetBBox();
	IRessource*			GetRessource();
	float				GetWeight();
	void				SetWeight( float fWeight );
	void				SetRessource( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, bool bDuplicate = false );
	void				SetMesh( IMesh* pMesh );
	void				AddAnimation( std::string sAnimationFile );
	void				SetCurrentAnimation( std::string sAnimation );
	IAnimation*			GetCurrentAnimation();
	IBone*				GetSkeletonRoot();
	void				GetRessourceFileName( string& sFileName );
	bool				HasAnimation( string sAnimationName );
	void				DetachCurrentAnimation();
	void				Hide( bool bHide );
	void				SetEntityRoot( CEntity* pRoot );
	CEntity*			GetEntityRoot();
	void				RunAction( string sAction, bool bLoop ){}
	void                LocalTranslate(float dx , float dy , float dz);
	void				LocalTranslate( const CVector& vTranslate );
	void				LinkEntityToBone( IEntity* pChild, IBone* pParentBone, TLinkType = ePreserveChildRelativeTM );
	void				SetAnimationSpeed( TAnimation eAnimationType, float fSpeed ){}
	TAnimation			GetCurrentAnimationType() const{return eNone;}
	void				GetTypeName( string& sName );
	void				SetScaleFactor( float x, float y, float z );
	void				SetRenderingType( IRenderer::TRenderType t );
	void				DrawBoundingSphere( bool bDraw );
	void				DrawBoneBoundingSphere( int nID, bool bDraw );
	void				DrawAnimationBoundingBox( bool bDraw );
	float				GetBoundingSphereRadius() const;
	void                Link( CNode* pNode );
	void				Goto( const CVector& oPosition, float fSpeed );
	void				SetEntityName( string sName );
	void				GetEntityName( string& sName );
};

#endif // ENTITY_H