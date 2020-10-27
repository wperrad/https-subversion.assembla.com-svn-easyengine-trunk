#include "SoftRenderer.h"
#include "MeshBuffer.h"
#include "Utils2/DebugTool.h"
#include "Utils2/RenderUtils.h"

// gl
#include <gl/gl.h>

// slt
#include <iomanip>
#include <sstream>

using namespace std;

CSoftRenderer::CSoftRenderer( const Desc& oDesc ):
CRenderer( oDesc ),
m_bUseTransposeMatrix( false ),
m_nCurrentProgram( T_SKINNING_PROGRAM ),
m_nBindedVertexBuffer( -1 ),
m_fRot( 0 ),
m_fRotSpeed( 0 ),
m_pMatBones( NULL )
{
	//m_mShader.clear();
	LoadShaderDirectory( oDesc.m_sShaderDirectory + "\\Software", m_oFileSystem );
	m_bUseTransposeMatrix = false;
	int nVarID = 1;
	m_mProgramID[ T_PER_PIXEL_LIGHTING_PROGRAM ] = VertexShaderPerPixelLighting;	

	nVarID = 1;
	m_mProgramID[ T_SKINNING_PROGRAM ] = VertexShaderSkinning;
	m_mAttributeID[ T_SKINNING_PROGRAM ][ "vVertexWeight" ] = nVarID++;
	m_mAttributeID[ T_SKINNING_PROGRAM ][ "vWeightedVertexID" ] = nVarID++;
	int nUniformID = 1;
	m_mUniformID[ T_SKINNING_PROGRAM ][ "matBones" ] = nUniformID++;
	m_mUniformID[ T_SKINNING_PROGRAM ][ "vSkeleton" ] = nUniformID++;
	m_mUniformID[ T_SKINNING_PROGRAM ][ "vOrgSkeleton" ] = nUniformID++;
}

IBuffer* CSoftRenderer::CreateGeometry(	const vector< float >&	vVertexArray, const vector< unsigned int >& vIndexArray, 
								const vector< float >& vUVVertexArray, const vector< unsigned int >& vUVIndexArray, 
								const vector< float >& vNormalFaceArray, const vector< float >& vNormalVertexArray )
{

	vector< float > vNewVertexArray;
	CRenderUtils::CreateNonIndexedVertexArray( vIndexArray, vVertexArray, 3, vNewVertexArray );
	int nBufferID = CreateBuffer( static_cast< int >( vNewVertexArray.size() ) );
	FillBuffer( vNewVertexArray, nBufferID );
	CGeometryBuffer* pBuffer = new CGeometryBuffer( ( unsigned int )vIndexArray.size(), ( unsigned int )vUVIndexArray.size(), nBufferID );
	return pBuffer;
}

IBuffer* CSoftRenderer::CreateIndexedGeometry( const vector< float >& vVertexArray, const vector< unsigned int >& vIndexArray, const vector< float >& vUVVertexArray, const vector< float >& vOldNormalVertexArray )
{
	CIndexedGeometryBuffer* pBuffer = new CIndexedGeometryBuffer;
	pBuffer->m_nVertexBufferID = CreateBuffer( static_cast< unsigned int >( vVertexArray.size() ) );
	FillBuffer( vVertexArray, pBuffer->m_nVertexBufferID );
	pBuffer->m_nVertexBufferSize = static_cast< unsigned int >( vVertexArray.size() );
	pBuffer->m_nIndexBufferID = CreateIndexBuffer( vIndexArray );
	pBuffer->m_nIndexBufferSize = static_cast< unsigned int >( vIndexArray.size() );
	return pBuffer;
}

int CSoftRenderer::CreateIndexBuffer( const std::vector< unsigned int >& vData )
{
	m_vIndex.resize( m_vIndex.size() + 1 );
	m_vIndex.back().resize( vData.size() );
	memcpy( &m_vIndex.back()[ 0 ], &vData[ 0 ], vData.size() * sizeof( unsigned int ) );
	return static_cast< int >( m_vIndex.size() - 1 );
}

void CSoftRenderer::DrawGeometry( const IBuffer* pBuffer )
{
	const CGeometryBuffer* pGeomBuffer = static_cast< const CGeometryBuffer* >( pBuffer );
	DrawArray( pGeomBuffer->GetID() );
}

void CSoftRenderer::DrawIndexedGeometry( const IBuffer* pBuffer, TDrawStyle style )
{
	const CIndexedGeometryBuffer* pGeomBuffer = static_cast< const CIndexedGeometryBuffer* >( pBuffer );
	int nIndexBufferID = pGeomBuffer->m_nIndexBufferID, nVertexBufferID = pGeomBuffer->m_nVertexBufferID;
	CVertexShaderInput oInput;
	CVertexShaderOutput oOutput;
	vector< CVector > vPosition, vColor;
	vector< float >& vVertex = m_vVertex[ nVertexBufferID ];
	vector< int >& vIndex = m_vIndex[ nIndexBufferID ];
	for ( unsigned i = 0; i < vVertex.size() / 3; i++ )
	{
		oInput.m_vVertex.Fill( vVertex[ i * 3 ], vVertex[ i * 3 + 1 ], vVertex[ i * 3 + 2 ], 1.f );
		m_mProgramID[ m_nCurrentProgram ]( *this, oInput, oOutput, i );
		vPosition.push_back( oOutput.m_oPosition );
		vColor.push_back( oOutput.m_oColor );
	}
	//style = T_POINTS;
	glBegin( m_mDrawStyle[ style ] );
	for ( unsigned int i = 0; i < vIndex.size(); i++ )
	{
		int iIndex = vIndex[ i ];
		float r = vColor[ iIndex ].m_x, g = vColor[ iIndex ].m_y, b = vColor[ iIndex ].m_z, a = vColor[ iIndex ].m_w;
		glColor4f( r, g, b, a );
		glVertex4f( vPosition[ iIndex ].m_x, vPosition[ iIndex ].m_y, vPosition[ iIndex ].m_z, vPosition[ iIndex ].m_w );
	}
	glEnd();
}

void CSoftRenderer::SetModelViewMatrix( const CMatrix& m )
{
	if ( m_bUseTransposeMatrix )
		m_oModelViewMatrix = m.GetTranspose();
	else
		m_oModelViewMatrix = m;
}


void CSoftRenderer::SetProjectionMatrix( const CMatrix& oMatrix )
{
	if ( m_bUseTransposeMatrix )
		m_oProjectionMatrix = oMatrix.GetTranspose();
	else
		m_oProjectionMatrix = oMatrix;
}

int CSoftRenderer::CreateBuffer( int nElementCount )
{
	m_vVertex.resize( m_vVertex.size() + 1 );
	int nBufferID = static_cast< int >( m_vVertex.size() - 1 );
	return nBufferID;
}

void CSoftRenderer::FillBuffer( const std::vector< float >& vData, int nBufferID, int nOffset )
{
	m_vVertex[ nBufferID ].resize( vData.size() );
	memcpy( &m_vVertex[ nBufferID ][ 0 ], &vData[ 0 ], vData.size() * sizeof( float ) );
}

void CSoftRenderer::DrawArray( int nBufferID )
{
	CVertexShaderInput oInput;
	vector< CVector > vPosition, vColor;

	if ( m_bUseTransposeMatrix )
	{
		m_oProjectionMatrix = m_oProjectionMatrix.GetTranspose();
		m_oModelViewMatrix = m_oModelViewMatrix.GetTranspose();
	}

	for ( unsigned int i = 0; i < m_vVertex[ nBufferID ].size() / 3;  i++ )
	{
		oInput.m_vVertex = CVector( m_vVertex[ nBufferID ][ 3 * i ], m_vVertex[ nBufferID ][ 3 * i + 1 ], m_vVertex[ nBufferID ][ 3 * i + 2  ] );
		CVertexShaderOutput oOutput;
		m_mProgramID[ m_nCurrentProgram ]( *this, oInput, oOutput, i );
		vPosition.push_back( oOutput.m_oPosition );
		vColor.push_back( oOutput.m_oColor );
	}

	glBegin( GL_TRIANGLES );
	for ( unsigned int i = 0; i < vPosition.size(); i++ )
	{
		float r = vColor[ i ].m_x, g = vColor[ i ].m_y, b = vColor[ i ].m_z, a = vColor[ i ].m_w;
		glColor4f( r, g, b, a );
		glVertex4f( vPosition[ i ].m_x, vPosition[ i ].m_y, vPosition[ i ].m_z, vPosition[ i ].m_w );		
	}
	glEnd();

	if ( m_bUseTransposeMatrix )
	{
		m_oProjectionMatrix = m_oProjectionMatrix.GetTranspose();
		m_oModelViewMatrix = m_oModelViewMatrix.GetTranspose();
	}
}

void CSoftRenderer::VertexShaderPerPixelLighting( CSoftRenderer& oRenderer, const CVertexShaderInput& oInput, CVertexShaderOutput& oOutput, int iIndex )
{
	const CMatrix& gl_ModelViewMatrix = oRenderer.m_oModelViewMatrix;
	const CVector& gl_Vertex = oInput.m_vVertex;
	const CMatrix& gl_ProjectionMatrix = oRenderer.m_oProjectionMatrix;
	CVector& gl_Position = oOutput.m_oPosition;
	const CVector& gl_Color = oInput.m_oColor;
	CVector& gl_FrontColor = oOutput.m_oColor;

	// void main()
	gl_FrontColor = gl_Color;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}

//float GetCaped( float fNumber, float cap )
//{
//	if( fNumber > cap || fNumber < -cap )
//		return fNumber;
//	return 0.f;
//}

//void SerializeMatrix( const CMatrix& m, int nPrecision, int nWidth, float fMin, string& sOut )
//{
//	ostringstream oss;
//	oss << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_00, fMin ) << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_01, fMin ) << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_02, fMin ) << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_03, fMin ) << "\n";
//	oss << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_10, fMin ) << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_11, fMin ) << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_12, fMin ) << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_13, fMin ) << "\n";
//	oss << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_20, fMin ) << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_21, fMin ) << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_22, fMin ) << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_23, fMin ) << "\n";
//	oss << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_30, fMin ) << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_31, fMin ) << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_32, fMin ) << setw(nWidth) << setprecision(nPrecision) << GetCaped( m.m_33, fMin ) << "\n";
//	sOut = oss.str();
//}

void CSoftRenderer::VertexShaderSkinning( CSoftRenderer& oRenderer, const CVertexShaderInput& oInput, CVertexShaderOutput& oOutput, int iIndex )
{
	const CMatrix& gl_ModelViewMatrix = oRenderer.m_oModelViewMatrix;
	const CVector& gl_Vertex = oInput.m_vVertex;
	const CMatrix& gl_ProjectionMatrix = oRenderer.m_oProjectionMatrix;
	CVector& gl_Position = oOutput.m_oPosition;
	//const CVector& gl_Color = oInput.m_oColor;
	CVector& gl_FrontColor = oOutput.m_oColor;
	int matBonesID = oRenderer.GetUniformID( oRenderer.m_nCurrentProgram, "matBones" );
	int vSkeletonID = oRenderer.GetUniformID( oRenderer.m_nCurrentProgram, "vSkeleton" );
	int vOrgSkeletonID = oRenderer.GetUniformID( oRenderer.m_nCurrentProgram, "vOrgSkeleton" );
	CFrontMaterial& gl_FrontMaterial = oRenderer.m_oFrontMaterial;

	// début du shader
	attribute vec4 vVertexWeight;
	attribute vec4 vWeightedVertexID;
	vector< mat4 >& matBones = oRenderer.m_mUniformMatrix4v[ oRenderer.m_nCurrentProgram ][ matBonesID ];
	vector< mat4 >& vSkeleton = oRenderer.m_mUniformMatrix4v[ oRenderer.m_nCurrentProgram ][ vSkeletonID ];
	vector< mat4 >& vOrgSkeleton = oRenderer.m_mUniformMatrix4v[ oRenderer.m_nCurrentProgram ][ vOrgSkeletonID ];

	oRenderer.GetAttributeVertex( oRenderer.m_nCurrentProgram, "vVertexWeight", iIndex, 4, vVertexWeight );
	oRenderer.GetAttributeVertex( oRenderer.m_nCurrentProgram, "vWeightedVertexID", iIndex, 4, vWeightedVertexID );

	//void main()
	{
		mat4 matWeight = mat4(0.f);
		for ( int iBone = 0; iBone < 4; iBone++ )
		{
			float fBoneID = vWeightedVertexID[ iBone ];
			if ( fBoneID != -1. )
			{
				float fWeightedVertexValue = vVertexWeight[ iBone ];
				matWeight += matBones[ (int)fBoneID ] * fWeightedVertexValue;
			}
		}
		gl_FrontColor = gl_FrontMaterial.diffuse; //gl_Color;

		//test		
		if( oRenderer.IsDisplayDebugInfos() && iIndex == 3 )
			DisplayDebugInfos( oRenderer, matWeight, gl_Vertex );
		// fin test
		gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * matWeight * gl_Vertex;
	}
}

void CSoftRenderer::DisplayDebugInfos( CSoftRenderer& oRenderer, const CMatrix& matWeight, const CVector& gl_Vertex )
{
	vec4 vSkinnedVertex = matWeight * gl_Vertex;
	vec4 vTransformedVertex = oRenderer.m_mCurrentObjectLocalTM * vSkinnedVertex;
	string sModelName;
	oRenderer.GetDebugString( sModelName );
	map<string, int >::iterator itModel = oRenderer.m_mFrameNumberByModel.find( sModelName );
	if(  itModel == oRenderer.m_mFrameNumberByModel.end() )
	{
		oRenderer.m_mFrameNumberByModel.insert( map<string,int>::value_type( sModelName, 0 ) );
	}
	if( oRenderer.m_mFrameNumberByModel[ sModelName ] % 10 == 0 )
	{
		string sEntityName;
		oRenderer.GetDebugString( sEntityName );
		ostringstream oss;
		oss << sEntityName << "_debugInfos.txt";
		FILE* pFile = NULL;
		fopen_s(&pFile, oss.str().c_str(), "a" );
		oss.str( "" );
		oss << "Model " << sModelName << "\n";
		oss << "Frame " << oRenderer.m_mFrameNumberByModel[ sModelName ] << "\n";
		int nPrecision = 5;
		oss << "vSkinnedVertex = ( " << setprecision(nPrecision) << vSkinnedVertex.m_x << ", " << setprecision(nPrecision) << vSkinnedVertex.m_y << " , " << setprecision(nPrecision) << vSkinnedVertex.m_z << "\n";
		oss << "vTransformedVertex = ( " << setprecision(nPrecision) << vTransformedVertex.m_x << ", " << setprecision(nPrecision) << vTransformedVertex.m_y << " , " << setprecision(nPrecision) << vTransformedVertex.m_z << "\n\n";
		

		string s;
		CDebugTool dt;
		dt.SetNumberPrecision( nPrecision );
		dt.SetNumberWidth( 15 );
		dt.SerializeMatrix( matWeight, 0.001f, s );
		//SerializeMatrix( matWeight, nPrecision, 15, 0.001f, s );
		oss << "\nmatWeight : \n\n" << s;
		//SerializeMatrix( oRenderer.m_mCurrentObjectLocalTM, nPrecision, 15, 0.001f, s );
		dt.SerializeMatrix( oRenderer.m_mCurrentObjectLocalTM, 0.001f, s );
		oss << "\n LocalTM : \n\n" << s;
		CMatrix m = matWeight * oRenderer.m_mCurrentObjectLocalTM;
		//SerializeMatrix( m, nPrecision, 15, 0.001f, s );
		dt.SerializeMatrix( m, 0.001f, s );
		oss << "\n Produit des deux matrices : \n\n" << s << "\n\n\n";
		fwrite( oss.str().c_str(), sizeof(char), oss.str().size(), pFile );
		fclose( pFile );
	}
	oRenderer.m_mFrameNumberByModel[ sModelName ] ++;
}

unsigned int CSoftRenderer::GetAttributeID( unsigned int nProgramID, const std::string& sVariableName ) const
{
	map< int, map< string, int > >::const_iterator itProgram = m_mAttributeID.find( m_nCurrentProgram );
	if( itProgram != m_mAttributeID.end() )
	{
		map< string, int >::const_iterator itID = itProgram->second.find( sVariableName );
		if( itID != itProgram->second.end() )
			return itID->second;
	}
	return -1;
}

void CSoftRenderer::EnableVertexAttribArray( unsigned int nAttributeID )
{
	m_mEnabledAttribute[ m_nCurrentProgram ][ nAttributeID ] = true;
}

bool CSoftRenderer::IsAttributeEnabled( unsigned int nAttributeID ) const
{
	map< int, map< int, bool > >::const_iterator itProgram = m_mEnabledAttribute.find( m_nCurrentProgram );
	if ( itProgram != m_mEnabledAttribute.end() )
	{
		map< int, bool >::const_iterator itAttribute = itProgram->second.find( nAttributeID );
		if ( itAttribute != itProgram->second.end() )
			return itAttribute->second;
	}
	return false;
}

void CSoftRenderer::DisableVertexAttribArray( unsigned int nAttributeID )
{
	m_mEnabledAttribute[ m_nCurrentProgram ][ nAttributeID ] = false;
}

void CSoftRenderer::GetAttributeVertex( int nProgram, string sVarName, int nIndex, int nVertexSize, CVector& oVertex )
{
	int nAttributeID = m_mAttributeID[ nProgram ][ sVarName ];
	int nBufferID = m_mAttributeToBufferPointer[ nProgram ][ nAttributeID ];
	vector< float >& vVertexWeight = m_vVertex[ nBufferID ];
	oVertex.Fill( vVertexWeight[ nIndex * nVertexSize ], vVertexWeight[ nIndex * nVertexSize + 1 ], vVertexWeight[ nIndex * nVertexSize + 2 ], vVertexWeight[ nIndex * nVertexSize + 3 ] );
}

void CSoftRenderer::BindVertexBuffer( int nBufferID )
{
	m_nBindedVertexBuffer = nBufferID;
}

void CSoftRenderer::VertexAttribPointer( unsigned int nAttributeID, int nSize, TNumberType Type, int nPos )
{
	if ( IsAttributeEnabled( nAttributeID ) )
		m_mAttributeToBufferPointer[ m_nCurrentProgram ][ nAttributeID ] = m_nBindedVertexBuffer;
}

unsigned int CSoftRenderer::GetUniformID( unsigned int nProgramID, const std::string& sVariableName ) const
{
	map< int, map< string, int > >::const_iterator itProgram = m_mUniformID.find( m_nCurrentProgram );
	if ( itProgram != m_mUniformID.end() )
	{
		map< string, int >::const_iterator itUniform = itProgram->second.find( sVariableName );
		if ( itUniform != itProgram->second.end() )
			return itUniform->second;
	}
	return -1;
}

void CSoftRenderer::SendUniformMatrix4fv( unsigned int nUniformID, int nCount, bool bTranspose, vector< float >& vValues )
{
	bTranspose = false;
	//m_mUniformMatrix4v.clear();
	m_mUniformMatrix4v[ m_nCurrentProgram ][ nUniformID ].clear();
	for ( int i = 0; i < nCount; i++ )
	{
		CMatrix m;
		if ( bTranspose )
			m = CMatrix( &vValues[ i * 16 ] ).GetTranspose();
		else
			m = CMatrix( &vValues[ i * 16 ] );
		m_mUniformMatrix4v[ m_nCurrentProgram ][ nUniformID ].push_back( m );
	}
}

void CSoftRenderer::DrawBase( const CMatrix& mBase, float fSize )
{
	CVertexShaderInput oInput;
	CVertexShaderOutput oOutput;
	CVector x = mBase * CVector( fSize, 0, 0, 1 );
	CVector y = mBase * CVector( 0, fSize, 0, 1 );
	CVector z = mBase * CVector( 0, 0, fSize, 1 );
	vector< CVector > vVertex, vPosition, vColor;
	vVertex.push_back( CVector( mBase.m_03, mBase.m_13, mBase.m_23, mBase.m_33 ) );
	vVertex.push_back( CVector( x.m_x, x.m_y, x.m_z, x.m_w ) );
	vVertex.push_back( CVector( mBase.m_03, mBase.m_13, mBase.m_23, mBase.m_33) );
	vVertex.push_back( CVector( y.m_x, y.m_y, y.m_z, y.m_w ) );
	vVertex.push_back( CVector( mBase.m_03, mBase.m_13, mBase.m_23, mBase.m_33 ) );
	vVertex.push_back( CVector( z.m_x, z.m_y, z.m_z, z.m_w ) );
	vColor.push_back( CVector( 1, 0, 0 ) );
	vColor.push_back( CVector( 0, 1, 0 ) );
	vColor.push_back( CVector( 0, 0, 1 ) );
	for ( unsigned int i = 0; i < vVertex.size(); i++ )
	{
		oInput.m_vVertex = vVertex[ i ];
		VertexShaderPerPixelLighting( *this, oInput, oOutput, 0 );
		vPosition.push_back( oOutput.m_oPosition );
	}
	
	glBegin( GL_LINES );
	for ( unsigned int i = 0; i < vPosition.size(); i++ )
	{
		int nColor = i / 2;
		CVector& v = vColor[ nColor ];
		glColor4f( v.m_x, v.m_y, v.m_z, v.m_w );
		glVertex4f( vPosition[ i ].m_x, vPosition[ i ].m_y, vPosition[ i ].m_z, vPosition[ i ].m_w );		
	}
	glEnd();
}

void CSoftRenderer::SetMaterialAmbient( float* fAmbient )
{
	m_oFrontMaterial.ambient.Fill( fAmbient[ 0 ], fAmbient[ 1 ], fAmbient[ 2 ], fAmbient[ 3 ] );
}

void CSoftRenderer::SetMaterialDiffuse( float* fDiffuse )
{
	m_oFrontMaterial.diffuse.Fill( fDiffuse[ 0 ], fDiffuse[ 1 ], fDiffuse[ 2 ], fDiffuse[ 3 ] );
}

void CSoftRenderer::SetMaterialSpecular( float* fSpecular )
{
	m_oFrontMaterial.specular.Fill( fSpecular[ 0 ], fSpecular[ 1 ], fSpecular[ 2 ], fSpecular[ 3 ] );
}

void CSoftRenderer::SetMaterialEmissive( float* fEmissive )
{
	m_oFrontMaterial.emissive.Fill( fEmissive[ 0 ], fEmissive[ 1 ], fEmissive[ 2 ], fEmissive[ 3 ] );
}

void CSoftRenderer::SetMaterialShininess( float fShininess )
{
	m_oFrontMaterial.shininess = fShininess;
}
//
//void CSoftRenderer::SetDebugString( std::string s )
//{
//	IRenderer::SetDebugString( s );
//}
//
//bool CSoftRenderer::DisplayDebugInfos( bool bDisplay )
//{
//	IRenderer::DisplayDebugInfos( bDisplay );
//}

extern "C" _declspec(dllexport) IRenderer* CreateSoftRenderer( IRenderer::Desc& oDesc )
{
	return new CSoftRenderer( oDesc );
}
