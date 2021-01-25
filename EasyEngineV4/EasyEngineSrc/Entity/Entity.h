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
class CEntityManager;
class IGUIManager;

typedef std::map< std::string, std::map< int, const CNode* > > AnimationBonesMap;

class CEntity : public IEntity
{
protected:

	typedef void (*TCollisionCallback)( CEntity*, vector<CEntity*>);

	IRessource*								m_pRessource;
	IRenderer&								m_oRenderer;
	IRessourceManager&						m_oRessourceManager;
	CEntityManager*							m_pEntityManager;
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
	bool									m_bUseAdditionalColor;
	CVector									m_oAdditionalColor;
	ICollisionMesh*							m_pCollisionMesh;
	IGeometry*								m_pBoundingGeometry;
	float									m_fMaxStepHeight;


	void				SetNewBonesMatrixArray( std::vector< CMatrix >& vMatBones );
	void				GetBonesMatrix( CNode* pInitRoot, CNode* pCurrentRoot, std::vector< CMatrix >& vMatrix );
	virtual void		UpdateCollision();
	void				GetEntitiesCollision(vector<CEntity*>& entities);
	void				CreateAndLinkCollisionChildren(string sFileName);
	float				GetBoundingSphereDistance(CEntity* pEntity);
	void				UpdateBoundingBox();
	bool				ManageGroundCollision(const CMatrix& olastLocalTM);
	bool				TestEntityCollision(CEntity* pEntity);
	bool				ManageBoxCollision(vector<CEntity*>& vCollideEntities, float dx, float dy, float dz, const CMatrix& oBackupMatrix);
	void				SendBonesToShader();
	static void			OnAnimationCallback( IAnimation::TEvent e, void* );

public:
	CEntity( IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IGeometryManager& oGeometryManager, ICollisionManager& oCollisionManager );
	CEntity( const std::string& sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IGeometryManager& IGeometryManager, ICollisionManager& oCollisionManager, bool bDuplicate = false );
	virtual				~CEntity();
	void				Update();
	void				DrawBoundingBox( bool bDraw );
	void				SetShader( IShader* pShader );
	void				CenterToworld();
	IRessource*			GetRessource();
	float				GetWeight();
	void				SetWeight( float fWeight );
	void				SetRessource( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, bool bDuplicate = false );
	void				SetMesh( IMesh* pMesh );
	void				AddAnimation( std::string sAnimationFile );
	void				SetCurrentAnimation( std::string sAnimation );
	IAnimation*			GetCurrentAnimation();
	IBone*				GetSkeletonRoot();
	bool				HasAnimation( string sAnimationName );
	void				DetachCurrentAnimation();
	void				Hide( bool bHide );
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
	void				Colorize(float r, float g, float b, float a);
	ICollisionMesh*		GetCollisionMesh();
	void				ForceAssignBoundingGeometry(IGeometry* pBoundingGeometry);
	IGeometry*			GetBoundingGeometry();
	float				GetHeight();
	void				LinkAndUpdateMatrices(CEntity* pEntity);
	virtual float		GetGroundHeight(float x, float z);
	virtual void		UpdateRessource();
};

#endif // ENTITY_H