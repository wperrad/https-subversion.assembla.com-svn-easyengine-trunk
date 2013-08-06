#ifndef ILOADER_H
#define ILOADER_H



#include <string>
#include <vector>
#include "EEPlugin.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
#include "Exception.h"

class IFileSystem;
class CChunk;
class CNode;
class ITexture;
class IBox;
class IGeometryManager;

using namespace std;



class ILoader
{
protected:
	int		m_nAsciiExportPrecision;
public:

	enum TPixelFormat
	{
		eRGB = 0,
		eRGBA,
		eBGR,
		eBGRA
	};

	struct IRessourceInfos
	{
		virtual ~IRessourceInfos() = 0{}
		string	m_sFileName;
		string	m_sName;
		string	m_sFileVersion;
	};

	struct CMaterialInfos : public IRessourceInfos
	{
		//std::string						m_sMaterialName;
		int								m_nID;
		std::string						m_sDiffuseMapName;
		std::vector< float >			m_vAmbient;
		std::vector< float >			m_vDiffuse;	
		std::vector< float >			m_vSpecular;
		std::vector< float >			m_vEmissive;
		float							m_fShininess;
		std::vector< CMaterialInfos >	m_vSubMaterials;
		string							m_sShaderName;
		bool							m_bExists;
		CMaterialInfos():m_nID(-1), m_fShininess( 0 ), m_bExists(false){}
		~CMaterialInfos(){}
	};

	//typedef map< int, pair< string, vector< float > > > TSkeletonMap;
	typedef map< int, pair< string, CMatrix > > TSkeletonMap;

	struct CMeshInfos : public IRessourceInfos
	{
		vector< float >									m_vVertex;
		vector< unsigned int >							m_vIndex;
		vector< float >									m_vNormalVertex;
		vector< float >									m_vNormalFace;
		vector< float >									m_vUVVertex;
		vector< unsigned int >							m_vUVIndex;
		vector< float >									m_vWeightVertex;
		vector< float >									m_vWeigtedVertexID;
		bool											m_bCanBeIndexed;
		ILoader::CMaterialInfos							m_oMaterialInfos;
		vector< unsigned short >						m_vFaceMaterialID;
		bool											m_bMultiMaterial;
		IBox*											m_pBoundingBox;
		string											m_sShaderName;
		int												m_nParentBoneID;
		//CMatrix											m_oOrgMaxMatrix;
		CVector											m_oOrgMaxPosition;
		map< string, map< int, IBox* > >				m_oKeyBoundingBoxes;
		map< string, int >								m_oPreferedKeyBBox;
		CMeshInfos() : 
		m_bCanBeIndexed( true ), m_nParentBoneID( -1 ), m_bMultiMaterial( false ){}
	};

	struct CAnimatableMeshData : public IRessourceInfos
	{
		map< int, IBox* >			m_mBonesBoundingBoxes;
		TSkeletonMap				m_mBones; // associe à chaque identifiant d'un bone une pair (nom , matrice )
		map< int, int >				m_mHierarchyBones;
		vector< CMeshInfos >		m_vMeshes;
		bool						m_bMultiMaterialActivated;
	};

	struct CTextureInfos : public IRessourceInfos
	{
		vector< unsigned char >			m_vTexels;
		int								m_nWidth;
		int								m_nHeight;
		TPixelFormat					m_ePixelFormat;
		bool							m_bFlip;
		CTextureInfos() : m_bFlip( false ) {}
	};

	struct CAnimationInfos : public IRessourceInfos
	{
		vector< unsigned int >		m_vBonesIDArray;
		vector< unsigned int >		m_vKeyCountArray;
		vector< unsigned int >		m_vTimeValueArray;
		vector< int >				m_vKeyTypeArray;
		vector< CMatrix >			m_vLocalTMArray;
		vector< CMatrix >			m_vWorldTMArray;
		vector< float >				m_vAngleArray;
		vector< float >				m_vAxisArray;
		vector< CQuaternion >		m_vLocalQuatArray;
		int							m_nBoneCount;
		int							m_nStartTime;
		int							m_nEndTime;
	};

	struct CLightInfos : public IRessourceInfos
	{
		enum TLight
		{
			eDirectionnelle,
			eOmni,
			eTarget
		};
		TLight		m_eLightType;
		CVector		m_vPosition;
		CVector		m_oColor;
		float		m_fIntensity;
	};

	struct CSceneObjInfos
	{
		CSceneObjInfos() : m_nParentBoneID(-1){}
		CMatrix		m_oXForm;
		string		m_sRessourceFileName;
		string		m_sRessourceName;
		string		m_sObjectName;
		string		m_sParentName;
		int			m_nParentBoneID;
		virtual		~CSceneObjInfos() = 0{}
	};

	struct CEntityInfos : public CSceneObjInfos
	{
		string						m_sAnimationFileName;
		float						m_fWeight;
		string						m_sTypeName;
		vector< CEntityInfos* >		m_vSubEntityInfos;
	};

	struct CLightEntityInfos : public CSceneObjInfos
	{
		CLightInfos::TLight		m_eType;
		CVector					m_oColor;
		float					m_fIntensity;
	};

	struct CSceneInfos : public IRessourceInfos
	{
		vector< CSceneObjInfos* >	m_vObject;
		string						m_sSceneFileName;
		string						m_sPersoName;
	};

	enum TObjScene
	{
		eEntity = 0,
		eLight
	};

	protected:
		int					m_nFileOffset;
		int					m_nNumArray;
		static char*		s_RessourceDir;
		void				Read( void* pDestBuffer, unsigned int nElementSize, unsigned int nElementCount, FILE* pFile );
		int					ReadInt( FILE* pFile );

	public:	
		ILoader() : m_nFileOffset( 0 ), m_nAsciiExportPrecision( 3 ){}
		virtual void		Load( string sFileName, ILoader::IRessourceInfos& ri, IFileSystem& ) = 0;
		virtual void		Export( string sFileName, const ILoader::IRessourceInfos& ri ) = 0;
		virtual void		SetAsciiExportPrecision( int nPrecision );
};

class ILoaderManager : public CPlugin
{
protected:
	ILoaderManager( const Desc& oDesc ) : CPlugin( oDesc.m_pParent, oDesc.m_sName ){}

public:
	class CBadExtension : public CEException
	{
	public:
		CBadExtension( string& sMessage ) : CEException( sMessage ) {}
	};
	struct Desc : public CPlugin::Desc
	{
		IFileSystem&		m_oFileSystem;
		IGeometryManager&	m_oGeometryManager;
		Desc( IFileSystem& oFileSystem, IGeometryManager& oGeometryManager ) : 
			CPlugin::Desc( NULL, "" ), 
			m_oFileSystem( oFileSystem ),
			m_oGeometryManager( oGeometryManager ){}
	};

	virtual ILoader*		GetLoader( std::string sExtension ) = 0;
	virtual void			LoadTexture( string sFileName, ILoader::CTextureInfos& ti ) = 0;
	virtual void			Load( string sFileName, ILoader::IRessourceInfos& ri ) = 0;
	//virtual void			ExportBME( string sFileName, const ILoader::CMeshInfos& mi ) = 0;
	virtual void			Export( string sFileName, const ILoader::IRessourceInfos& ri ) = 0;
	virtual void			CreateBMPFromData( const vector< unsigned char >& vData, int nWidth, int nHeight, int nBitPerPixel, string sFileName ) = 0;
};

#endif