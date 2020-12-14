#ifndef RESSOURCE_MANAGER_H
#define RESSOURCE_MANAGER_H

#define RESSOURCEMANAGER_CPP
#include "RessourceManager.h"

// Engine
#include "../Utils2/EasyFile.h"
#include "../Utils2/Node.h"
#include "../Utils2/Dimension.h"
#include "../Utils2/StringUtils.h"

// Ressources
#include "Mesh.h"
#include "AnimatableMesh.h"
#include "Animation.h"
#include "Material.h"
#include "Texture.h"
#include "Light.h"
#include "CollisionMesh.h"
#include "Exception.h"

// Interfaces
#include "ILoader.h"
#include "ISystems.h"
#include "IGeometry.h"

// stl
#include <algorithm>

using namespace std;

CRessourceManager::CRessourceManager( const Desc& oDesc ) :
IRessourceManager( oDesc ),
m_nLightCount(0),
m_oLoaderManager( oDesc.m_oLoaderManager ),
m_pCurrentRenderer( NULL ),
m_pDrawTool( NULL ),
m_bCatchException( true ),
m_oSystemManager( oDesc.m_oSystemManager )
{
	m_mRessourceCreation[ "ase" ] = CreateMesh;
	m_mRessourceCreation[ "ame" ] = CreateMesh;
	m_mRessourceCreation[ "bme" ] = CreateMesh;
	m_mRessourceCreation[ "bke" ] = CreateAnimation;
	m_mRessourceCreation[ "ale" ] = CreateLight;
	m_mRessourceCreation[ "tga" ] = CreateTexture;
	m_mRessourceCreation[ "bmp" ] = CreateTexture;
	m_mRessourceCreation[ "col" ] = CreateCollisionMesh;
}


//-------------------------------------------------------------------------------------------
//										destructor
//-------------------------------------------------------------------------------------------
CRessourceManager::~CRessourceManager(void)
{
}

bool CRessourceManager::IsCatchingExceptionEnabled()
{
	return m_bCatchException;
}

void CRessourceManager::EnableCatchingException( bool bEnable )
{
	m_bCatchException = bEnable;
}

ITexture* CRessourceManager::CreateTexture2D( IRenderer& oRenderer, IShader* pShader, int nUnitTexture, vector< unsigned char >& vData, int nWidth, int nHeight, IRenderer::TPixelFormat eFormat )
{
	CTexture2D::CDesc oDesc( oRenderer, pShader, nUnitTexture );
	oDesc.m_vTexels.insert( oDesc.m_vTexels.begin(), vData.begin(), vData.end() );
	oDesc.m_nWidth = nWidth;
	oDesc.m_nHeight = nHeight;
	oDesc.m_eFormat = eFormat;
	oDesc.m_nUnitTexture = nUnitTexture;
	CTexture2D* pTexture = new CTexture2D( oDesc );
	return pTexture;
}

//-------------------------------------------------------------------------------------------
//										_GetRessource
//-------------------------------------------------------------------------------------------
IRessource* CRessourceManager::GetRessource( const string& sRessourceFileName, IRenderer& oRenderer, bool bDuplicate ) 
{	
	IRessource* pRessource = NULL;
	if ( m_mRessource.count( sRessourceFileName ) > 0 && !bDuplicate )
	{
		map< string, IRessource* >::iterator itFind = m_mRessource.find( sRessourceFileName );
		pRessource = itFind->second;		
	}
	else
	{
		pRessource = GetRessourceByExtension( sRessourceFileName, oRenderer );
		if ( pRessource )
		{
			m_mRessource[ sRessourceFileName ] = pRessource;
			pRessource->SetFileName( sRessourceFileName );
		}
	}
	return pRessource;
}

ITestMesh* CRessourceManager::GetTestRessource( const std::string& sRessourceFileName, ITestShaderManager& oTestShaderManager, IRenderer& oRenderer )
{
	string sExtension;
	CStringUtils::GetExtension( sRessourceFileName, sExtension );
	map< string, TTestRessourceCreation >::iterator itRessourceCreation = m_mTestRessourceCreation.find( sExtension );
	if( itRessourceCreation == m_mTestRessourceCreation.end() )
	{
		string sMessage = string( "\"" ) + sRessourceFileName + "\" n'existe pas ou n'est pas prise en charge par le gestionnaire de ressources";
		exception e( sMessage.c_str() );
		throw e;
	}
	ITestMesh* pRessource = itRessourceCreation->second( sRessourceFileName, this, oTestShaderManager, oRenderer );
	return pRessource;
}

IRessource*	CRessourceManager::CreateMaterial( ILoader::CMaterialInfos& mi, IRenderer& oRenderer, ITexture* pAlternative )
{
	IShader* pShader = oRenderer.GetShader( mi.m_sShaderName );
	CMaterial::Desc oDesc( oRenderer, pShader );
	oDesc.m_fShininess = mi.m_fShininess;
	if( !pAlternative )
		oDesc.m_pTexture = static_cast< ITexture* >( GetRessource( mi.m_sDiffuseMapName, oRenderer ) );
	else
		oDesc.m_pTexture = pAlternative;
	oDesc.m_sName = mi.m_sFileName;
	std::copy( mi.m_vAmbient.begin(), mi.m_vAmbient.end(), oDesc.m_vAmbient.begin() );
	std::copy( mi.m_vDiffuse.begin(), mi.m_vDiffuse.end(), oDesc.m_vDiffuse.begin() );
	std::copy( mi.m_vEmissive.begin(), mi.m_vEmissive.end(), oDesc.m_vEmissive.begin() );
	std::copy( mi.m_vSpecular.begin(), mi.m_vSpecular.end(), oDesc.m_vSpecular.begin() );
	return new CMaterial( oDesc );
}

IRessource* CRessourceManager::GetRessourceByExtension( string sRessourceFileName, IRenderer& oRenderer )
{
	string sExtension;
	CStringUtils::GetExtension( sRessourceFileName, sExtension );
	map< string, TRessourceCreation >::iterator itRessourceCreation = m_mRessourceCreation.find( sExtension );
	if( itRessourceCreation == m_mRessourceCreation.end() )
	{
		string sMessage = string( "\"" ) + sRessourceFileName + "\" n'existe pas ou n'est pas prise en charge par le gestionnaire de ressources";
		CExtensionNotFoundException e( sMessage.c_str() );
		throw e;
	}
	IRessource* pRessource = itRessourceCreation->second( sRessourceFileName, this, oRenderer );
	return pRessource;
}

int CRessourceManager::GetLightCount()
{
	return m_nLightCount;
}

IRessource* CRessourceManager::CreateMesh( string sFileName, CRessourceManager* pRessourceManager, IRenderer& oRenderer )
{
	ILoader::CAnimatableMeshData oData;
	oData.m_sFileName = sFileName;
	pRessourceManager->m_oLoaderManager.Load( sFileName, oData );
	return pRessourceManager->CreateMesh( oData, oRenderer );
}

IAnimatableMesh* CRessourceManager::CreateMesh( ILoader::CAnimatableMeshData& oData, IRenderer& oRenderer, IRessource* pMaterial )
{
	IRessource::Desc oResDesc( oRenderer, NULL );
	CAnimatableMesh* pAMesh = new CAnimatableMesh( oResDesc );
	IBone* pSkeleton = LoadSkeleton( oData );
	pAMesh->SetSkeleton( pSkeleton );
	for( unsigned int i = 0; i < oData.m_vMeshes.size(); i++ )
	{
		ILoader::CMeshInfos& mi = oData.m_vMeshes[ i ];

		vector< float > vFinalUVVertexArray;
		vector< unsigned int > vFinalUVIndexArray;
		if ( mi.m_vUVIndex.size() > 0 )
		{
			vFinalUVIndexArray.resize( mi.m_vUVIndex.size() );
			memcpy( &vFinalUVIndexArray[ 0 ], &mi.m_vUVIndex[ 0 ], mi.m_vUVIndex.size() * sizeof( unsigned int ) );
			vFinalUVVertexArray.resize( mi.m_vUVVertex.size() );
			memcpy( &vFinalUVVertexArray[ 0 ], &mi.m_vUVVertex[ 0 ], mi.m_vUVVertex.size() * sizeof( float ) );
		}
		IShader* pShader = NULL;	
		string sShaderName;

		CMesh::Desc oDesc( mi.m_vVertex, mi.m_vIndex, vFinalUVVertexArray, vFinalUVIndexArray, mi.m_vNormalVertex, oRenderer, pShader );
		oDesc.m_vNormalFaceArray.swap( mi.m_vNormalFace );
		oDesc.m_vNormalVertexArray.swap( mi.m_vNormalVertex );
		oDesc.m_pBbox = mi.m_pBoundingBox;

		if ( mi.m_vWeightVertex.size() > 0 )
		{
			pShader = oRenderer.GetShader( "skinning" );
			oDesc.m_vVertexWeight.resize( mi.m_vWeightVertex.size() );
			memcpy( &oDesc.m_vVertexWeight[ 0 ], &mi.m_vWeightVertex[ 0 ], mi.m_vWeightVertex.size() * sizeof( float ) );
			oDesc.m_vWeightedVertexID.resize( mi.m_vWeigtedVertexID.size() );
			memcpy( &oDesc.m_vWeightedVertexID[ 0 ], &mi.m_vWeigtedVertexID[ 0 ], mi.m_vWeigtedVertexID.size() * sizeof( int ) );
		}
		oDesc.m_bIndexed = mi.m_bCanBeIndexed;

		if( !pShader )
		{
			pShader = oRenderer.GetShader( mi.m_sShaderName );
			if( !pShader )
				pShader = oRenderer.GetShader( "perpixellighting" );
		}
		
		if ( !pMaterial )
			CollectMaterials( mi.m_oMaterialInfos, oRenderer, pShader, this, oDesc.m_mMaterials );
		else
			oDesc.m_mMaterials[ 0 ] = static_cast< CMaterial* >( pMaterial );
		if( oData.m_bMultiMaterialActivated )
			oDesc.m_vFaceMaterialID.swap( mi.m_vFaceMaterialID );

		for( map< string, int >::iterator itPrefAnim = mi.m_oPreferedKeyBBox.begin(); itPrefAnim != mi.m_oPreferedKeyBBox.end(); itPrefAnim++ )
		{
			map< string, map< int, IBox* > >::const_iterator itAnimation = mi.m_oKeyBoundingBoxes.find( itPrefAnim->first );
			if( itAnimation != mi.m_oKeyBoundingBoxes.end() )
			{
				map< int, IBox* >::const_iterator itBone = itAnimation->second.find( itPrefAnim->second );
				if( itBone != itAnimation->second.end() )
				{
					IBox* pBox = itBone->second;
					string sAnimNameLow = itPrefAnim->first;
					std::transform( itPrefAnim->first.begin(), itPrefAnim->first.end(), sAnimNameLow.begin(), tolower );
					oDesc.m_mAnimationKeyBox[ sAnimNameLow ] = pBox;
				}
			}
		}

		oDesc.m_pShader = pShader;
		oDesc.m_nParentBoneID = mi.m_nParentBoneID;
		oDesc.m_sName = mi.m_sName;
		oDesc.m_sFileName = mi.m_sFileName;
		oDesc.m_oOrgMaxPosition = mi.m_oOrgMaxPosition;
		
		CMesh* pMesh = new CMesh( oDesc );
		pAMesh->AddMesh( pMesh );
	}
	return pAMesh;
}

IRessource* CRessourceManager::CreateCollisionMesh(string sFileName, CRessourceManager* pRessourceManager, IRenderer& oRenderer)
{
	ILoader::CCollisionModelInfos cmi;
	pRessourceManager->m_oLoaderManager.Load(sFileName, cmi);
	CCollisionMesh::Desc oDesc(oRenderer, cmi);
	return new CCollisionMesh(oDesc);
}

IBone* CRessourceManager::LoadSkeleton( ILoader::CAnimatableMeshData& oData )
{
	IBone* pRoot = NULL;
	if ( oData.m_mHierarchyBones.size() > 0 )
	{
		map< int, IBone* > mBone;
		map< int, int >::iterator itHierarchyBone = oData.m_mHierarchyBones.begin();
		for (  itHierarchyBone; itHierarchyBone != oData.m_mHierarchyBones.end(); ++itHierarchyBone )
		{
			int nBoneID = itHierarchyBone->first;
			IBone* pBone = m_oSystemManager.CreateBone();
			pBone->SetID( nBoneID );
			pBone->SetName( oData.m_mBones[ nBoneID ].first );
			CMatrix& m = oData.m_mBones[ itHierarchyBone->first ].second;
			pBone->SetWorldMatrix( m );
			map< int, IBox* >::const_iterator itBone = oData.m_mBonesBoundingBoxes.find( nBoneID );
			if( itBone != oData.m_mBonesBoundingBoxes.end() )
				pBone->SetBoundingBox( itBone->second );
			mBone[ itHierarchyBone->first ] = pBone;
		}
		for (  itHierarchyBone = oData.m_mHierarchyBones.begin(); itHierarchyBone != oData.m_mHierarchyBones.end(); ++itHierarchyBone )
		{
			if ( itHierarchyBone->second == -1 )
				pRoot = mBone[ itHierarchyBone->first ];
			else
			{
				IBone* pBone = mBone[ itHierarchyBone->first ];
				IBone* pParent = mBone[ itHierarchyBone->second ];
				pBone->Link( pParent );
			}
		}
		pRoot->SetLocalTMByWorldTM();
	}
	return pRoot;
}

void CRessourceManager::PopErrorMessage( string& sMessage )
{
	if( m_vErrorMessage.size() > 0 )
	{
		sMessage = m_vErrorMessage.back();
		m_vErrorMessage.pop_back();
	}
}

void CRessourceManager::DestroyAllRessources()
{
	for( map< string, IRessource* >::iterator itRessource = m_mRessource.begin(); itRessource != m_mRessource.end(); itRessource++ )
	{
		delete itRessource->second;
	}
	m_mRessource.clear();
}

void CRessourceManager::CollectMaterials( const ILoader::CMaterialInfos& oMaterialInfos, IRenderer& oRenderer, IShader* pShader, IRessourceManager* pRessourceManager, std::map< int, CMaterial* >& mMaterials )
{
	mMaterials[ oMaterialInfos.m_nID ] = CreateMaterial( &oMaterialInfos, oRenderer, pShader, pRessourceManager );
	for ( unsigned int i = 0; i < oMaterialInfos.m_vSubMaterials.size(); i++ )
		CollectMaterials( oMaterialInfos.m_vSubMaterials[ i ], oRenderer, pShader, pRessourceManager, mMaterials );

}

CMaterial* CRessourceManager::CreateMaterial( const ILoader::CMaterialInfos* pMaterialInfos, IRenderer& oRenderer, IShader* pShader, IRessourceManager* pRessourceManager )
{
	CMaterial::Desc oMatDesc( oRenderer, pShader );
	if ( pMaterialInfos && pMaterialInfos->m_vAmbient.size() > 0 )
	{
		oMatDesc.m_vAmbient.resize( pMaterialInfos->m_vAmbient.size() );
		memcpy( &oMatDesc.m_vAmbient[ 0 ], &pMaterialInfos->m_vAmbient[ 0 ], pMaterialInfos->m_vAmbient.size() * sizeof( float ) );		
		oMatDesc.m_vDiffuse.resize( pMaterialInfos->m_vDiffuse.size() );
		memcpy( &oMatDesc.m_vDiffuse[ 0 ], &pMaterialInfos->m_vDiffuse[ 0 ], pMaterialInfos->m_vDiffuse.size() * sizeof( float ) );
		if( pMaterialInfos->m_vEmissive.size() > 0 )
		{
			oMatDesc.m_vEmissive.resize( pMaterialInfos->m_vEmissive.size() );
			memcpy( &oMatDesc.m_vEmissive[ 0 ], &pMaterialInfos->m_vEmissive[ 0 ], pMaterialInfos->m_vEmissive.size() * sizeof( float ) );
		}
		oMatDesc.m_fShininess = pMaterialInfos->m_fShininess;
		oMatDesc.m_vSpecular.resize( pMaterialInfos->m_vSpecular.size() );
		memcpy( &oMatDesc.m_vSpecular[ 0 ], &pMaterialInfos->m_vSpecular[ 0 ], pMaterialInfos->m_vSpecular.size() * sizeof( float ) );

		if ( pMaterialInfos->m_sDiffuseMapName != "NONE" )
		{
			ITexture* pTexture = static_cast< ITexture* > ( pRessourceManager->GetRessource( pMaterialInfos->m_sDiffuseMapName, oRenderer ) );
			if ( !pTexture )
			{
				string sMessage = string( "Texture " ) + pMaterialInfos->m_sDiffuseMapName + " not found";
				exception e( sMessage.c_str() );
				throw e;
			}
			pTexture->SetShader( pShader );
			oMatDesc.m_pTexture = pTexture;
		}		
	}
	return new CMaterial( oMatDesc );
}

IRessource* CRessourceManager::CreateAnimation( string sFileName, CRessourceManager* pRessourceManager, IRenderer& oRenderer )
{	
	ILoader::CAnimationInfos ai;
	pRessourceManager->m_oLoaderManager.Load( sFileName, ai );
	
	CAnimation::Desc oRessourceDesc ( oRenderer, NULL );
	CAnimation* pAnimation = new CAnimation( oRessourceDesc );
	pAnimation->SetStartAnimationTime( ai.m_nStartTime );
	pAnimation->SetEndAnimationTime( ai.m_nEndTime );
	pAnimation->SetName( ai.m_sName );
	
	unsigned int iKeyIndex = 0;
	for ( int i = 0; i < ai.m_nBoneCount; i++ )
	{
		unsigned int nBoneID = ai.m_vBonesIDArray[ i ];
		pAnimation->AddBone( nBoneID, pRessourceManager->m_oSystemManager );
		int nKeyCount = ai.m_vKeyCountArray[ i ];
		for ( int iKey = 0; iKey < nKeyCount; iKey++ )
		{
			int nTimeValue = ( int )ai.m_vTimeValueArray[ iKeyIndex ];
			int nKeyType = ( int )ai.m_vKeyTypeArray[ iKeyIndex ];
			pAnimation->AddKey( nBoneID, nTimeValue, (CKey::TKey)nKeyType, ai.m_vLocalTMArray[ iKeyIndex ], ai.m_vWorldTMArray[ iKeyIndex ], ai.m_vLocalQuatArray[ iKeyIndex ] );
			iKeyIndex ++;
		}
	}
	return static_cast< IAnimation* > ( pAnimation );
}


IRessource* CRessourceManager::CreateTexture( string sFileName, CRessourceManager* pRessourceManager, IRenderer& oRenderer )
{
	ILoader::CTextureInfos ti;
	pRessourceManager->m_oLoaderManager.LoadTexture( sFileName, ti );
	ti.m_sFileName = sFileName;
	IRenderer::TPixelFormat format = IRenderer::T_FormatNone;
	switch(ti.m_ePixelFormat)
	{
	case ILoader::eRGB:
		format = IRenderer::T_RGB;
		break;
	case ILoader::eRGBA:
		format = IRenderer::T_RGBA;
		break;
	case ILoader::eBGR:
		format = IRenderer::T_BGR;
		break;
	case ILoader::eBGRA:
		format = IRenderer::T_BGRA;
		break;
	default:
		{
			ostringstream oss;
			oss << "Error : \"" << sFileName << "\" : Bad Texture format";
			CRessourceException e(oss.str());
			throw e;
		}
	}	

	CTexture2D::CDesc desc( oRenderer, NULL, 0 );
	desc.m_nWidth = ti.m_nWidth;
	desc.m_nHeight = ti.m_nHeight;
	desc.m_eFormat = format;
	desc.m_vTexels.swap( ti.m_vTexels );
	desc.m_nUnitTexture = 3;
	CTexture2D* pTexture = new CTexture2D( desc );
	return static_cast< ITexture* > ( pTexture );
}

IRessource* CRessourceManager::CreateLight( CVector Color, IRessource::TLight type, float fIntensity, IRenderer& oRenderer )
{
	CLight::Desc oDesc( oRenderer, NULL );
	oDesc.Color = Color;
	oDesc.fIntensity = fIntensity;
	oDesc.type = type;
	return new CLight( oDesc );
}

void CRessourceManager::SetLightIntensity( IRessource* pRessource, float fIntensity )
{
	CLight* pLight = dynamic_cast< CLight* >( pRessource );
	pLight->SetIntensity( fIntensity );
}

float CRessourceManager::GetLightIntensity( IRessource* pRessource )
{
	CLight* pLight = dynamic_cast< CLight* >( pRessource );
	return pLight->GetIntensity();
}

CVector CRessourceManager::GetLightColor( IRessource* pRessource )
{
	CLight* pLight = dynamic_cast< CLight* >( pRessource );
	return pLight->GetColor();
}

IRessource* CRessourceManager::CreateLight( string sFileName, CRessourceManager* pRessourceManager, IRenderer& oRenderer )
{
	ILoader::CLightInfos li;
	li.m_sFileName = sFileName;
	pRessourceManager->m_oLoaderManager.Load( sFileName, li );
	CLight::TLight type;
	switch( li.m_eLightType )
	{
	case ILoader::CLightInfos::eDirectionnelle:
		type = CLight::DIRECTIONAL;
		break;
	case ILoader::CLightInfos::eOmni:
		type = CLight::OMNI;
		break;
	case ILoader::CLightInfos::eTarget:
		type = CLight::SPOT;
		break;
	}
	CLight::Desc desc( oRenderer, NULL );
	desc.type = type;
	desc.Color = CVector( li.m_oColor.m_x, li.m_oColor.m_y, li.m_oColor.m_z, li.m_oColor.m_w );
	desc.fIntensity = li.m_fIntensity;
	CLight* pLight = new CLight( desc );
	pRessourceManager->m_nLightCount++;
	return pLight;
}

IRessource::TLight	CRessourceManager::GetLightType( IRessource* pRessource )
{
	CLight* pLight = dynamic_cast< CLight* >( pRessource );
	return pLight->GetType();
}

void CRessourceManager::DisableLight( IRessource* pRessource )
{
	CLight* pLight = dynamic_cast< CLight* >( pRessource );
	pLight->Disable();
}

void CRessourceManager::SetCurrentRenderer( IRenderer* pRenderer )
{
	m_pCurrentRenderer = pRenderer;
}

extern "C" _declspec(dllexport) IRessourceManager* CreateRessourceManager( IRessourceManager::Desc& oDesc )
{
	return new CRessourceManager( oDesc );
}


#endif	//RESSOURCE_MANAGER_H