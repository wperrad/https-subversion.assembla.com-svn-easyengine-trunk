#include "ScriptRegistration.h"

#include "IEntity.h"
#include "IInputManager.h"
#include "IConsole.h"
#include "IWindow.h"
#include "ILoader.h"
#include "IEntity.h"
#include "IRenderer.h"
#include "ICameraManager.h"
#include "ICamera.h"
#include "IRessource.h"
#include "Exception.h"
#include "IGUIManager.h"
#include "ICollisionManager.h"
#include "IShader.h"
#include "ISystems.h"
#include "IGeometry.h"
#include "../Utils2/RenderUtils.h"
#include "../Utils2/DebugTool.h"


// stl
#include <sstream>
#include <algorithm>

extern IEntity*				m_pScene;
extern IScriptManager*		m_pScriptManager;
extern IConsole*			m_pConsole;
extern IWindow*				m_pWindow;
extern ILoaderManager*		m_pLoaderManager;
extern IEntityManager*		m_pEntityManager;
extern IRenderer*			m_pRenderer;
extern ICameraManager*		m_pCameraManager;
extern ISceneManager*		m_pSceneManager;
extern IGUIManager*			m_pGUIManager;
extern ICollisionManager*	m_pCollisionManager;
extern IRessourceManager*	m_pRessourceManager;
extern IFileSystem*			m_pFileSystem;
extern CDebugTool*			m_pDebugTool;
extern IGeometryManager*	m_pGeometryManager;
extern IEntity*				m_pRepere;
extern bool					m_bRenderScene;
extern IEventDispatcher*	m_pEventDispatcher;


enum TObjectType
{
	eNone = 0,
	eEntity,
	eBone
};

TObjectType	m_eSelectionType = eNone;
CNode*	m_pSelectedNode = NULL;

struct CNodeInfos
{
	string	m_sName;
	int		m_nID;
};

IEntity* LoadEntity( string sName )
{
	bool bak = m_pRessourceManager->IsCatchingExceptionEnabled();
	m_pRessourceManager->EnableCatchingException( false );
	IEntity* pEntity = NULL;
	try
	{
		pEntity = m_pEntityManager->CreateEntity( sName, "", *m_pRenderer );
		ostringstream oss;
		oss << "L'entit� \"" << sName << "\"a �t� charg�e avec l'identifiant " << m_pEntityManager->GetEntityID( pEntity ) << ".";
		m_pConsole->Println( oss.str() );
	}
	catch( CFileNotFoundException& e )
	{		
		string sMessage = string( "Erreur : fichier \"" ) + e.m_sFileName + "\" manquant.";
		m_pConsole->Println( sMessage );
	}
	catch( CRessourceException& e )
	{
		string s;
		e.GetErrorMessage( s );
		m_pConsole->Println( s );
	}
	catch( CBadFileFormat )
	{
		m_pConsole->Println( "Mauvais format de fichier, essayez de le r�exporter" );
	}
	catch( CEException )
	{
		string sMessage = string( "\"" ) + sName + "\" introuvable";
		m_pConsole->Println( sMessage );
	}
	m_pRessourceManager->EnableCatchingException( bak );
	return pEntity;
}

void DisplayFov( IScriptState* pState )
{
	m_pConsole->Println( "Fonction pas encore impl�ment�e" );
}

void SetFov( IScriptState* pState )
{
	CScriptFuncArgFloat* pFov = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 0 ) );
	m_pRenderer->SetFov( pFov->m_fValue );
}

void print(IScriptState* pState)
{
	CScriptFuncArgString* pVarName = (CScriptFuncArgString*)pState->GetArg(0);
	int val = m_pScriptManager->GetVariableValue(pVarName->m_sValue);
	char msg[16];
	sprintf(msg, "%d", val);
	m_pConsole->Println(msg);
}

void SetEntityName( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgString* pEntityName = static_cast< CScriptFuncArgString* >( pState->GetArg( 1 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	if( pEntity )
		pEntity->SetEntityName( pEntityName->m_sValue );
	else
	{
		ostringstream oss;
		oss << "Erreur : Entit� " << pEntityID->m_nValue << " inconnue";
		m_pConsole->Println( oss.str() );
	}
}

void Goto( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgFloat* px = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
	CScriptFuncArgFloat* py = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 2 ) );
	CScriptFuncArgFloat* pz = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 3 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	try
	{
		if( pEntity )
			pEntity->Goto( CVector(px->m_fValue, py->m_fValue, pz->m_fValue), 10.f );
		else
		{
			ostringstream oss;
			oss << "Erreur : Entit� " << pEntityID->m_nValue << " introuvable";
			m_pConsole->Println( oss.str() );
		}
	}
	catch( CEException& e )
	{
		string sMessage;
		e.GetErrorMessage( sMessage );
		m_pConsole->Println( sMessage );
	}
}

void DisplayAnimationBBox( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgInt* pBool = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	if( pEntity )
	{
		bool bDraw = pBool->m_nValue == 1 ? true : false;
		pEntity->DrawAnimationBoundingBox( bDraw );
	}
}

void CreateBox( IScriptState* pState )
{
	CScriptFuncArgFloat* px = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 0 ) );
	CScriptFuncArgFloat* py = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
	CScriptFuncArgFloat* pz = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 2 ) );
	IEntity* pBox = m_pEntityManager->CreateBox( *m_pRenderer, CVector( px->m_fValue, py->m_fValue, pz->m_fValue ) );
	pBox->Link( m_pScene );
	ostringstream oss;
	oss << "La boite a �t� cr��e avec l'identifiant " << m_pEntityManager->GetEntityID( pBox ) << ".";
	m_pConsole->Println( oss.str() );
}

void CreateSphere( IScriptState* pState )
{
	CScriptFuncArgFloat* pRadius = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 0 ) );
	//ISphere* pSphere = m_pGeometryManager->CreateSphere( CVector(), pRadius->m_fValue );
	IEntity* pSphereEntity = m_pEntityManager->CreateSphere( pRadius->m_fValue );
	pSphereEntity->Link( m_pScene );
	ostringstream oss;
	oss << "La sphere a �t� cr��e avec l'identifiant " << m_pEntityManager->GetEntityID( pSphereEntity ) << ".";
	m_pConsole->Println( oss.str() );
}

void CreateRepere( IScriptState* pState )
{
	IEntity* pRepere = m_pEntityManager->CreateRepere( *m_pRenderer );
	ostringstream oss;
	oss << "Le rep�re a �t� cr�� avec l'identifiant " << m_pEntityManager->GetEntityID( pRepere )  << ".";
	m_pConsole->Println( oss.str() );
}

void Test( IScriptState* pState )
{

	IEntity* pHuman = m_pEntityManager->GetEntity( 34 );
	pHuman->Goto( CVector( 502,411, 5000 ) , -1.f );
}

void ChangeBase( IScriptState* pState )
{
	CScriptFuncArgInt* pEntity1ID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgInt* pEntity2ID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
	IEntity* pEntity1 = static_cast< IEntity* >( m_pEntityManager->GetEntity( pEntity1ID->m_nValue ) );
	IEntity* pEntity2 = static_cast< IEntity* >( m_pEntityManager->GetEntity( pEntity2ID->m_nValue ) );
	CMatrix oWorld1, oWorld1Inv, oWorld2, oNewWorld2, id;
	pEntity1->GetWorldMatrix( oWorld1 );
	oWorld1.GetInverse( oWorld1Inv );
	pEntity2->GetWorldMatrix( oWorld2 );
	oNewWorld2 = oWorld1Inv * oWorld2;
	pEntity1->SetLocalMatrix( id );
	pEntity2->SetLocalMatrix( oNewWorld2 );
}

void SetPreferedKeyBBox( IScriptState* pState )
{
	CScriptFuncArgString* pFileName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	CScriptFuncArgString* pObjectName = static_cast< CScriptFuncArgString* >( pState->GetArg( 1 ) );
	CScriptFuncArgString* pAnimationName = static_cast< CScriptFuncArgString* >( pState->GetArg( 2 ) );
	CScriptFuncArgInt* pKey = static_cast< CScriptFuncArgInt* >( pState->GetArg( 3 ) );
	string sFileName = pFileName->m_sValue;
	if( sFileName.find( ".bme" ) == -1 )
		sFileName += ".bme";
	string sObjectName = pObjectName->m_sValue;
	std::transform( pObjectName->m_sValue.begin(), pObjectName->m_sValue.end(), sObjectName.begin(), tolower );
	string sAnimationName = pAnimationName->m_sValue;
	if( sAnimationName.find( ".bke" ) == -1 )
		sAnimationName += ".bke";
	
	ILoader::CAnimatableMeshData ami;
	m_pLoaderManager->Load( sFileName, ami );
	unsigned int i = 0;
	bool bFind = false;
	for( i = 0; i < ami.m_vMeshes.size(); i++ )
	{
		string sObjectNameLow = ami.m_vMeshes[ i ].m_sName;
		std::transform( ami.m_vMeshes[ i ].m_sName.begin(), ami.m_vMeshes[ i ].m_sName.end(), sObjectNameLow.begin(), tolower );
		if( sObjectNameLow == sObjectName )
		{
			bFind = true;
			break;
		}
	}
	if( bFind )
	{
		ILoader::CMeshInfos& mi = ami.m_vMeshes[ i ];
		string sAnimationNameWithoutExt = sAnimationName.substr( 0, sAnimationName.size() - 4 );
		string sAnimationNameWithoutExtLow = sAnimationNameWithoutExt;
		std::transform( sAnimationNameWithoutExt.begin(), sAnimationNameWithoutExt.end(), sAnimationNameWithoutExtLow.begin(), tolower );
		mi.m_oPreferedKeyBBox[ sAnimationNameWithoutExtLow ] = pKey->m_nValue;
		m_pLoaderManager->Export( sFileName, ami );
	}
	else
		m_pConsole->Println( "L'objet entr� en argument n'existe pas dans le fichier indiqu�" );
}

void ComputeKeysBoundingBoxes( IScriptState* pState )
{
	CScriptFuncArgString* pFileName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	CScriptFuncArgString* pObjectName = static_cast< CScriptFuncArgString* >( pState->GetArg( 1 ) );
	CScriptFuncArgString* pAnimationName = static_cast< CScriptFuncArgString* >( pState->GetArg( 2 ) );
	string sFileName = pFileName->m_sValue;
	if( sFileName.find( ".bme" ) == -1 )
		sFileName += ".bme";
	string sObjectName = pObjectName->m_sValue;
	std::transform( pObjectName->m_sValue.begin(), pObjectName->m_sValue.end(), sObjectName.begin(), tolower );
	string sAnimationName = pAnimationName->m_sValue;
	if( sAnimationName.find( ".bke" ) == -1 )
		sAnimationName += ".bke";
	
	ILoader::CAnimatableMeshData oData;
	m_pLoaderManager->Load( sFileName, oData );
	unsigned int i = 0;
	bool bFind = false;
	for( i = 0; i < oData.m_vMeshes.size(); i++ )
	{
		string sObjectNameLow = oData.m_vMeshes[ i ].m_sName;
		std::transform( oData.m_vMeshes[ i ].m_sName.begin(), oData.m_vMeshes[ i ].m_sName.end(), sObjectNameLow.begin(), tolower );
		if( sObjectNameLow == sObjectName )
		{
			bFind = true;
			break;
		}
	}
	if( bFind )
	{
		ILoader::CMeshInfos& mi = oData.m_vMeshes[ i ];
		IAnimation* pAnimation = static_cast< IAnimation* >( m_pRessourceManager->GetRessource( sAnimationName, *m_pRenderer ) );
		map< int, vector< CKey > > mBoneKeys;
		pAnimation->GetBoneKeysMap( mBoneKeys );
		IWeightTable* pWeightTable = m_pGeometryManager->CreateWeightTable();
		pWeightTable->BuildFromArrays( mi.m_vWeightVertex, mi.m_vWeigtedVertexID );

		map< int, int > mKeys;
		for( map< int, vector < CKey > >::iterator itBone = mBoneKeys.begin(); itBone != mBoneKeys.end(); itBone++ )
		{
			vector< CKey >& vKey = itBone->second;
			for( unsigned int i = 0; i < vKey.size(); i++ )
				mKeys[ vKey[ i ].m_nTimeValue ] = i;
		}

		mi.m_oKeyBoundingBoxes.clear();
		for( map< int, int >::iterator itKey = mKeys.begin(); itKey != mKeys.end(); itKey++ )
		{
			// On r�cup�re la matrice de passage de tous les bones du squelette
			map< int, CMatrix > mPassage;
			for( ILoader::TSkeletonMap::const_iterator itBone = oData.m_mBones.begin(); itBone != oData.m_mBones.end(); itBone++ )
			{
				// On r�cup�re la matrice de passage de la cl� initiale � la cl� courante
				CMatrix oPassage;
				map< int, vector< CKey > >::const_iterator itBoneKey = mBoneKeys.find( itBone->first );
				if( itBoneKey != mBoneKeys.end() )
				{
					// On r�cup�re la matrice de la cl� courante
					int nCurrentKeyIndex = 0;
					CMatrix oFirstKeyWorldTM, oCurrentKeyWorldTM, oFirstKeyWorldTMInv;
					bool bFoundKey = false;
					for( unsigned int iKey = 0; iKey < itBoneKey->second.size(); iKey++ )
					{
						if( itBoneKey->second.at( iKey ).m_nTimeValue == itKey->first )
						{
							oCurrentKeyWorldTM = itBoneKey->second.at( iKey ).m_oWorldTM;
							bFoundKey = true;
							break;
						}
					}
					if( !bFoundKey )
					{
						// Si il n'existe pas de cl� � cette position de l'animation, on cherche la pr�c�dent et la suivante
						unsigned int nLastTimeKey = 0, nNextTimeKey = -1;
						unsigned int nLastKeyIndex = 0, nNextKeyIndex = -1;
						for( unsigned int iKey = 0; iKey < itBoneKey->second.size(); iKey++ )
						{
							if( itBoneKey->second.at( iKey ).m_nTimeValue <  itKey->first )
							{
								if( nLastTimeKey < itBoneKey->second.at( iKey ).m_nTimeValue )
								{
									nLastTimeKey = itBoneKey->second.at( iKey ).m_nTimeValue;
									nLastKeyIndex = iKey;
								}
							}
							else 
							{
								if( nNextTimeKey > itBoneKey->second.at( iKey ).m_nTimeValue )
								{
									nNextTimeKey = itBoneKey->second.at( iKey ).m_nTimeValue;
									nNextKeyIndex = iKey;
								}
							}
						}
						// Une fois qu'on les a trouv�, on calcule la matrice interpol�e entre ces deux cl�s
						if( nNextKeyIndex == -1 )
							nNextKeyIndex = 0;
						const CMatrix& oLast = itBoneKey->second.at( nLastKeyIndex ).m_oWorldTM;
						const CMatrix& oNext = itBoneKey->second.at( nNextKeyIndex ).m_oWorldTM;
						float t = float( itKey->first - nLastTimeKey ) / ( nNextTimeKey - nLastTimeKey );
						CMatrix::GetInterpolationMatrix( oLast, oNext, oCurrentKeyWorldTM, t );
					}
					// On r�cup�re la matrice de la 1ere cl� :
					oFirstKeyWorldTM = itBoneKey->second.at( 0 ).m_oWorldTM;
					oFirstKeyWorldTM.GetInverse( oFirstKeyWorldTMInv );
					oPassage = oCurrentKeyWorldTM * oFirstKeyWorldTMInv;
				}
				mPassage[ itBone->first ] = oPassage;
			}

			// On parcours tous les vertex de la weight table et on calcul leur image
			IBox* pBox = m_pGeometryManager->CreateBox();
			map< int, bool > mComputedVertexIndex;
			for( unsigned int i = 0; i < mi.m_vIndex.size(); i++ )
			{
				int iIndex = mi.m_vIndex[ i ];
				map< int, bool >::const_iterator itVertexIndex = mComputedVertexIndex.find( iIndex );
				if( itVertexIndex == mComputedVertexIndex.end() )
				{
					mComputedVertexIndex[ iIndex ] = true;
					CVector v = CVector( mi.m_vVertex[ 3 * iIndex ], mi.m_vVertex[ 3 * iIndex + 1 ], mi.m_vVertex[ 3 * iIndex + 2 ] );
					map< int, float > mWeight;
					pWeightTable->Get( iIndex, mWeight );
					CMatrix oWeightedMatrix( 0.f );
					for( map< int, float >::const_iterator itBoneWeight = mWeight.begin(); itBoneWeight != mWeight.end(); itBoneWeight++ )
					{
						map< int, CMatrix >::const_iterator itPassage = mPassage.find( itBoneWeight->first );
						oWeightedMatrix = oWeightedMatrix + ( itPassage->second * itBoneWeight->second );
					}
					CVector v2 = oWeightedMatrix * v;
					pBox->AddPoint( v2 );
				}
			}
			string sAnimationNameWithoutExt = sAnimationName.substr( 0, sAnimationName.size() - 4 );
			mi.m_oKeyBoundingBoxes[ sAnimationNameWithoutExt ][ itKey->first ] = pBox;
		}
		m_pLoaderManager->Export( sFileName, oData );
	}
}

void LocalTranslate( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	if( pEntity )
	{
		CScriptFuncArgFloat* px = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
		CScriptFuncArgFloat* py = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 2 ) );
		CScriptFuncArgFloat* pz = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 3 ) );
		pEntity->LocalTranslate( px->m_fValue, py->m_fValue, pz->m_fValue );
	}
	else
	{
		ostringstream oss;
		oss << "Erreur : entit� " << pEntityID->m_nValue << " introuvable";
		m_pConsole->Println( oss.str() );
	}
}

void SetCameraType( IScriptState* pState )
{
	CScriptFuncArgString* pCamType = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	ICamera* pCamera = NULL;
	if( pCamType->m_sValue == "link" )
	{
		pCamera = m_pCameraManager->GetCameraFromType( ICameraManager::T_LINKED_CAMERA );
		m_pCameraManager->SetActiveCamera( pCamera );

		IEntity* pPerso = m_pEntityManager->GetPerso();
		if( pPerso )
			pCamera->Link( pPerso );
		else
			m_pConsole->Println( "Erreur : vous devez d�finir un personnage (fonction SetCurrentPerso(persoID)) avant de d�finir une cam�ra li�e." );
	}
	else if( pCamType->m_sValue == "free" )
	{
		pCamera = m_pCameraManager->GetCameraFromType( ICameraManager::T_FREE_CAMERA );
		m_pCameraManager->SetActiveCamera( pCamera );
	}
}

void SetCurrentPerso( IScriptState* pState )
{
	CScriptFuncArgInt* pPersoID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pPerso = m_pEntityManager->GetEntity( pPersoID->m_nValue );
	if(pPerso)
		m_pEntityManager->SetPerso( pPerso );
	else
	{
		ostringstream oss;
		oss << "Erreur : SetCurrentPerso(" << pPersoID->m_nValue << ") -> Id not exists";
		m_pConsole->Println(oss.str());
	}		
}

void SetGravity( IScriptState* pState )
{
	CScriptFuncArgFloat* pGravity = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 0 ) );
	pGravity->m_fValue;
}

void DisplayNodeInfos( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgInt* pNodeID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	if( pEntity )
	{
		IBone* pBone = static_cast< IBone* >( pEntity->GetSkeletonRoot()->GetChildBoneByID( pNodeID->m_nValue ) );		
		string sBoneName;
		pBone->GetName( sBoneName );
		sBoneName = string( "Bone \"" ) + sBoneName + "\" : ";
		m_pConsole->Println( sBoneName );
		CMatrix oTM;
		pBone->GetLocalMatrix( oTM );
		string sTM;
		m_pDebugTool->SetNumberWidth( 10 );
		m_pDebugTool->SerializeMatrix( oTM, 0.f, sTM );
		m_pConsole->Println( "Matrice locale : " );
		m_pConsole->Println( sTM );
		m_pDebugTool->SerializeMatrix( oTM, 0.f, sTM );
		m_pConsole->Println( "Matrice world : " );
		m_pConsole->Println( sTM );
	}
	else
	{
		ostringstream ossMessage;
		ossMessage << "Erreur d'identifiant pour l'entit� " << pEntityID->m_nValue;
		m_pConsole->Println( ossMessage.str() );
	}
}

void SetAnimationSpeed( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgString* pAnimationName = static_cast< CScriptFuncArgString* >( pState->GetArg( 1 ) );
	CScriptFuncArgFloat* pSpeed = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 2 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	if( pEntity )
	{
		IEntity::TAnimation eAnim = IEntity::eNone;
		if( pAnimationName->m_sValue == "stand" )
			eAnim = IEntity::eStand;
		else if( pAnimationName->m_sValue == "walk" )
			eAnim = IEntity::eWalk;
		else if( pAnimationName->m_sValue == "run" )
			eAnim = IEntity::eRun;		
		pEntity->SetAnimationSpeed( eAnim, pSpeed->m_fValue );
	}
	else
	{
		ostringstream oss;
		oss << "Erreur : entit� " << pEntityID->m_nValue << " inconnue";
		m_pConsole->Println( oss.str() );
	}
}

void StopRender( IScriptState* pState )
{
	CScriptFuncArgInt* pRender = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	m_bRenderScene = pRender->m_nValue == 1 ? false : true;
}

void Walk( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	//pEntity->Walk();
	pEntity->RunAction( "walk", true );
}

void Run( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	pEntity->RunAction( "run", true );
}

void Stand( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	if( pEntity )
		//pEntity->Stand();
		pEntity->RunAction( "stand", true );
	else
	{
		ostringstream oss;
		oss << "Erreur : Entit� " << pEntityID->m_nValue << " inconnue";
		m_pConsole->Println( oss.str() );
	}
}

void RunAction( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	if( pEntity )
	{
		CScriptFuncArgString* pAction = static_cast< CScriptFuncArgString* >( pState->GetArg( 1 ) );
		CScriptFuncArgInt* pLoop = static_cast< CScriptFuncArgInt* >( pState->GetArg( 2 ) );
		bool bLoop = pLoop->m_nValue == 1 ? true : false;
		pEntity->RunAction( pAction->m_sValue, bLoop );
	}
	else
	{
		ostringstream oss;
		oss << "Erreur : Entit� " << pEntityID->m_nValue << " inconnue";
		m_pConsole->Println( oss.str() );
	}
}

void SetScale( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	if( pEntity )
	{
		CScriptFuncArgFloat* px = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
		CScriptFuncArgFloat* py = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 2 ) );
		CScriptFuncArgFloat* pz = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 3 ) );
		pEntity->SetScaleFactor( px->m_fValue, py->m_fValue, pz->m_fValue );
	}
	else
	{
		ostringstream oss;
		oss << "Erreur : Entit� " << pEntityID->m_nValue << " inconnue";
		m_pConsole->Println( oss.str() );
	}
}

void CreateMobileEntity( IScriptState* pState )
{
	CScriptFuncArgString* pName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	string sName = pName->m_sValue;
	if( sName.find( ".bme" ) == -1 )
		sName += ".bme";
	bool bak = m_pRessourceManager->IsCatchingExceptionEnabled();
	m_pRessourceManager->EnableCatchingException( false );

	try
	{
		IEntity* pEntity = m_pEntityManager->CreateMobileEntity( sName, m_pFileSystem );
		pEntity->Link( m_pScene );
		ostringstream oss;
		oss << "L'entit� \"" << pName->m_sValue << "\"a �t� charg�e avec l'identifiant " << m_pEntityManager->GetEntityID( pEntity ) << ".";
		m_pConsole->Println( oss.str() );
	}
	catch( CFileNotFoundException& e )
	{		
		ostringstream oss;
		oss <<"Erreur : fichier \"" << e.m_sFileName << "\" manquant, l'entit� \"" << pName->m_sValue << "\" ne peut pas �tre charg�e." ;
		m_pConsole->Println( oss.str() );
	}
	catch( CRessourceException& e )
	{
		string s;
		e.GetErrorMessage( s );
		m_pConsole->Println( s );
	}
	catch( CBadFileFormat& e )
	{
		string sMessage;
		e.GetErrorMessage( sMessage );
		m_pConsole->Println( sMessage );
	}
	catch( CEException )
	{
		string sMessage = string( "\"" ) + sName + "\" introuvable";
		m_pConsole->Println( sMessage );
	}
	m_pRessourceManager->EnableCatchingException( bak );
}

void CreateNPC( IScriptState* pState )
{
	CScriptFuncArgString* pName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	string sName = pName->m_sValue;
	if( sName.find( ".bme" ) == -1 )
		sName += ".bme";
	bool bak = m_pRessourceManager->IsCatchingExceptionEnabled();
	m_pRessourceManager->EnableCatchingException( false );

	try
	{
		IEntity* pEntity = m_pEntityManager->CreateNPC( sName, m_pFileSystem );
		pEntity->Link( m_pScene );
		ostringstream oss;
		oss << "L'entit� \"" << pName->m_sValue << "\"a �t� charg�e avec l'identifiant " << m_pEntityManager->GetEntityID( pEntity ) << ".";
		m_pConsole->Println( oss.str() );
	}
	catch( CFileNotFoundException& e )
	{		
		ostringstream oss;
		oss <<"Erreur : fichier \"" << e.m_sFileName << "\" manquant, l'entit� \"" << pName->m_sValue << "\" ne peut pas �tre charg�e." ;
		m_pConsole->Println( oss.str() );
	}
	catch( CRessourceException& e )
	{
		string s;
		e.GetErrorMessage( s );
		m_pConsole->Println( s );
	}
	catch( CBadFileFormat& e )
	{
		string sMessage;
		e.GetErrorMessage( sMessage );
		m_pConsole->Println( sMessage );
	}
	catch( CEException )
	{
		string sMessage = string( "\"" ) + sName + "\" introuvable";
		m_pConsole->Println( sMessage );
	}
	m_pRessourceManager->EnableCatchingException( bak );
}

void DisplayAnimationTime( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	if( pEntity )
	{
		ostringstream oss;
		oss << pEntity->GetCurrentAnimation()->GetAnimationTime();
		m_pConsole->Println( oss.str() );
	}
	else
		m_pConsole->Println( "Erreur : identifiant incorrect" );
}

void SetAnimationTime( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgInt* pFrame = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	if( pEntity )
		pEntity->GetCurrentAnimation()->SetAnimationTime( pFrame->m_nValue );
	else
		m_pConsole->Println( "Erreur : identifiant incorrect" );
}

void NextAnimationKey( IScriptState* pState )
{
	IBone* pBone = dynamic_cast< IBone* >( m_pSelectedNode );
	if( pBone )
	{
		pBone->NextKey();
		pBone->Update();
	}
	else
		m_pConsole->Println( "Erreur : le noeud s�lectionn� n'est pas un bone" );
}

void NextAnimationFrame( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	if( pEntity )
		pEntity->GetCurrentAnimation()->NextFrame();
	else
		m_pConsole->Println( "Erreur : identifiant incorrect" );
}

void SetConstantLocalTranslate( IScriptState* pState )
{
	CScriptFuncArgInt* pEntityID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pEntityID->m_nValue );
	if( pEntity )
	{
		CScriptFuncArgFloat* px = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
		CScriptFuncArgFloat* py = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 2 ) );
		CScriptFuncArgFloat* pz = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 3 ) );
		pEntity->ConstantLocalTranslate( CVector( px->m_fValue, py->m_fValue, pz->m_fValue ) );
	}
}

void SetZCollisionError( IScriptState* pState )
{
	CScriptFuncArgFloat* pEpsilon = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 0 ) );
	m_pEntityManager->SetZCollisionError( pEpsilon->m_fValue );
}

void Link( IScriptState* pState )
{
	CScriptFuncArgInt* pIDEntity1 = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgInt* pIDNode1 = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
	CScriptFuncArgInt* pIDEntity2 = static_cast< CScriptFuncArgInt* >( pState->GetArg( 2 ) );
	CScriptFuncArgInt* pIDNode2 = static_cast< CScriptFuncArgInt* >( pState->GetArg( 3 ) );
	CScriptFuncArgString* pLinkType = static_cast< CScriptFuncArgString* >( pState->GetArg( 4 ) );

	IEntity* pEntity1 = m_pEntityManager->GetEntity( pIDEntity1->m_nValue );
	if( pEntity1 )
	{
		CNode* pNode1 = NULL;
		bool bEntity1 = false;
		bool bBone2 = false;
		IBone* pBone2 = NULL;
		if( pIDNode1->m_nValue != -1 )
			pNode1 = pEntity1->GetSkeletonRoot()->GetChildBoneByID( pIDNode1->m_nValue );
		else
		{
			pNode1 = pEntity1;
			bEntity1 = true;
		}

		if( pIDEntity2->m_nValue == -1 )
			pNode1->Unlink();
		else
		{
			IEntity* pEntity2 = m_pEntityManager->GetEntity( pIDEntity2->m_nValue );
			CNode* pNode2 = NULL;
			if( pIDNode2->m_nValue != -1 )
			{
				IBone* pSkeletonRoot = pEntity2->GetSkeletonRoot();
				if( pSkeletonRoot )
				{
					pNode2 = pSkeletonRoot->GetChildBoneByID( pIDNode2->m_nValue );
					pBone2 = dynamic_cast< IBone* >( pNode2 );
					if( pBone2 )
						bBone2 = true;
				}
				else
				{
					ostringstream oss;
					oss << "Erreur : l'entit� " << pIDEntity2->m_nValue << " ne poss�de pas de squelette";
					m_pConsole->Println( oss.str() );
					return;
				}
			}
			else
				pNode2 = pEntity2;

			if( bEntity1 && bBone2 )
			{
				IEntity::TLinkType t;
				if( pLinkType->m_sValue == "preserve" )
					t = IEntity::ePreserveChildRelativeTM;
				else if( pLinkType->m_sValue == "settoparent" )
					t = IEntity::eSetChildToParentTM;
				pEntity2->LinkEntityToBone( pEntity1, pBone2, t );
			}
			else
				pNode1->Link( pNode2 );
		}
	}
}

void HideEntity( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgInt* pHide = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	bool bHide = pHide->m_nValue == 1 ? true : false;
	pEntity->Hide( bHide );
}

void Sleep( IScriptState* pState )
{
	CScriptFuncArgInt* pTime = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	Sleep( pTime->m_nValue );
}

void StopAnimation( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	pEntity->GetCurrentAnimation()->Stop();
}

void DetachAnimation( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	pEntity->DetachCurrentAnimation();
}

void PauseAnimation( IScriptState* pState )
{
	CScriptFuncArgInt* pIDEntity = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	
	bool bGoodEntityID = true;
	if( pIDEntity )
	{
		CScriptFuncArgInt* pIDBool = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
		IEntity* pEntity = m_pEntityManager->GetEntity( pIDEntity->m_nValue );
		if( pEntity )
		{
			bool bPause = pIDBool->m_nValue == 1 ? true : false;
			pEntity->GetCurrentAnimation()->Pause( bPause );
		}
		else
			bGoodEntityID = false;
	}
	else
		bGoodEntityID = false;
	if( !bGoodEntityID )
	{
		ostringstream ossMessage;
		ossMessage << "Erreur, l'identifiant num�ro " << pIDEntity->m_nValue << " n'est pas valide";
		m_pConsole->Println( ossMessage.str() );
	}	
}

//ID entit�, ID bone
void SelectBone( IScriptState* pState )
{
	CScriptFuncArgInt* pIDEntity = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgInt* pIDBone = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pIDEntity->m_nValue );
	if( pEntity )
	{
		IBone* pSkeleton = pEntity->GetSkeletonRoot();
		if( pSkeleton )
		{
			m_pSelectedNode = pSkeleton->GetChildBoneByID( pIDBone->m_nValue );
			if( m_pSelectedNode )
			{
				m_eSelectionType = eBone;
				string sBoneName;
				m_pSelectedNode->GetName( sBoneName );
				string sMessage = string( "Bone \"" ) + sBoneName + "\" s�lectionn�";
				m_pConsole->Println( sMessage );
			}
			else
				m_pConsole->Println( "Identifiant de bone incorrect" );
		}
		else
			m_pConsole->Println( "Erreur : L'entit� s�lectionn� n'a pas de squelette" );
	}
	else
		m_pConsole->Println( "Identifiant d'entit� incorrect" );
}

void Yaw( IScriptState* pState )
{
	if( m_pSelectedNode )
	{
		CScriptFuncArgFloat* pAngle = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 0 ) );
		m_pSelectedNode->Yaw( pAngle->m_fValue );
		m_pSelectedNode->Update();
	}
}

void Pitch( IScriptState* pState )
{
	if( m_pSelectedNode )
	{
		CScriptFuncArgFloat* pAngle = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 0 ) );
		m_pSelectedNode->Pitch( pAngle->m_fValue );
		m_pSelectedNode->Update();
	}
}

void Roll( IScriptState* pState )
{
	if( m_pSelectedNode )
	{
		CScriptFuncArgFloat* pAngle = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 0 ) );
		m_pSelectedNode->Roll( pAngle->m_fValue );
		m_pSelectedNode->Update();
	}
}

void GetSkeletonInfos( CNode* pNode, vector< CNodeInfos >& vInfos )
{
	IBone* pBone = dynamic_cast< IBone* >( pNode );
	if( pBone )
	{
		CNodeInfos ni;
		pNode->GetName( ni.m_sName );
		ni.m_nID = pNode->GetID();
		vInfos.push_back( ni );
		for( unsigned int i = 0; i < pNode->GetChildCount(); i++ )
			GetSkeletonInfos( pNode->GetChild( i ), vInfos );
	}
}

void DisplayEntitySkeletonInfos( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	if( pEntity )
	{
		vector< CNodeInfos > vInfos;
		GetSkeletonInfos( pEntity->GetSkeletonRoot(), vInfos );
		ostringstream oss;
		for( unsigned int i = 0; i < vInfos.size(); i++ )
		{
			oss.str( "" );
			oss << "Nom : " << vInfos[ i ].m_sName << " , ID : " << vInfos[ i ].m_nID;
			m_pConsole->Println( oss.str() );
		}
	}
	else
		m_pConsole->Println( "Erreur d'identifiant" );
}

void reset( IScriptState* pState )
{
	m_pSceneManager->ClearScene( m_pScene );	
	RunScript( "start.eas" );
}


void SetAnimation( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >(  pState->GetArg( 0 ) );
	CScriptFuncArgString* pAnim = static_cast< CScriptFuncArgString* >( pState->GetArg( 1 ) );
	string sAnimationFileName = pAnim->m_sValue + ".bke";
	IEntity* pEntity = dynamic_cast< IEntity* >( m_pEntityManager->GetEntity( pID->m_nValue ) );
	if( pEntity )
	{
		try
		{
			if( !pEntity->HasAnimation( sAnimationFileName ) )
				pEntity->AddAnimation( sAnimationFileName );
			pEntity->SetCurrentAnimation( sAnimationFileName );
		}
		catch( CFileNotFoundException& e )
		{
			string sMessage = string( "fichier \"" ) + e.m_sFileName + "\" introuvable";
			m_pConsole->Println( sMessage );
		}
		catch( CEException&e )
		{
			string sMessage;
			e.GetErrorMessage( sMessage );
			m_pConsole->Println( sMessage );
		}
	}
	else
		m_pConsole->Println( "Erreur : L'identifiant entr� ne correspond pas � celui d'une entit� animable" );
}

void PlayCurrentAnimation( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	if( pEntity )
	{
		IAnimation* pAnimation = pEntity->GetCurrentAnimation();
		if( pAnimation )
			pAnimation->Play( true );
		else
			m_pConsole->Println( "Errreur : L'entit� s�lectionn�e est animable mais ne contient pas l'animation demand�e." );
	}
	else
		m_pConsole->Println( "Erreur : Identifiant incorrect" );
}
//
//void ClearRessources( IScriptState* pState )
//{
//	m_pSceneManager->ClearScene( m_pScene );
//	m_pRessourceManager->DestroyAllRessources();
//}

void LoadShader( IScriptState* pState )
{
	CScriptFuncArgString* pShaderName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	m_pRenderer->LoadShader( pShaderName->m_sValue, *m_pFileSystem );
}

void DisplayShaderName( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	string sShaderName;
	pEntity->GetRessource()->GetCurrentShader()->GetName( sShaderName );
	m_pConsole->Println( sShaderName );
}

void SetHMPrecision( IScriptState* pState )
{
	CScriptFuncArgInt* pPrecision = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	m_pCollisionManager->SetHeightMapPrecision( pPrecision->m_nValue );
}

void ExtractHM( IScriptState* pState )
{
	CScriptFuncArgString* pFileName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	string sFileName = pFileName->m_sValue + ".bmp";
	string sOutFileName = pFileName->m_sValue + "_extract.bmp";
	try
	{
		m_pCollisionManager->ExtractHeightMapFromTexture( sFileName, sOutFileName );
	}
	catch( CEException& e )
	{
		string sMessage;
		e.GetErrorMessage( sMessage );
		sMessage = string( "Erreur : ") + sMessage;
		m_pConsole->Println( sMessage );
	}
}

void StopDisplayHM( IScriptState* pState )
{
	m_pCollisionManager->StopDisplayHeightMap();
}

void DisplayHM( IScriptState* pState )
{
	CScriptFuncArgString* pString = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	string sFileName = pString->m_sValue;
	if( sFileName.find( ".bme" ) == -1 )
		sFileName += ".bme";
	IMesh* pMesh = dynamic_cast< IMesh* >( m_pRessourceManager->GetRessource( sFileName, *m_pRenderer ) );
	if( pMesh )
		m_pCollisionManager->DisplayHeightMap( pMesh );
	else
		m_pConsole->Println("Erreur : ressource non valide");
}

void DisplaySceneChilds( IScriptState* pState )
{
	string sName;
	for( unsigned int i = 0; i < m_pScene->GetChildCount(); i++ )
	{
		m_pScene->GetChild( i )->GetName( sName );
		m_pConsole->Println( sName );
	}
}

void flist( IScriptState* pState )
{
	CScriptFuncArgString* pString = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	string sBegin = pString->m_sValue;
	transform( pString->m_sValue.begin(), pString->m_sValue.end(), sBegin.begin(), tolower );
	vector< string > vFuncNames;
	m_pScriptManager->GetRegisteredFunctions( vFuncNames );
	for( unsigned int i = 0; i < vFuncNames.size(); i++ )
	{
		string sFuncNameLow = vFuncNames[ i ];
		transform( vFuncNames[ i ].begin(), vFuncNames[ i ].end(), sFuncNameLow.begin(), tolower );
		if( sFuncNameLow.find( sBegin ) != -1 )
			m_pConsole->Println( vFuncNames[ i ] );
	}
}

void DisplayLightIntensity( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pLightEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	float fIntensity = m_pRessourceManager->GetLightIntensity( pLightEntity->GetRessource() );
	ostringstream oss;
	oss << fIntensity;
	m_pConsole->Println( oss.str() );
}

void SetReperePos( IScriptState* pState )
{
	CScriptFuncArgFloat* px = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 0 ) );
	CScriptFuncArgFloat* py = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
	CScriptFuncArgFloat* pz = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 2 ) );
	m_pRepere->SetWorldPosition( px->m_fValue, py->m_fValue, pz->m_fValue );
}

void RunScript( string sFileName )
{
	FILE* pFile = m_pFileSystem->OpenFile( sFileName, "r" );
	if( pFile )
	{
		fseek( pFile, 0, SEEK_END );
		long size = ftell( pFile );
		fseek( pFile, 0, SEEK_SET );
		string script;
		script.resize( size );
		fread( &script[ 0 ], sizeof( char ), size, pFile );
		try
		{
			m_pScriptManager->ExecuteCommand( script );
		}
		catch( CCompilationErrorException& e )
		{
			string sMessage;
			e.GetErrorMessage( sMessage );
			m_pConsole->Println( sMessage );
		}
		fclose( pFile );
	}
	else
		m_pConsole->Println( "Fichier introuvable" );
}

void run( IScriptState* pState )
{
	CScriptFuncArgString* pName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	string sFileName = pName->m_sValue;
	if( sFileName.find( ".eas" ) == -1 )
		sFileName += ".eas";
	RunScript( sFileName );
}

void LoadImage( IScriptState* pState )
{
	CScriptFuncArgString* pName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	ILoader::CTextureInfos ti;
	ti.m_bFlip = true;
	try
	{
		m_pLoaderManager->Load( pName->m_sValue, ti );
		m_pConsole->Println("Fichier charg�.");
	}
	catch( ILoaderManager::CBadExtension& )
	{
		m_pConsole->Println("Erreur : extension de fichier non g�r�e.");
	}
}

void DisplayRepere( IScriptState* pState )
{
	CScriptFuncArgInt* pDisplay = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	if( pDisplay->m_nValue == 1 && m_pRepere->GetParent() != m_pScene )
		m_pRepere->Link( m_pScene );
	else if( pDisplay->m_nValue == 0 )
		m_pRepere->Unlink();
}

DWORD WINAPI ExportBMEToAsciiCallback(void* lpThreadParameter)
{
	vector<string>* callbackArg = (vector<string>*)lpThreadParameter;
	string sBMEName = callbackArg->at(0);
	string sOutputName = callbackArg->at(1);
	delete callbackArg;
	ILoader::CAnimatableMeshData oData;
	ILoader* pLoader = m_pLoaderManager->GetLoader("bme");
	pLoader->Load(sBMEName, oData, *m_pFileSystem);
	for (int i = 0; i < oData.m_vMessages.size(); i++) {
		m_pConsole->Println(oData.m_vMessages[i]);
	}

	m_pConsole->Println("Export en cours...");
	
	oData.m_vMessages.clear();
	pLoader->SetAsciiExportPrecision(7);
	try {
		pLoader->Export(sOutputName, oData);
		m_pConsole->Println("Export termin�");
	}
	catch (exception& e) {
		char msg[256];
		sprintf(msg, "Error while accessing \"%s\"", e.what());
		m_pConsole->Println(msg);
	}
	m_pConsole->EnableInput(true);
	return 0;
}

void ExportBMEToAscii( IScriptState* pState )
{
	CScriptFuncArgString* pFileName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	CScriptFuncArgString* pOutFileName = static_cast< CScriptFuncArgString* >(pState->GetArg(1));
	string sBMEName = pFileName->m_sValue;
	int nExtPos = (int)sBMEName.find( ".bme" );
	string sFileNameWithoutExt;
	if( nExtPos == -1 )
	{
		sFileNameWithoutExt = sBMEName;
		sBMEName += ".bme";		
	}
	else
		sFileNameWithoutExt = sBMEName.substr(0, nExtPos );
	string sOutputName = pOutFileName ? pOutFileName->m_sValue : sFileNameWithoutExt + ".txt";
	

	vector<string>* callbackArg = new vector<string>;
	callbackArg->push_back(sBMEName);
	callbackArg->push_back(sOutputName);
	DWORD threadID;
	CreateThread(NULL, 0, ExportBMEToAsciiCallback, callbackArg, 0, &threadID);

	m_pConsole->EnableInput(false);

	return;
	ILoader::CAnimatableMeshData oData;
	ILoader* pLoader = m_pLoaderManager->GetLoader( "bme" );
	pLoader->Load( sBMEName, oData, *m_pFileSystem );	
	for (int i = 0; i < oData.m_vMessages.size(); i++) {
		m_pConsole->Println(oData.m_vMessages[i]);
	}
	pLoader->SetAsciiExportPrecision( 7 );
	try{ 
		pLoader->Export(sOutputName, oData);
		m_pConsole->Println("Export termin�");
	}
	catch (exception& e) {
		char msg[256];
		sprintf(msg, "Error while accessing \"%s\"", e.what());
		m_pConsole->Println(msg);
	}
}

void ExportBKEToAscii( IScriptState* pState )
{
	CScriptFuncArgString* pFileName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	CScriptFuncArgInt* pPrecision = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
	string sBKEName = pFileName->m_sValue;
	int nExtPos = (int)sBKEName.find( ".bke" );
	string sFileNameWithoutExt;
	if( nExtPos == -1 )
	{
		sFileNameWithoutExt = sBKEName;
		sBKEName += ".bke";
	}
	else
		sFileNameWithoutExt = sBKEName.substr( nExtPos );
	string sTXTName = sFileNameWithoutExt + ".txt";
	ILoader::CAnimationInfos oData;
	ILoader* pLoader = m_pLoaderManager->GetLoader( "bke" );
	pLoader->Load( sBKEName, oData, *m_pFileSystem );
	pLoader->SetAsciiExportPrecision( pPrecision->m_nValue );
	pLoader->Export( sTXTName, oData );
	string sMessage = string( "Fichier export� dans \"" ) + sTXTName + "\"";
	m_pConsole->Println( sMessage );
}

void ClearScene( IScriptState* pState )
{
	ICamera* pLinkedCamera = m_pCameraManager->GetCameraFromType( ICameraManager::T_LINKED_CAMERA );
	if( pLinkedCamera->GetParent() )
		pLinkedCamera->Unlink();
	CMatrix oLinkedMatrix;
	pLinkedCamera->SetLocalMatrix( oLinkedMatrix );

	m_pSceneManager->ClearScene( m_pScene );

	ICamera* pCamera = m_pCameraManager->GetCameraFromType( ICameraManager::T_FREE_CAMERA );
	m_pCameraManager->SetActiveCamera( pCamera );
	m_pEntityManager->AddEntity( pCamera, "FreeCamera" );
	m_pEntityManager->AddEntity( pLinkedCamera, "LinkedCamera" );
	m_pRepere = m_pEntityManager->CreateRepere( *m_pRenderer );
	m_pRepere->Link( m_pScene );
	m_pEntityManager->AddEntity( m_pScene, "SceneGame", 0 );

	//m_pRessourceManager->DestroyAllRessources();
}

void SetSceneMap( IScriptState* pState )
{
	CScriptFuncArgString* pFileName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	string sFileName = pFileName->m_sValue;
	int nExtPos = (int)pFileName->m_sValue.find( ".bme" );
	if( nExtPos == -1 )
		sFileName = pFileName->m_sValue + ".bme";
	bool bak = m_pRessourceManager->IsCatchingExceptionEnabled();
	m_pRessourceManager->EnableCatchingException( false );
	try
	{
		m_pScene->SetRessource( sFileName, *m_pRessourceManager, *m_pRenderer );
		string sError;
		m_pRessourceManager->PopErrorMessage( sError );
		if( sError.size() > 0 )
			m_pConsole->Println( sError );
	}
	catch( CRessourceException& e )
	{
		string s;
		e.GetErrorMessage( s );
		m_pConsole->Println( s );
	}
	catch( CBadFileFormat& )
	{
		m_pConsole->Println( "Mauvais format de fichier, essayez de le r�exporter" );
	}
	catch( CFileNotFoundException& e )
	{
		string sFileName;
		e.GetErrorMessage( sFileName );
		string sMessage = string("Erreur : fichier \"") + sFileName + "\" manquant";
		m_pConsole->Println( sMessage );
	}
	catch( CEException&  )
	{
		string sMessage = string( "\"" ) + sFileName + "\" introuvable";
		m_pConsole->Println( sMessage );
	}
	m_pRessourceManager->EnableCatchingException( bak );
}

void SetEntityWeight( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgFloat* pWeight = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	if( pEntity )
		pEntity->SetWeight( pWeight->m_fValue );
	else
	{
		ostringstream oss;
		oss << "Erreur : Entit� " << pID->m_nValue << " introuvable";
		m_pConsole->Println( oss.str() );
	}
}

void LoadHM( IScriptState* pState )
{
	CScriptFuncArgString* pFileName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	vector< vector< unsigned char > > vPixels;
	try
	{
		m_pCollisionManager->LoadHeightMap( pFileName->m_sValue, vPixels );
		m_pConsole->Println( "Height map correctement charg�e" );
	}
	catch ( ILoaderManager::CBadExtension&  )
	{
		m_pConsole->Println( "Erreur -> fichier non trouv�" );
	}
	int test = 0;
}

void GetFreeFileName( string sPrefixName, string sExtension, string& sFileName )
{
	ostringstream ossFile;
	int i = 0;
	FILE* pFile = NULL;
	do
	{
		ossFile << sPrefixName << i << "." << sExtension;
		fopen_s( &pFile, ossFile.str().c_str(), "r" );
		if( pFile )
		{
			fclose( pFile );
			i++;
			ossFile.str( "" );
		}
	}
	while( pFile );
	sFileName = ossFile.str();
}

void CreateHM( IScriptState* pState )
{
	CScriptFuncArgString* pString = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	string sFileName = pString->m_sValue;
	if( sFileName.find( ".bme" ) == -1 )
		sFileName += ".bme";
	IEntity* pEntity = NULL;
	try
	{
		pEntity = m_pEntityManager->CreateEntity( sFileName, "",  *m_pRenderer );
	}
	catch( CEException& e )
	{
		string sError;
		e.GetErrorMessage( sError );
		string s = string( "Erreur : " ) + sError;
		m_pConsole->Println( s );
	}
	if( pEntity )
	{
		IMesh* pMesh = dynamic_cast< IMesh* >( pEntity->GetRessource() );
		if( pMesh )
		{
			ILoader::CTextureInfos ti;
			m_pCollisionManager->CreateHeightMap( pMesh, ti, IRenderer::T_BGR );
			ti.m_ePixelFormat = ILoader::eBGR;
			string sTextureFileName = string( "HM_" ) + pString->m_sValue + ".bmp";
			m_pLoaderManager->Export( sTextureFileName, ti );
		}
	}
	else
		m_pConsole->Println( "Une erreur s'est produite lors de la cr�ation de l'entit�" );
}

void ScreenCapture( IScriptState* pState )
{
	ostringstream ossFile;
	int i = 0;
	FILE* pFile = NULL;
	do
	{
		ossFile << "Capture_" << i << ".bmp";
		fopen_s( &pFile, ossFile.str().c_str(), "r" );
		if( pFile )
		{
			fclose( pFile );
			i++;
			ossFile.str( "" );
		}
	}
	while( pFile );
	CRenderUtils::ScreenCapture( ossFile.str(), m_pRenderer, m_pLoaderManager, m_pSceneManager->GetScene( "Game" ) );
}

void SetLightIntensity( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgFloat* pIntensity = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
	try
	{
		m_pEntityManager->SetLightIntensity( pID->m_nValue, pIntensity->m_fValue );
	}
	catch( CBadTypeException& )
	{
		ostringstream oss;
		oss << "Erreur : " << pID->m_nValue << " n'est pas un identifiant de lumi�re";
		m_pConsole->Println("Erreur : ");
	}
}

void AddLight( IScriptState* pState )
{
	CScriptFuncArgInt* pr = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgInt* pg = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
	CScriptFuncArgInt* pb = static_cast< CScriptFuncArgInt* >( pState->GetArg( 2 ) );
	CScriptFuncArgString* pType = static_cast< CScriptFuncArgString* >( pState->GetArg( 3 ) );
	CScriptFuncArgFloat* pIntensity = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 4 ) );
	CVector Color( (float)pr->m_nValue / 255.f, (float)pg->m_nValue / 255.f, (float)pb->m_nValue / 255.f, 1.f );
	string sType = pType->m_sValue;
	transform( pType->m_sValue.begin(), pType->m_sValue.end(), sType.begin(), tolower );
	IRessource::TLight type;
	if( sType == "omni" )
		type = IRessource::OMNI;
	else if( sType == "dir" )
		type = IRessource::DIRECTIONAL;
	else if( sType == "spot" )
		type = IRessource::SPOT;
	else
	{
		m_pConsole->Println( "Param�tre 4 invalide, vous devez entrer un type de lumi�re parmis les 3 suivants : \"omni\" , \"dir\" , \"spot\" " );
		return;
	}
	IEntity* pEntity = m_pEntityManager->CreateLightEntity( Color, type, pIntensity->m_fValue );
	pEntity->Link( m_pScene );
	ostringstream oss;
	oss << "La lumi�re a �t� cr��e avec l'identifiant " << m_pEntityManager->GetEntityID( pEntity );;
	m_pConsole->Println( oss.str() );
}

void AddLightw( IScriptState* pState )
{
	CScriptFuncArgString* pType = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	CScriptFuncArgFloat* pIntensity = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
	CVector Color( 1.f, 1.f, 1.f, 1.f );
	string sType = pType->m_sValue;
	transform( pType->m_sValue.begin(), pType->m_sValue.end(), sType.begin(), tolower );
	IRessource::TLight type;
	if( sType == "omni" )
		type = IRessource::OMNI;
	else if( sType == "dir" )
		type = IRessource::DIRECTIONAL;
	else if( sType == "spot" )
		type = IRessource::SPOT;
	else
	{
		m_pConsole->Println( "Param�tre 4 invalide, vous devez entrer un type de lumi�re parmis les 3 suivants : \"omni\" , \"dir\" , \"spot\" " );
		return;
	}
	IEntity* pEntity = m_pEntityManager->CreateLightEntity( Color, type, pIntensity->m_fValue );
	pEntity->Link( m_pScene );
	ostringstream oss;
	oss << "La lumi�re a �t� cr��e avec l'identifiant " << m_pEntityManager->GetEntityID( pEntity );;
	m_pConsole->Println( oss.str() );
}

void RollEntity( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgFloat* pRoll = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	pEntity->Roll( pRoll->m_fValue );
}

void PitchEntity( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgFloat* pPitch = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	if( pEntity )
		pEntity->Pitch( pPitch->m_fValue );
	else
		m_pConsole->Println("Identifiant invalide");
}

void YawEntity( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgFloat* pYaw = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	pEntity->Yaw( pYaw->m_fValue );
}

void SetEntityShader( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgString* pShaderName = static_cast< CScriptFuncArgString* >( pState->GetArg( 1 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	IShader* pShader = m_pRenderer->GetShader( pShaderName->m_sValue );
	pEntity->SetShader( pShader );
}

void DisplayNodeInfos( CNode* pNode, int nLevel = 0 )
{
	IEntity* pEntity = dynamic_cast< IEntity* >( pNode );
	if( pEntity )
	{
		
		ostringstream sLine;
		for( int j = 0; j < nLevel; j++ )
			sLine << "\t";
		string sEntityName;
		pEntity->GetName( sEntityName );
		sLine << "Entity name = " << sEntityName << ", ";
		IRessource* pRessource = pEntity->GetRessource();
		if( pRessource )
		{
			string sRessourceFileName;
			pRessource->GetFileName( sRessourceFileName );
			if( sRessourceFileName.size() > 0 )
				sLine << sRessourceFileName;
			sLine << ", ID = " << m_pEntityManager->GetEntityID( pEntity );
			m_pConsole->Println( sLine.str() );
		}
		else
		{
			IEntity* pRepere = dynamic_cast< IEntity* >( pRessource );
			if( pRepere )
				m_pConsole->Println( "Repere" );
		}
		CNode* pSkeleton = pEntity->GetSkeletonRoot();
		if( pSkeleton )
			DisplayNodeInfos( pSkeleton );
	}
	for( unsigned int i = 0; i < pNode->GetChildCount(); i++ )
		DisplayNodeInfos( pNode->GetChild( i ), nLevel + 1 );
}

void DisplayEntities( IScriptState* pState )
{
	DisplayNodeInfos( m_pScene );
}

void GetEntityID( IScriptState* pState )
{
	ostringstream oss;
	CScriptFuncArgString* pName = static_cast< CScriptFuncArgString* >(  pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pName->m_sValue );
	if( pEntity )
	{
		int nID = m_pEntityManager->GetEntityID( pEntity );
		oss << nID;
		m_pConsole->Println( oss.str() );
	}
	else
	{
		oss << "Entit� \"" << pName->m_sValue << "\" introuvable";
		m_pConsole->Println( oss.str() );
	}
}

void DisplayBBox( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgInt* pDraw = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	bool bDraw = pDraw->m_nValue == 1 ? true : false;
	pEntity->DrawBoundingBox( bDraw );
}

void DisplayBBoxInfos( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	IBox* pBox = pEntity->GetBBox();
	ostringstream oss;
	oss << "Dimensions : ( " << pBox->GetDimension().m_x << ", " << pBox->GetDimension().m_y << ", " << pBox->GetDimension().m_z << ")  "
		<< " min point : " << pBox->GetMinPoint().m_x << ", " << pBox->GetMinPoint().m_y << ", " << pBox->GetMinPoint().m_z << ")";
	m_pConsole->Println( oss.str() );
	delete pBox;
}

void DisplayBkgColor( IScriptState* pState )
{
	CVector vColor;
	m_pRenderer->GetBackgroundColor( vColor );
	ostringstream oss;
	oss << (int)( vColor.m_x * 255. )<< " , " << (int)( vColor.m_y * 255. )<< " , " << (int)( vColor.m_z * 255. );
	m_pConsole->Println( oss.str() );
}

void SetBkgColor( IScriptState* pState )
{
	vector< int > v;
	for( int i = 0; i < 3; i++ )
	{
		CScriptFuncArgInt* pArg = static_cast< CScriptFuncArgInt* >( pState->GetArg( i ) );
		v.push_back( pArg->m_nValue );
	}
	m_pRenderer->SetBackgroundColor( v[0 ], v[ 1 ], v[ 2 ], 1 );
}

void DisplayCamPos( IScriptState* pState )
{
	CVector vPos;
	m_pCameraManager->GetActiveCamera()->GetWorldPosition( vPos );
	ostringstream oss;
	oss << vPos.m_x << " , " << vPos.m_y << " , " << vPos.m_z;
	m_pConsole->Println( oss.str() );
}

void SetCamPos( IScriptState* pState )
{
	CScriptFuncArgFloat* px = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 0 ) );
	CScriptFuncArgFloat* py = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
	CScriptFuncArgFloat* pz = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 2 ) );
	m_pCameraManager->GetActiveCamera()->SetWorldPosition( px->m_fValue, py->m_fValue, pz->m_fValue );
}

void SetEntityPos( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgFloat* px = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
	CScriptFuncArgFloat* py = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 2 ) );
	CScriptFuncArgFloat* pz = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 3 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	if( pEntity )
		pEntity->SetWorldPosition( px->m_fValue, py->m_fValue, pz->m_fValue );
	else
		m_pConsole->Println("Identifiant invalide");
}

void DisplayEntityPosition( IScriptState* pState )
{
	CScriptFuncArgInt* pInt = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pInt->m_nValue );
	CVector vPos;
	pEntity->GetWorldPosition( vPos );
	ostringstream oss;
	oss << vPos.m_x << " , " << vPos.m_y << " , " << vPos.m_z;
	m_pConsole->Println( oss.str() );
}

void Exit( IScriptState* pState )
{
	m_pWindow->Close();
}

void LoadEntity( IScriptState* pState )
{
	CScriptFuncArgString* pName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	string sName = pName->m_sValue;
	if( sName.find( ".bme" ) == -1 )
		sName += ".bme";
	bool bak = m_pRessourceManager->IsCatchingExceptionEnabled();
	m_pRessourceManager->EnableCatchingException( false );
	try
	{
		IEntity* pEntity = m_pEntityManager->CreateEntity( sName, "", *m_pRenderer );
		pEntity->Link( m_pScene );
		//pEntity->Pitch( -90.f );
		ostringstream oss;
		oss << "L'entit� \"" << pName->m_sValue << "\"a �t� charg�e avec l'identifiant " << m_pEntityManager->GetEntityID( pEntity ) << ".";
		m_pConsole->Println( oss.str() );
	}
	catch( CFileNotFoundException& e )
	{		
		string sMessage = string( "Erreur : fichier \"" ) + e.m_sFileName + "\" manquant.";
		m_pConsole->Println( sMessage );
	}
	catch( CRessourceException& e )
	{
		string s;
		e.GetErrorMessage( s );
		m_pConsole->Println( s );
	}
	catch( CBadFileFormat )
	{
		ostringstream oss;
		oss << "\"" << sName << "\" : Mauvais format de fichier, essayez de le r�exporter";
		m_pConsole->Println( oss.str() );
	}
	catch( CEException )
	{
		string sMessage = string( "\"" ) + sName + "\" introuvable";
		m_pConsole->Println( sMessage );
	}
	m_pRessourceManager->EnableCatchingException( bak );
}

void SaveScene( IScriptState* pState )
{
	try
	{
		CScriptFuncArgString* pName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
		string sFileName = pName->m_sValue;
		if( pName->m_sValue.find( '.' ) == -1 )
			sFileName += ".bse";
		m_pSceneManager->Export( m_pScene, sFileName );
		//m_pScene->Export( sFileName );
		m_pConsole->Println( "Sc�ne sauvegard�e" );
	}
	catch( exception e )
	{
		m_pConsole->Println( e.what() );
	}
}

void LoadScene( IScriptState* pState )
{
	CScriptFuncArgString* pName = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	string sFileName = pName->m_sValue;
	if( sFileName.find( '.' ) == -1 )
		sFileName += ".bse";
	try
	{
		m_pSceneManager->Load( m_pScene, sFileName );
	}
	catch( CFileNotFoundException )
	{
		string s = string("Fichier \"") + sFileName + "\" introuvable";
		m_pConsole->Println( s );
	}
	catch( CExtensionNotFoundException )
	{
		//m_pConsole->Println( "L'extension \"bse\" n'est pas g�r�e par le gestionnaire de ressource" );
		m_pConsole->Println( "Erreur inconnue, veuillez contacter l'administrateur pour plus d'information" );
	}
	catch( CBadFileFormat )
	{
		m_pConsole->Println( "Mauvais format de fichier,essayez de r�exporter la scene" );
	}
	catch( CEException& e )
	{
		m_pConsole->Println(e.what());
	}
}

void Merge( IScriptState* pState )
{
	CScriptFuncArgString* pString = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	CScriptFuncArgString* pType = static_cast< CScriptFuncArgString* >( pState->GetArg( 1 ) );
	CScriptFuncArgFloat* px = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 2 ) );
	CScriptFuncArgFloat* py = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 3 ) );
	CScriptFuncArgFloat* pz = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 4 ) );
	
	m_pSceneManager->Merge( m_pScene, pString->m_sValue, pType->m_sValue, px->m_fValue, py->m_fValue, pz->m_fValue );
}

void TestMessageBox( IScriptState* pState )
{
	CScriptFuncArgString* pMessage = static_cast< CScriptFuncArgString* >( pState->GetArg( 0 ) );
	CScriptFuncArgString* pCaption = static_cast< CScriptFuncArgString* >( pState->GetArg( 1 ) );
	CScriptFuncArgInt* pBoxType = static_cast< CScriptFuncArgInt* >( pState->GetArg( 2 ) );
	MessageBox( NULL, pMessage->m_sValue.c_str(), pCaption->m_sValue.c_str(), pBoxType->m_nValue );
}

void Operation( IScriptState* pState )
{
	CScriptFuncArgFloat* p0 = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 0 ) );
	CScriptFuncArgFloat* p1 = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 1 ) );
	CScriptFuncArgFloat* p2 = static_cast< CScriptFuncArgFloat* >( pState->GetArg( 2 ) );
	ostringstream oss;
	oss << "arg 0 = " << p0->m_fValue << "\narg 1 = " << p1->m_fValue << "\narg 2 = " << p2->m_fValue;
	m_pConsole->Println( oss.str() );
}

void Operation3( IScriptState* pState )
{
	CScriptFuncArgInt* p0 = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	CScriptFuncArgInt* p1 = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
	CScriptFuncArgInt* p2 = static_cast< CScriptFuncArgInt* >( pState->GetArg( 2 ) );
	ostringstream oss;
	oss << "arg 0 = " << p0->m_nValue << "\narg 1 = " << p1->m_nValue << "\narg 2 = " << p2->m_nValue;
	MessageBox( NULL, oss.str().c_str(), "", MB_OK );
}

void cls( IScriptState* pState )
{
	m_pConsole->Cls();
}

void SetRenderType( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	if( pEntity )
	{
		CScriptFuncArgString* pType = static_cast< CScriptFuncArgString* >( pState->GetArg( 1 ) );
		if( pType->m_sValue == "line" )
			pEntity->SetRenderingType( IRenderer::eLine );
		else if( pType->m_sValue == "fill" )
			pEntity->SetRenderingType( IRenderer::eFill );
		else if( pType->m_sValue == "point" )
			pEntity->SetRenderingType( IRenderer::ePoint );
	}
}

void DisplayBoundingSphere( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	if( pEntity )
	{
		CScriptFuncArgInt* pBool = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
		bool bDraw = pBool->m_nValue == 1 ? true : false;
		pEntity->DrawBoundingSphere( bDraw );
	}
}

void DisplayBoneBoundingSphere( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	if( pEntity )
	{
		CScriptFuncArgInt* pBoneID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 1 ) );
		CScriptFuncArgInt* pBool = static_cast< CScriptFuncArgInt* >( pState->GetArg( 2 ) );
		bool bDraw = pBool->m_nValue == 1 ? true : false;
		pEntity->DrawBoneBoundingSphere( pBoneID->m_nValue, bDraw );
	}
}

void Unlink( IScriptState* pState )
{
	CScriptFuncArgInt* pID = static_cast< CScriptFuncArgInt* >( pState->GetArg( 0 ) );
	IEntity* pEntity = m_pEntityManager->GetEntity( pID->m_nValue );
	if( pEntity )
		pEntity->Unlink();
}

void RegisterAllFunctions( IScriptManager* pScriptManager )
{
	vector< TFuncArgType > vType;

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "DisplayBoneBoundingSphere", DisplayBoneBoundingSphere, vType );

	vType.push_back( eString );
	vType.push_back( eString );
	vType.push_back( eFloat);
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "Merge", Merge, vType );

	vType.clear();
	vType.push_back( eString );
	vType.push_back( eString );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "MessageBox", TestMessageBox, vType );

	vType.clear();
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "Operation", Operation, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "Operation3", Operation3, vType );

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "LoadScene", LoadScene, vType );

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "SaveScene", SaveScene, vType );

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "LoadEntity", LoadEntity, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "cls", cls, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "exit", Exit, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "DisplayEntityPosition", DisplayEntityPosition, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "SetEntityPos", SetEntityPos, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "DisplayCamPos", DisplayCamPos, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "SetBkgColor", SetBkgColor, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "DisplayBkgColor", DisplayBkgColor, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "DisplayBBox", DisplayBBox, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "DisplayEntities", DisplayEntities, vType );
	
	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "GetEntityID", GetEntityID, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "SetEntityShader", SetEntityShader, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "YawEntity", YawEntity, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "PitchEntity", PitchEntity, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "RollEntity", RollEntity, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	vType.push_back( eInt );
	vType.push_back( eString );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "AddLight", AddLight, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "SetLightIntensity", SetLightIntensity, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "DisplayBBoxInfos", DisplayBBoxInfos, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "ScreenCapture", ScreenCapture, vType );

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "CreateHM", CreateHM, vType );

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "LoadHM", LoadHM, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "SetEntityWeight", SetEntityWeight, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "ClearScene", ClearScene, vType );

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "SetSceneMap", SetSceneMap, vType );

	vType.clear();
	vType.push_back(eString);
	vType.push_back(eString);
	m_pScriptManager->RegisterFunction( "ExportBMEToAscii", ExportBMEToAscii, vType );

	vType.clear();
	vType.push_back( eString );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "ExportBKEToAscii", ExportBKEToAscii, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "DisplayRepere", DisplayRepere, vType );

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "LoadImage", LoadImage, vType );

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "run", run, vType );

	vType.clear();
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "SetCamPos", SetCamPos, vType );

	vType.clear();
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "SetReperePos", SetReperePos, vType );

	vType.clear();
	vType.push_back( eString );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "AddLightw", AddLightw, vType );

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "flist", flist, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "DisplaySceneChilds", DisplaySceneChilds, vType );

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "DisplayHM", DisplayHM, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "StopDisplayHM", StopDisplayHM, vType );

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "ExtractHM", ExtractHM, vType );
	
	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "SetHMPrecision", SetHMPrecision, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "DisplayShaderName", DisplayShaderName, vType );	

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "LoadShader", LoadShader, vType );
	
	//vType.clear();
	//m_pScriptManager->RegisterFunction( "ClearRessources", ClearRessources, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "SetAnimation", SetAnimation, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "PlayCurrentAnimation", PlayCurrentAnimation, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "reset", reset, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "DisplayEntitySkeletonInfos", DisplayEntitySkeletonInfos, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "SelectBone", SelectBone, vType );

	vType.clear();
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "Yaw", Yaw, vType );

	vType.clear();
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "Pitch", Pitch, vType );

	vType.clear();
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "Roll", Roll, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "PauseAnimation", PauseAnimation, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "DetachAnimation", DetachAnimation, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "StopAnimation", StopAnimation, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "Sleep", Sleep, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "HideEntity", HideEntity, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	vType.push_back( eInt );
	vType.push_back( eInt );
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "Link", Link, vType );

	vType.clear();
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "SetZCollisionError", SetZCollisionError, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "SetConstantLocalTranslate", SetConstantLocalTranslate, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "NextAnimationFrame", NextAnimationFrame, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "NextAnimationKey", NextAnimationKey, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "SetAnimationTime", SetAnimationTime, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "DisplayAnimationTime", DisplayAnimationTime, vType );	

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "CreateMobileEntity", CreateMobileEntity, vType );

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "CreateNPC", CreateNPC, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "Walk", Walk, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "Stand", Stand, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "Run", Run, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "DisplayNodeInfos", DisplayNodeInfos, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "StopRender", StopRender, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eString );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "SetAnimationSpeed", SetAnimationSpeed, vType );

	vType.clear();
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "SetGravity", SetGravity, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "SetCurrentPerso", SetCurrentPerso, vType );

	vType.clear();
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "SetCameraType", SetCameraType, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "LocalTranslate", LocalTranslate, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eString );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "RunAction", RunAction, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "SetScale", SetScale, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "SetRenderType", SetRenderType, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "DisplayBoundingSphere", DisplayBoundingSphere, vType );

	vType.clear();
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "Unlink", Unlink, vType );

	vType.clear();
	vType.push_back( eString );
	vType.push_back( eString );
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "ComputeKeysBoundingBoxes", ComputeKeysBoundingBoxes, vType );

	vType.clear();
	vType.push_back( eString );
	vType.push_back( eString );
	vType.push_back( eString );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "SetPreferedKeyBBox", SetPreferedKeyBBox, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "Test", Test, vType );

	vType.clear();
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "CreateBox", CreateBox, vType );


	vType.clear();
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "CreateSphere", CreateSphere, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "ChangeBase", ChangeBase, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eInt );
	m_pScriptManager->RegisterFunction( "DisplayAnimationBBox", DisplayAnimationBBox, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "Goto", Goto, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "CreateRepere", CreateRepere, vType );

	vType.clear();
	vType.push_back( eInt );
	vType.push_back( eString );
	m_pScriptManager->RegisterFunction( "SetEntityName", SetEntityName, vType );

	vType.clear();
	m_pScriptManager->RegisterFunction( "DisplayFov", DisplayFov, vType );

	vType.clear();
	vType.push_back( eFloat );
	m_pScriptManager->RegisterFunction( "SetFov", SetFov, vType );

	vType.clear();
	vType.push_back(eString);
	m_pScriptManager->RegisterFunction("print", print, vType);
}