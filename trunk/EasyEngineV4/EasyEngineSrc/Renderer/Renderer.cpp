#define RENDERER_CPP

// stl
#include <algorithm>
#include <sstream>

#include "Renderer.h"
#include "Exception.h"
#include <gl/gl.h>
#include "gl/glext.h"
#include <gl/gl.h >
#include <gl/glu.h>
#include "GLExtension.h"
#include "MeshBuffer.h"
#include "IWindow.h"
#include "Shader.h"
#include "IFileSystem.h"
#include "RenderUtils.h"

#pragma pack(push, 1)


using namespace std;


#define VBO_RENDER


CRenderer::CRenderer( const Desc& oDesc ) :
IRenderer( oDesc ),
m_Mode(MODE_3D),
m_hRC( 0 ),
m_oWindow( oDesc.m_oWindow ),
m_nCurrentBufferID( -1 ),
m_oFileSystem( oDesc.m_oFileSystem ),
m_nCurrentBufferOffset( 0 ),
m_eCurrentRenderType( eFill ),
m_bMustChangeFov( false ),
m_fFov( 40.f )
{
	m_mDrawStyle[ T_LINES ] = GL_LINES;
	m_mDrawStyle[ T_POINTS ] = GL_POINTS;
	m_mDrawStyle[ T_TRIANGLES ] = GL_TRIANGLES;
	m_mDrawStyle[ T_LINE_LOOP ] = GL_LINE_LOOP;
	m_mPixelFormat[ T_RGB ] = GL_RGB;
	m_mPixelFormat[ T_RGBA ] = GL_RGBA;
	m_mPixelFormat[ T_BGR ] = GL_BGR;
	m_mPixelFormat[ T_BGRA ] = GL_BGRA;
	m_mNumberType[ T_FLOAT ] = GL_FLOAT;
	m_mNumberType[ T_INT ] = GL_INT;
	m_mTextureDimension[ T_1D ] = GL_TEXTURE_1D;
	m_mTextureDimension[ T_2D ] = GL_TEXTURE_2D;
	m_mTextureDimension[ T_3D ] = GL_TEXTURE_3D;
	m_mRenderType[ eFill ] = GL_FILL;
	m_mRenderType[ eLine ] = GL_LINE;
	m_mRenderType[ ePoint ] = GL_POINT;
	CreateOGLContext( oDesc );
	InitGLExtensions();
	InitOpengl();
	m_sDefaultShader = oDesc.m_sDefaultShader;
	m_sShadersDirectory = oDesc.m_sShaderDirectory;
	LoadShaderDirectory( m_sShadersDirectory + "\\Hardware", m_oFileSystem );
	TurnOffAllLight();
}

CRenderer::~CRenderer()
{
	for( map< string, CShader* >::iterator itShader = m_mShader.begin(); itShader != m_mShader.end(); itShader++ )
		delete itShader->second;
	m_mShader.clear();
}

void CRenderer::TurnOffAllLight()
{
	for( int i = GL_LIGHT0; i <= GL_LIGHT7; i++ )
	{
		GLfloat diffuse[] = {0, 0, 0, 1};
		GLfloat specular[] = {0, 0, 0, 1};
		GLfloat ambient[] = {0, 0, 0, 1};

		SetLightAmbient( i, ambient[0], ambient[1], ambient[2], ambient[3] );
		SetLightDiffuse( i, diffuse[0], diffuse[1], diffuse[2], diffuse[3] );
		SetLightSpecular( i, specular[0], specular[1], specular[2], specular[3] );
	}
}

void CRenderer::SetRenderType( TRenderType t )
{
	glPolygonMode( GL_FRONT_AND_BACK, m_mRenderType[ t ] );
}

void CRenderer::InitGLExtensions()
{
	InitExtensions();
}

void CRenderer::InitOpengl()
{
	m_pQuadricObj = gluNewQuadric();
	m_oWindow.SetForeground();
	m_oWindow.Setfocus();
	int nWidth, nHeight;
	m_oWindow.GetDimension( nWidth, nHeight );
	glViewport( 0, 0, nWidth, nHeight );

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);

	SetBackgroundColor( 0, 0, 0 );
	glShadeModel(GL_SMOOTH);							

	glClearDepth(1.0f);									
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);								
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	

	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

void CRenderer::SetBackgroundColor( int r, int g, int b, int a )
{
	m_vBackgroundColor = CVector( (float)r / 255., (float)g / 255., (float)b / 255. );
	glClearColor( m_vBackgroundColor.m_x, m_vBackgroundColor.m_y, m_vBackgroundColor.m_z, m_vBackgroundColor.m_w );
}

void CRenderer::GetBackgroundColor( CVector& vColor )
{
	vColor = m_vBackgroundColor;
}

void CRenderer::EnableZTest( bool bEnable )
{
	if( bEnable )
		glEnable(GL_DEPTH_TEST);
	else
		glDisable( GL_DEPTH_TEST );
}

void CRenderer::CalculProjectionMatrix( CMatrix& oMatrix, float l, float r, float b, float t, float n, float f )
{
	oMatrix.SetIdentity();
	oMatrix.m_00 = 2*n / ( r - l );
	oMatrix.m_02 = ( r + l ) / ( r - l );
	oMatrix.m_11 = ( 2 * n ) / ( t - b );
	oMatrix.m_12 = ( t + b ) / ( t - b );
	oMatrix.m_22 = - ( f + n ) / ( f - n );
	oMatrix.m_23 = - 2 * f * n / ( f - n );
	oMatrix.m_32 = - 1;
	oMatrix.m_33 = 0;
	return;
}

void CRenderer::CalculProjectionMatrix( CMatrix& oMatrix, float fov )
{
	int nWidth, nHeight;
	m_oWindow.GetDimension( nWidth, nHeight );
	float fRatio = (GLfloat)nWidth / (GLfloat)nHeight;
	float fInvRatio = 1.f / fRatio;
	float fovRad = fov * 3.1415927f / 180.f;
	CalculProjectionMatrix( oMatrix, -fovRad, fovRad, -fovRad*fInvRatio, fovRad*fInvRatio, 1.f, 1000000.f );
}

void CRenderer::BeginRender()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	
	glLoadIdentity();
	for ( unsigned int i = 0; i < m_vRenderEventCallback.size(); i++ )
		m_vRenderEventCallback[ i ]( this );
	if( m_bMustChangeFov )
	{
		ApplySetFov();
		m_bMustChangeFov = false;
	}
}

void CRenderer::DestroyContext()
{
	wglMakeCurrent(NULL,NULL);
	
	wglDeleteContext(m_hRC);
	m_hRC=NULL;

	ReleaseDC( m_oWindow.GetHandle() ,m_hDC);
	m_hDC=NULL;
	m_oWindow.Close();
}

void CRenderer::SetFov( float fov )
{
	m_bMustChangeFov = true;
	m_fFov = fov;
}

void CRenderer::ApplySetFov()
{
	CalculProjectionMatrix( m_oProjectionMatrix, m_fFov );	
	SetProjectionMatrix( m_oProjectionMatrix );	
}

void CRenderer::GetProjectionMatrix( CMatrix& oMatrix )
{
	oMatrix = m_oProjectionMatrix;
}

void CRenderer::ResizeScreen( int nWidth, int nHeight )
{
	glViewport( 0, 0, nWidth, nHeight );
}

void CRenderer::CreateOGLContext( const Desc& oDesc )
{
	HGLRC hRC = wglGetCurrentContext();
	if ( !hRC )
	{
		GLuint		PixelFormat;
		int nBits = m_oWindow.GetBits();
		if ( nBits < 0 )
			nBits = 32;
		static	PIXELFORMATDESCRIPTOR pfd=				
		{
			sizeof(PIXELFORMATDESCRIPTOR),				
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,							
			PFD_TYPE_RGBA,								
			nBits,										
			0, 0, 0, 0, 0, 0,							
			0,											
			0,											
			0,											
			0, 0, 0, 0,									
			16,											
			0,											
			0,											
			PFD_MAIN_PLANE,								
			0,											
			0, 0, 0										
		};
		
		if ( !( m_hDC = GetDC( m_oWindow.GetHandle() ) ) )
		{
			m_oWindow.Close();
			exception e( "Impossible de cr�er un contexte opengl" );
			throw e;
		}

		PixelFormat = ChoosePixelFormat( m_hDC, &pfd );
		SetPixelFormat( m_hDC, PixelFormat, &pfd );
		m_hRC = wglCreateContext( m_hDC );
		wglMakeCurrent( m_hDC, m_hRC );
	}
}

void CRenderer::EndRender()
{
	SwapBuffers(m_hDC);
}

IBuffer* CRenderer::CreateGeometry( const vector< float >&	vVertexArray, const vector< unsigned int >& vIndexArray, 
									const vector< float >& vUVVertexArray, const vector< unsigned int >& vUVIndexArray, 
									const vector< float >& vNormalFaceArray, const vector< float >& vNormalVertexArray )
{
	vector< float > vNewVertexArray;
	CRenderUtils::CreateNonIndexedVertexArray( vIndexArray, vVertexArray, 3, vNewVertexArray );
	int nVertexBufferSize = ( int ) ( vNewVertexArray.size() * sizeof( float ) );

	vector< float > vNonIndexedNormal;
	CRenderUtils::CreateNonIndexedVertexArray( vIndexArray, vNormalVertexArray, 3, vNonIndexedNormal );

	vector< float > vNewUVVertexArray;
	CRenderUtils::CreateNonIndexedVertexArray( vUVIndexArray, vUVVertexArray, 2, vNewUVVertexArray );

	int nUVVertexBufferSize = (int)( vNewUVVertexArray.size() * sizeof( float ) );
	
	glEnableClientState ( GL_VERTEX_ARRAY );	
    glEnableClientState ( GL_NORMAL_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    unsigned int nVertexBufferID;
	glGenBuffers( 1, &nVertexBufferID );
	glBindBuffer( GL_ARRAY_BUFFER_ARB, nVertexBufferID );
	glBufferData( GL_ARRAY_BUFFER_ARB, nVertexBufferSize*2 + nUVVertexBufferSize , NULL , GL_STATIC_DRAW_ARB );
	glBufferSubData( GL_ARRAY_BUFFER_ARB, 0, nVertexBufferSize, &vNewVertexArray[ 0 ] );
	glBufferSubData( GL_ARRAY_BUFFER_ARB, nVertexBufferSize, nVertexBufferSize, &vNonIndexedNormal[ 0 ] );
	if ( vNewUVVertexArray.size() > 0 )
		glBufferSubData( GL_ARRAY_BUFFER_ARB, nVertexBufferSize * 2, nUVVertexBufferSize, &vNewUVVertexArray[ 0 ] );

	glDisableClientState ( GL_VERTEX_ARRAY );
	glDisableClientState ( GL_NORMAL_ARRAY );
	glDisableClientState ( GL_TEXTURE_COORD_ARRAY );

	CGeometryBuffer* pBuffer = new CGeometryBuffer( ( unsigned int )vIndexArray.size(), ( unsigned int )vUVIndexArray.size(), nVertexBufferID );
    return pBuffer;
}

void CRenderer::GenUVVertexArray( vector< unsigned int >& vIndexArray, vector< float >& vVertexArray, const vector< unsigned int >& vUVIndexArray, const vector< float >& vUVVertexArray, vector< float >& vNewUVVertexArray )
{
	vector< float > nNonIndexedUVVertexArray;
	CRenderUtils::CreateNonIndexedVertexArray( vUVIndexArray, vUVVertexArray, 2, nNonIndexedUVVertexArray );
	map< int, CVector > mUVVertexArray;
	for( int i = 0; i < vIndexArray.size(); i++ )
	{
		int nIndex = vIndexArray[ i ];
		int nUVIndex = vUVIndexArray[ i ];
		CVector oUVVertex = CVector( vUVVertexArray[ 2 * nUVIndex ], vUVVertexArray[ 2 * nUVIndex + 1 ], 0 );
		map< int, CVector >::iterator itUVIndex = mUVVertexArray.find( nIndex );
		if( itUVIndex == mUVVertexArray.end() )
			mUVVertexArray[ nIndex ] = oUVVertex;
		else
		{
			CVector& vTemp = mUVVertexArray[ nIndex ];
			if( vTemp != oUVVertex )
			{
				int nNewVertexIndex = vVertexArray.size() / 3;
				for( int j = 0; j < 3; j++ )
					vVertexArray.push_back( vVertexArray[ 3 * nIndex + j ] );
				vIndexArray[ i ] = nNewVertexIndex;
				mUVVertexArray[ nNewVertexIndex ] = oUVVertex;
			}
		}
	}
	for( map< int, CVector >::iterator itVec = mUVVertexArray.begin(); itVec != mUVVertexArray.end(); itVec++ )
	{
		vNewUVVertexArray.push_back( itVec->second.m_x );
		vNewUVVertexArray.push_back( itVec->second.m_y );
	}
}

IBuffer* CRenderer::CreateIndexedGeometry( const vector< float >&	vVertexArray, const vector< unsigned int >& vIndexArray, 
										const vector< float >& vUVVertexArray, const vector< unsigned int >& vUVIndexArray, 
										const vector< float >& vNormalVertexArray )
{
	//// temporaire
	//vector< float > vNormalVertexArray;
	//vNormalVertexArray.resize( vVertexArray.size() );
	//for ( unsigned int i = 0; i < vIndexArray.size(); i++ )
	//{
	//	int nIndex = vIndexArray[ i ];
	//	for ( int j = 0; j < 3; j++ )
	//		vNormalVertexArray[ 3 * nIndex + j ] = vOldNormalVertexArray[ 3 * i + j ];
	//}
	//// fin temporaire

	CIndexedGeometryBuffer* pBuffer = new CIndexedGeometryBuffer;
	pBuffer->m_nVertexBufferSize = ( int ) ( vVertexArray.size() * sizeof( float ) );

	if ( vUVVertexArray.size() > 0 )
	{
		pBuffer->m_nUVVertexBufferSize = ( int )( vUVVertexArray.size() * sizeof( float ) );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glGenBuffers( 1, &pBuffer->m_nUVVertexBufferID );
		glBindBuffer( GL_ARRAY_BUFFER, pBuffer->m_nUVVertexBufferID );
		glBufferData( GL_ARRAY_BUFFER, pBuffer->m_nUVVertexBufferSize, &vUVVertexArray[ 0 ], GL_STATIC_DRAW );
	}
	
	glEnableClientState( GL_VERTEX_ARRAY );
	glGenBuffers( 1, &pBuffer->m_nVertexBufferID );
	glBindBuffer( GL_ARRAY_BUFFER, pBuffer->m_nVertexBufferID );
	glBufferData( GL_ARRAY_BUFFER, pBuffer->m_nVertexBufferSize, &vVertexArray[ 0 ] , GL_STATIC_DRAW );		

	glEnableClientState( GL_NORMAL_ARRAY );
	glGenBuffers( 1, &pBuffer->m_nNormalBufferID );
	glBindBuffer( GL_ARRAY_BUFFER, pBuffer->m_nNormalBufferID );
	glBufferData( GL_ARRAY_BUFFER, pBuffer->m_nVertexBufferSize, &vNormalVertexArray[ 0 ], GL_STATIC_DRAW );

	pBuffer->m_nIndexBufferSize = ( int ) ( vIndexArray.size() * sizeof( int ) );
	pBuffer->m_nIndexCount = ( unsigned int )vIndexArray.size();
	
	glGenBuffers( 1, &pBuffer->m_nIndexBufferID );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pBuffer->m_nIndexBufferID );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, pBuffer->m_nIndexBufferSize, &vIndexArray[ 0 ] , GL_STATIC_DRAW );

	glDisableClientState( GL_VERTEX_ARRAY );
	if ( vUVVertexArray.size() > 0 )
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
    return pBuffer;
}


void CRenderer::SetMaterialAmbient( float* fAmbient )
{
	glMaterialfv( GL_FRONT , GL_AMBIENT, fAmbient );
}

void CRenderer::SetMaterialDiffuse( float* fDiffuse )
{
	glMaterialfv( GL_FRONT , GL_DIFFUSE , fDiffuse );
}

void CRenderer::SetMaterialSpecular( float* fSpecular )
{
	glMaterialfv( GL_FRONT , GL_SPECULAR , fSpecular );
}

void CRenderer::SetMaterialEmissive( float* fEmissive )
{
	glMaterialfv( GL_FRONT , GL_EMISSION , fEmissive );
}

void CRenderer::SetMaterialShininess( float fShininess )
{
	glMaterialf( GL_FRONT , GL_SHININESS , fShininess );
}

void CRenderer::BindTexture( int nTextureID, int nUnitTextureID, TTextureDimension texDim )
{
	glActiveTexture( GL_TEXTURE0 + nUnitTextureID );
	glBindTexture( m_mTextureDimension[ texDim ], nTextureID );
}

void CRenderer::DrawGeometry( const IBuffer* pBuffer )
{
	CMatrix oModelView = m_oCameraMatrixInv * m_oCurrentObjectMatrix;
	LoadMatrix( oModelView );
 	const CGeometryBuffer* pGeometryBuffer = static_cast< const CGeometryBuffer* >( pBuffer );
	if (pGeometryBuffer->GetID() == -1)
	{
		CRenderException e("Invalid MeshBuffer");
		throw e;
	}
	
	glEnableClientState (GL_VERTEX_ARRAY);
  	glEnableClientState(GL_NORMAL_ARRAY);	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	int nVertexBufferSize = pGeometryBuffer->GetIndexCount() * 3 * sizeof(float);
	int nNormalVertexBufferSize = pGeometryBuffer->GetIndexCount() * 3 * sizeof(float);
	//int nTextCoordBufferSize = pGeometryBuffer->GetUVIndexCount() * 2 * sizeof(float);

	glBindBuffer( GL_ARRAY_BUFFER_ARB, pGeometryBuffer->GetID() );
	glVertexPointer( 3, GL_FLOAT, 0, 0 );
	glNormalPointer( GL_FLOAT, 0, BUFFER_OFFSET( nVertexBufferSize ) );
	glTexCoordPointer(2,GL_FLOAT, 0,BUFFER_OFFSET( nVertexBufferSize + nNormalVertexBufferSize ) );

	glDrawArrays(GL_TRIANGLES , 0 , pGeometryBuffer->GetIndexCount());
	
  	glDisableClientState (GL_VERTEX_ARRAY);
  	glDisableClientState (GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void CRenderer::DrawIndexedGeometry( const IBuffer* pBuffer, TDrawStyle style )
{
	CMatrix oModelView = m_oCameraMatrixInv * m_oCurrentObjectMatrix;
	LoadMatrix( oModelView );
	const CIndexedGeometryBuffer* pIndexedBuffer = static_cast< const CIndexedGeometryBuffer* >( pBuffer );

 	if ( pIndexedBuffer->m_nUVVertexBufferID != 0 )
	{
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glBindBuffer( GL_ARRAY_BUFFER, pIndexedBuffer->m_nUVVertexBufferID );
		glTexCoordPointer( 2, GL_FLOAT, 0, BUFFER_OFFSET( 0 ) );
	}
	
	glEnableClientState ( GL_VERTEX_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, pIndexedBuffer->m_nVertexBufferID );
	glVertexPointer( 3, GL_FLOAT, 0, BUFFER_OFFSET( 0 ) );

	glEnableClientState( GL_NORMAL_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, pIndexedBuffer->m_nNormalBufferID );
	glNormalPointer( GL_FLOAT, 0, BUFFER_OFFSET( 0 ) );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pIndexedBuffer->m_nIndexBufferID );

	GLenum glDrawStyle = m_mDrawStyle[ style ];
	glDrawElements( glDrawStyle, pIndexedBuffer->m_nIndexCount, GL_UNSIGNED_INT, 0 );
  	glDisableClientState (GL_VERTEX_ARRAY);
	glDisableClientState( GL_NORMAL_ARRAY );
}

void CRenderer::DrawBase( const CMatrix& mBase, float fSize )
{
	CVector x = mBase * CVector( 1 * fSize, 0, 0, 1 );
	CVector y = mBase * CVector( 0, 1 * fSize, 0, 1 );
	CVector z = mBase * CVector( 0, 0, 1 * fSize, 1 );
	DrawLine( CVector( mBase.m_03, mBase.m_13, mBase.m_23 ), CVector( x.m_x, x.m_y, x.m_z ), CVector( 1, 0, 0 ) );
	DrawLine( CVector( mBase.m_03, mBase.m_13, mBase.m_23 ), CVector( y.m_x, y.m_y, y.m_z ), CVector( 0, 1, 0 ) );
	DrawLine( CVector( mBase.m_03, mBase.m_13, mBase.m_23 ), CVector( z.m_x, z.m_y, z.m_z ), CVector( 0, 0, 1 ) );
}

void CRenderer::SetModelViewMatrix( const CMatrix& m )
{
	LoadMatrix( m );
}

void CRenderer::SetProjectionMatrix( const CMatrix& oMatrix )
{
	glMatrixMode( GL_PROJECTION );
	LoadMatrix( oMatrix );
	glMatrixMode( GL_MODELVIEW );
}


void CRenderer::SetCameraMatrix( const CMatrix& oMatrix )
{
	oMatrix.GetInverse( m_oCameraMatrixInv );
}

void CRenderer::SetObjectMatrix( const CMatrix& oMatrix )
{
	m_oCurrentObjectMatrix = oMatrix;
}

unsigned int CRenderer::CreateVertexBuffer( const std::vector< float >& vData ) const
{
	unsigned int nBufferID;
	glGenBuffers( 1, &nBufferID );
	glBindBuffer( GL_ARRAY_BUFFER, nBufferID );
	glBufferData( GL_ARRAY_BUFFER, vData.size() * sizeof( float ), &vData[ 0 ], GL_STATIC_DRAW );
	return nBufferID;
}

unsigned int CRenderer::CreateVertexBuffer( const std::vector< int >& vData ) const
{
	unsigned int nBufferID;
	glGenBuffers( 1, &nBufferID );
	glBindBuffer( GL_ARRAY_BUFFER, nBufferID );
	glBufferData( GL_ARRAY_BUFFER, vData.size() * sizeof( int ), &vData[ 0 ], GL_STATIC_DRAW );
	return nBufferID;
}


int CRenderer::CreateBuffer( int nElementCount )
{
	unsigned int nBufferID;
	glGenBuffers( 1, &nBufferID );
	glBindBuffer( GL_ARRAY_BUFFER, nBufferID );
	glBufferData( GL_ARRAY_BUFFER_ARB, nElementCount * sizeof( float ), NULL , GL_STATIC_DRAW_ARB );
	m_nCurrentBufferID = nBufferID;
	return nBufferID;
}

void CRenderer::GetNonIndexedVertexBuffer( const vector< float >& vVertexBuffer, const vector< unsigned int >& vIndexBuffer, vector< float >& vOutVertexBuffer )
{
	for ( unsigned int i = 0; i < vIndexBuffer.size() ; i++ )
	{
		int nIndex = vIndexBuffer[ i ];
		vOutVertexBuffer.push_back( vVertexBuffer[ nIndex * 3 ] );
		vOutVertexBuffer.push_back( vVertexBuffer[ nIndex * 3 + 1 ] );
		vOutVertexBuffer.push_back( vVertexBuffer[ nIndex * 3 + 2 ] );
	}
}

void CRenderer::SetCurrentBuffer( int nBufferID )
{
	m_nCurrentBufferID = nBufferID;
	glBindBuffer( GL_ARRAY_BUFFER_ARB, nBufferID );
	m_nCurrentBufferOffset = 0;
}

void CRenderer::FillBuffer( const std::vector< float >& vData, int nBufferID, int nOffset )
{
	glBufferSubData( GL_ARRAY_BUFFER_ARB, nOffset, vData.size() * sizeof(float), &vData[ 0 ] );
	GLenum error = glGetError();
	if( error != 0 )
	throw 1;
	m_nCurrentBufferOffset += static_cast< int >( vData.size() ) * sizeof(float) + nOffset * sizeof(float);
}

void CRenderer::AppendBuffer( const std::vector< float >& vData )
{
	glBufferSubData( GL_ARRAY_BUFFER_ARB, m_nCurrentBufferOffset, vData.size() * sizeof(float), &vData[ 0 ] );
	m_nCurrentBufferOffset += static_cast< int >( vData.size() ) * sizeof(float);
}

void CRenderer::BindVertexBuffer( int nBufferID )
{
	glBindBuffer( GL_ARRAY_BUFFER, nBufferID );
}

void CRenderer::DeleteBuffer( unsigned int nBufferID )
{
	if( nBufferID != -1 )
		glDeleteBuffers( 1, &nBufferID );
}

void CRenderer::DeleteBuffer( IBuffer* pBuffer )
{
	CGeometryBuffer* pGBuffer = dynamic_cast< CGeometryBuffer* >( pBuffer );
	if( pGBuffer )
	{
		unsigned int nID = pGBuffer->GetID();
		glDeleteBuffers( 1, &nID );
	}
	else
	{
		CIndexedGeometryBuffer* pIGBuffer = dynamic_cast< CIndexedGeometryBuffer* >( pBuffer );
		if( pIGBuffer )
		{
			glDeleteBuffers( 1, &pIGBuffer->m_nVertexBufferID );
			glDeleteBuffers( 1, &pIGBuffer->m_nIndexBufferID );
			glDeleteBuffers( 1, &pIGBuffer->m_nNormalBufferID );
			glDeleteBuffers( 1, &pIGBuffer->m_nUVIndexBufferID );
			glDeleteBuffers( 1, &pIGBuffer->m_nUVVertexBufferID );
		}
	}
	
}

void CRenderer::LoadMatrix(const CMatrix& m)
{
	GLfloat params[16] = {	m.m_00	,	m.m_10	,	m.m_20	,	m.m_30 ,  
							m.m_01	,	m.m_11	,	m.m_21	,	m.m_31 , 
							m.m_02	,	m.m_12	,	m.m_22	,	m.m_32 , 
							m.m_03	,	m.m_13	,	m.m_23	,	m.m_33  	};	
	glLoadMatrixf(params);
}

void CRenderer::MultMatrix( const CMatrix& m )
{
	GLfloat params[16] = {	m.m_00	,	m.m_10	,	m.m_20	,	m.m_30 , 
							m.m_01	,	m.m_11	,	m.m_21	,	m.m_31 , 
							m.m_02	,	m.m_12	,	m.m_22	,	m.m_32 , 
							m.m_03	,	m.m_13	,	m.m_23	,	m.m_33  	};	
	glMultMatrixf(params);
}

void CRenderer::EnableLighting(bool bEnable)
{
	if (bEnable)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

}

int CRenderer::CreateTexture1D( float* pTexelsArray, int nSize, TPixelFormat format )
{
	unsigned int nTextureID;
	glGenTextures( 1, &nTextureID);
	glBindTexture( GL_TEXTURE_1D, nTextureID );
	GLint nInternalFormat;
	if ( format == T_RGB )
		nInternalFormat = 3;
	if (format == T_RGBA )
		nInternalFormat = 4;
	GLenum glFormat = m_mPixelFormat[ format ];
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	//glTexImage2D( GL_TEXTURE_2D, 0, nInternalFormat, (GLsizei) nWidth, (GLsizei)nHeight, 0, glFormat, GL_UNSIGNED_BYTE, pTexelsArray );	
	glTexImage1D( GL_TEXTURE_1D, 0, nInternalFormat, nSize, 0, glFormat, GL_FLOAT, pTexelsArray );
	return nTextureID;
}

int CRenderer::CreateTexture2D( vector< unsigned char>& vTexel, int nWidth, int nHeight, IRenderer::TPixelFormat format )
{	
	unsigned int nTextureID;
	glGenTextures( 1, &nTextureID);
	glBindTexture( GL_TEXTURE_2D, nTextureID );
	GLint nInternalFormat;
	if ( format == T_RGB )
		nInternalFormat = 3;
	if (format == T_RGBA )
		nInternalFormat = 4;
	if( nWidth * nHeight * nInternalFormat != vTexel.size() )
	{
		CRenderException e( "La texture que vous essayez de cr�er ne contient pas suffisamment de donn�es par rapport � sa taille et son format" );
		throw e;
	}

	GLenum glFormat = m_mPixelFormat[ format ];

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, nInternalFormat, (GLsizei) nWidth, (GLsizei)nHeight, 0, glFormat, GL_UNSIGNED_BYTE, &vTexel[ 0 ] );	

	return nTextureID;
}

void CRenderer::EnableFog()
{
	glEnable(GL_FOG);
	GLfloat fogColor [4] = {0.9f,0.9f,0.9f,1.f};
	glFogi(GL_FOG_MODE,GL_EXP2);
	glFogfv(GL_FOG_COLOR,fogColor);
	glFogf(GL_FOG_DENSITY,(GLfloat)0.0001);
	glHint(GL_FOG_HINT,GL_NICEST);
	glFogf(GL_FOG_START,1.0f);
	glFogf(GL_FOG_END,10000.f);
}


void CRenderer::Set2DMode()
{
	if (m_Mode == MODE_2D)
	{
		CRenderException e( "" );
		throw e;
	}
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	m_Mode = MODE_2D;
}

void CRenderer::Set3DMode()
{
	if (m_Mode == MODE_3D)
	{
		CRenderException e( "" );
		throw e;
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	m_Mode = MODE_3D;
}

void CRenderer::GetResolution( int& nWidth, int& nHeight )
{
	m_oWindow.GetDimension( nWidth, nHeight );
}

void CRenderer::Translate2D(float fx, float fy)
{
	glTranslatef(fx,-fy,0.f);
}

void CRenderer::EnableTextureBlend(bool bEnable)
{
	if (bEnable)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
	}
	else
		glDisable(GL_BLEND);
}


void CRenderer::SetModelViewToIdentity()
{
	glLoadIdentity();
}


void CRenderer::DisableLight(unsigned int LightID)
{
	glDisable((GLenum)LightID);
}

void CRenderer::EnableLight(unsigned int LightID)
{
	glEnable((GLenum)LightID);
}

void CRenderer::DrawLine( const CVector& p1, const CVector& p2, const CVector& color )
{
	CMatrix oModelView = m_oCameraMatrixInv * m_oCurrentObjectMatrix;
	LoadMatrix( oModelView );
	glColor3f( color.m_x, color.m_y, color.m_z );
	glBegin( GL_LINES );
	glVertex3f( p1.m_x, p1.m_y, p1.m_z );
	glVertex3f( p2.m_x, p2.m_y, p2.m_z );
	glEnd();
}

void CRenderer::DrawBox( const CVector& oMinPoint, const CVector& oDimension )
{
	CVector p0 = oMinPoint;
	CVector p1 = oMinPoint + CVector( oDimension.m_x , 0, 0 );
	CVector p2 = oMinPoint + CVector( oDimension.m_x , oDimension.m_y, 0 );
	CVector p3 = oMinPoint + CVector( 0, oDimension.m_y, 0 );

	CVector p4 = oMinPoint + CVector( 0, 0, oDimension.m_z );
	CVector p5 = oMinPoint + CVector( oDimension.m_x , 0, oDimension.m_z );
	CVector p6 = oMinPoint + CVector( oDimension.m_x , oDimension.m_y, oDimension.m_z );
	CVector p7 = oMinPoint + CVector( 0, oDimension.m_y, oDimension.m_z );
	
	CVector color( 1, 1, 1, 1 );
	DrawLine( p0, p1, color );
	DrawLine( p1, p2, color  );
	DrawLine( p2, p3, color  );
	DrawLine( p3, p0, color  );
	DrawLine( p0, p4, color  );
	DrawLine( p1, p5, color  );
	DrawLine( p2, p6, color  );
	DrawLine( p3, p7, color  );
	DrawLine( p4, p5, color  );
	DrawLine( p5, p6, color  );
	DrawLine( p6, p7, color  );
	DrawLine( p7, p4, color  );
}

void CRenderer::DrawSphere(double dRadius, unsigned int nSliceCount, unsigned int nStackCount)
{
	CMatrix oModelView = m_oCameraMatrixInv * m_oCurrentObjectMatrix;
	LoadMatrix( oModelView );
	gluSphere(m_pQuadricObj,dRadius,nSliceCount,nStackCount);	
}

void CRenderer::DrawCylinder(double dBaseRadius, double dTopRadius, double dHeight, 
							unsigned int nSlicesCount, unsigned int nStacksCount)
{
	gluCylinder(m_pQuadricObj,dBaseRadius,dTopRadius,dHeight,nSlicesCount,nStacksCount);
}


void CRenderer::SetLightAmbient(unsigned int nLightID, float r, float g, float b, float a)
{
	GLfloat val_array[] = {r,g,b,a};
	glLightfv((GLenum) nLightID, GL_AMBIENT, val_array);
}

void CRenderer::SetLightDiffuse(unsigned int nLightID, float r, float g, float b, float a)
{
	GLfloat val_array[] = {r,g,b,a};
	glLightfv((GLenum) nLightID, GL_DIFFUSE, val_array);	
}


void CRenderer::SetLightSpecular(unsigned int nLightID, float r, float g, float b, float a)
{
	GLfloat val_array[] = {r,g,b,a};
	glLightfv((GLenum) nLightID, GL_SPECULAR, val_array);
}

void CRenderer::SetLightAttenuation( unsigned int nLightID, float fConstant, float fLinear, float fQuadratic )
{
	glLightf((GLenum) nLightID , GL_CONSTANT_ATTENUATION, (GLfloat)fConstant );
	glLightf((GLenum) nLightID , GL_LINEAR_ATTENUATION, (GLfloat)fLinear );	
	glLightf((GLenum) nLightID , GL_QUADRATIC_ATTENUATION, (GLfloat)fQuadratic );
}

void CRenderer::SetLightSpotProp( unsigned int nLightID, float fCutoff, float fExponent )
{	
	glLightf((GLenum) nLightID, GL_SPOT_CUTOFF, (GLfloat)fCutoff);		
	glLightf((GLenum) nLightID, GL_SPOT_EXPONENT, (GLfloat)fExponent);
}


void CRenderer::SetLightLocalPos(unsigned int nLightID, float x, float y, float z, float w)
{
	CMatrix oModelView = m_oCameraMatrixInv * m_oCurrentObjectMatrix;
	LoadMatrix( oModelView );
	GLfloat light_position[] = {x,y,z,w};
	glLightfv((GLenum) nLightID , GL_POSITION , light_position);
}


void CRenderer::SetLightSpotDirection(unsigned int nLightID, float x, float y ,float z, float w)
{
	float spot_direction[] = {x,y,z,w};
	glLightfv((GLenum) nLightID, GL_SPOT_DIRECTION, spot_direction);
}


void CRenderer::LoadShaderDirectory( const string& sShaderDirectory, IFileSystem& oFileSystem )
{
	WIN32_FIND_DATAA data;
	HANDLE hFile;
	string sVertexShaderDir = sShaderDirectory + "\\*.vs";
	hFile = oFileSystem.FindFirstFile_EE( sVertexShaderDir, data );
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		do
		{
			string sShaderName = data.cFileName;
			CShader* pShader = new CShader( *this );
			string sShaderPath = sShaderDirectory + "\\" + data.cFileName;
			pShader->Attach( sShaderPath, IRenderer::T_VERTEX_SHADER, oFileSystem );
			string sShaderNameWithoutExt = sShaderName.substr( 0, sShaderName.size() - 3 );
			string sShaderNameWithoutExtLower = sShaderNameWithoutExt;
			transform( sShaderNameWithoutExt.begin(), sShaderNameWithoutExt.end(), sShaderNameWithoutExtLower.begin(), tolower );
			m_mShader[ sShaderNameWithoutExtLower ] = pShader;
		}
		while ( FindNextFileA( hFile, &data ) == TRUE );
	}

	sVertexShaderDir = sShaderDirectory + "\\*.ps";
	hFile = m_oFileSystem.FindFirstFile_EE( sVertexShaderDir, data );
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		do
		{
			string sShaderName = data.cFileName;
			string sShaderNameWithoutExt = sShaderName.substr( 0, sShaderName.size() - 3 );
			string sShaderNameLower = sShaderNameWithoutExt;
			transform( sShaderNameWithoutExt.begin(), sShaderNameWithoutExt.end(), sShaderNameLower.begin(), tolower );
			map< string, CShader* >::iterator itShader = m_mShader.find( sShaderNameLower );
			CShader* pShader = NULL;
			if ( itShader == m_mShader.end() )
			{
				pShader = new CShader( *this );
				m_mShader[ sShaderNameLower ] = pShader;
			}
			else
				pShader = itShader->second;
			string sShaderPath = sShaderDirectory + "\\" + data.cFileName;
			pShader->Attach( sShaderPath, IRenderer::T_PIXEL_SHADER, oFileSystem );
		}
		while ( FindNextFileA( hFile, &data ) == TRUE );
	}

	for ( map< string, CShader* >::iterator itShader = m_mShader.begin(); itShader != m_mShader.end(); ++itShader )
		itShader->second->Link();
}


IShader* CRenderer::GetShader( string sShaderName )
{
	string sLowShaderName = sShaderName;
	transform( sShaderName.begin(), sShaderName.end(), sLowShaderName.begin(), tolower );
	map< string, CShader* >::iterator itShader = m_mShader.find( sLowShaderName );
	if ( itShader != m_mShader.end() )
		return itShader->second;
	return NULL;
	//string sMessage = string( "CShaderManager::SetCurrent : Shader \"" ) + sShaderName + "\" introuvable";
	//exception e( sMessage.c_str() );
	//throw e;
}

void CRenderer::GetDefaultShader( std::string& sDefaultShader )
{
	sDefaultShader = m_sDefaultShader;
}


unsigned int CRenderer::CreateProgram() const
{
	return glCreateProgram();
}

unsigned int CRenderer::CreateShader( IRenderer::TShaderType type ) const
{
	map< IRenderer::TShaderType, GLenum > mShaderType;
	mShaderType[ T_PIXEL_SHADER ] = GL_FRAGMENT_SHADER;
	mShaderType[ T_VERTEX_SHADER ] = GL_VERTEX_SHADER;
	unsigned int nShaderID = glCreateShader( mShaderType[ type ] );
	if ( ( nShaderID == 0 ) || ( glIsShader( nShaderID ) == false ) )
	{
		exception e( "CRenderer::CreateShader() : Impossible de cr�er le shader" );
		throw e;
	}
	return nShaderID;
}

void CRenderer::DeleteShader( unsigned int nShaderID ) const
{
	glDeleteShader( nShaderID );
}

void CRenderer::DeleteProgram( unsigned int nProgramID ) const
{
	glDeleteProgram( nProgramID );
}

void CRenderer::SetShaderSource( unsigned int nShaderID, const char* pSource ) const
{
	glShaderSource( nShaderID, 1, (const GLchar**)&pSource, NULL );
}

void CRenderer::LoadShader( string sShaderName, IFileSystem& oFileSystem )
{
	CShader* pShader = NULL;
	vector< string > vShaderExt;
	vShaderExt.push_back( "vs" );
	vShaderExt.push_back( "ps" );

	for( int i = 0; i < 2; i++ )
	{
		string sShaderPath = m_sShadersDirectory + "\\" + sShaderName + "." + vShaderExt[ i ];
		WIN32_FIND_DATAA data;
		HANDLE hFile = m_oFileSystem.FindFirstFile_EE( sShaderPath, data );
		if( hFile != INVALID_HANDLE_VALUE )
		{
			if( !pShader )
				pShader = new CShader( *this );
			pShader->Attach( sShaderPath, IRenderer::T_VERTEX_SHADER, oFileSystem );
			string sShaderNameWithoutExt = sShaderName.substr( 0, sShaderName.size() - 3 );
			string sShaderNameWithoutExtLower = sShaderNameWithoutExt;
			transform( sShaderNameWithoutExt.begin(), sShaderNameWithoutExt.end(), sShaderNameWithoutExtLower.begin(), tolower );
			CShader* pOldShader = NULL;
			map< string, CShader* >::iterator itShader = m_mShader.find( sShaderNameWithoutExtLower );
			if( itShader != m_mShader.end() )
				pOldShader = itShader->second;
			delete pOldShader;
			m_mShader[ sShaderNameWithoutExtLower ] = pShader;
		}
	}
	if( !pShader )
	{
		string sMessage = string( "Shader \"" ) + sShaderName + "\" not found";
		CFileNotFoundException e( sMessage );
		e.m_sFileName = sShaderName;
		throw e;
	}

}

void CRenderer::CompileShader( unsigned int nShaderID ) const
{
	glCompileShader( nShaderID );
	GLint CompileStatus = GL_TRUE;
	glGetShaderiv( nShaderID, GL_COMPILE_STATUS, &CompileStatus );
	if ( CompileStatus != GL_TRUE )
	{
		GLsizei LogSize = 0;
		glGetShaderiv( nShaderID, GL_INFO_LOG_LENGTH, &LogSize );
		char* szLog = new char[ LogSize + 1 ];
		glGetShaderInfoLog( nShaderID, LogSize, &LogSize, szLog );
		exception e( szLog );
		throw e;
	}
}

void CRenderer::AttachShaderToProgram( unsigned int nProgramID, unsigned int nShaderID ) const
{
	glAttachShader( nProgramID, nShaderID );
}

void CRenderer::LinkProgram( unsigned int nProgramID ) const
{
	glLinkProgram( nProgramID );
	GLint result = GL_TRUE;
	glGetProgramiv( nProgramID, GL_LINK_STATUS, &result );
	if ( result != GL_TRUE )
	{
		GLsizei LogSize = 0;
		glGetProgramiv( nProgramID, GL_INFO_LOG_LENGTH, &LogSize );
		char* szLog = new char[ LogSize + 1 ];
		glGetProgramInfoLog( nProgramID, LogSize, &LogSize, szLog );
		exception e( szLog );
		throw e;
	}
}

void CRenderer::UseProgram( unsigned int nProgramID ) const
{
	glUseProgram( nProgramID );
}

unsigned int CRenderer::GetUniformID( unsigned int nProgramID, const std::string& sVariableName ) const
{
	int id = glGetUniformLocation( nProgramID, sVariableName.c_str() );
	if ( id == -1 )
	{
		ostringstream ossMessage;
		ossMessage << "CRenderer::GetUniformID() : La variable uniforme \"" << sVariableName << "\" n'existe pas dans le program " << nProgramID ;
		exception e( ossMessage.str().c_str() );
		//throw e;
	}
	return id;
}

unsigned int CRenderer::GetAttributeID( unsigned int nProgramID, const std::string& sVariableName ) const
{
	int id = glGetAttribLocation( nProgramID, sVariableName.c_str() );
	if ( id == -1 )
	{
		ostringstream ossMessage; 
		ossMessage << "CRenderer::GetAttributeID() : L'attribut \"" << sVariableName << "\" n'existe pas dans le program " << nProgramID;
		exception e( ossMessage.str().c_str() );
		throw e;
	}
	return id;
}

void CRenderer::SendUniform3f( unsigned int nUniformID, float f1, float f2, float f3 )
{
	glUniform3f( nUniformID, f1, f2, f3 );
}

void CRenderer::SendUniform4f( unsigned int nUniformID, float f1, float f2, float f3, float f4 )
{
	glUniform4f( nUniformID, f1, f2, f3, f4 );
}

void CRenderer::SendUniform2fv( unsigned int nUniformID, int nCount, std::vector< float >& vValues ) const
{
	if ( ( (float)vValues.size() / 2. ) == (float)nCount )
		glUniform2fv( nUniformID, nCount, &vValues[ 0 ] );
	else
	{
		ostringstream oss;
		oss << "CRenderer::SendUniform2fv() : le nombre de flottants envoy�s (" << vValues.size() 
			<< ") ne correspond pas au nombre de vecteurs (" << nCount << ") qu'il devrait contenir";
		exception e( oss.str().c_str() );
		throw e;
	}
}

void CRenderer::SendUniform4fv( unsigned nUniformID, int nCount, std::vector< float >& vValues ) const
{
	if ( ( (float)vValues.size() / 4. ) == (float)nCount )
		glUniform4fv( nUniformID, nCount, &vValues[ 0 ] );
	else
	{
		ostringstream oss;
		oss << "CRenderer::SendUniform4fv() : le nombre de flottants envoy�s (" << vValues.size() 
			<< ") ne correspond pas au nombre de vecteurs (" << nCount << ") qu'il devrait contenir";
		exception e( oss.str().c_str() );
		throw e;
	}
}

void CRenderer::SendUniform4iv( unsigned nUniformID, int nCount, std::vector< int >& vValues ) const
{
	if ( ( (float)vValues.size() / 4. ) == (float)nCount )
		glUniform4iv( nUniformID, nCount, &vValues[ 0 ] );
	else
	{
		ostringstream oss;
		oss << "CRenderer::SendUniform4fv() : le nombre de flottants envoy�s (" << vValues.size()
			<< ") ne correspond pas au nombre de vecteurs (" << nCount << ") envoy�es";
		exception e( oss.str().c_str() );
		throw e;
	}
}

void CRenderer::SendUniform1i( unsigned int nUniformID, int nValue ) const
{
	glUniform1i( nUniformID, nValue );
}

void CRenderer::SendUniform1f( unsigned int nUniformID, float fValue ) const
{
	glUniform1f( nUniformID, fValue );
}

void CRenderer::SendUniformMatrix4fv( unsigned int nUniformID, int nCount, bool bTranspose, vector< float >& vValues )
{
	if ( (float)vValues.size() / 16. == (float)nCount )
		glUniformMatrix4fv( nUniformID, nCount, bTranspose, &vValues[ 0 ] );
	else
	{
		ostringstream oss;
		oss << "CRenderer::SendUniformMatrix4fv() : le nombre de flottants envoy�s (" << vValues.size() 
			<< ") ne correspond pas au nombre de matrices (" << nCount << ") envoy�es";
		exception e( oss.str().c_str() );
		throw e;
	}
}

void CRenderer::EnableVertexAttribArray( unsigned int nAttributeID )
{
	glEnableVertexAttribArray( nAttributeID );
}

void CRenderer::DisableVertexAttribArray( unsigned int nAttributeID )
{
	glDisableVertexAttribArray( nAttributeID );
}

void CRenderer::VertexAttribPointer( unsigned int nAttributeID, int nSize, TNumberType type, int nPos )
{
	std::map< TNumberType, GLenum >::const_iterator it = m_mNumberType.find( type );
	GLenum glType = it->second;
	glVertexAttribPointer( nAttributeID, nSize, glType, GL_FALSE, 0, BUFFER_OFFSET( nPos ) );
	GLenum error = glGetError();
	if( error != 0 )
		throw 1;
}

void CRenderer::AbonneToRenderEvent( TRenderEventCallback callback )
{
	m_vRenderEventCallback.push_back( callback );
}

void CRenderer::DesabonneToRenderEvent( TRenderEventCallback callback )
{
	for( vector< TRenderEventCallback >::iterator it = m_vRenderEventCallback.begin(); it != m_vRenderEventCallback.end(); it++ )
	{
		if( callback == *it )
		{
			m_vRenderEventCallback.erase( it );
			break;
		}
	}
}

void CRenderer::BeginDirectGeometry( IRenderer::TDrawStyle style )
{
	glBegin( m_mDrawStyle[ style ] );
}

void CRenderer::EndDirectGeometry()
{
	glEnd();
}

void CRenderer::SetVertex3f( float x, float y, float z )
{
	glVertex3f( x, y, z );
}

void CRenderer::SetVertex4f( float x, float y, float z, float w )
{
	glVertex4f( x, y, z, w );
}

void CRenderer::SetColor3f( float r, float g, float b )
{
	glColor3f( r, g, b );
}

void CRenderer::SetDebugString( std::string s )
{
	IRenderer::SetDebugString( s );
}

void CRenderer::GetDebugString( string& s ) const
{
	IRenderer::GetDebugString( s );
}

void CRenderer::DisplayDebugInfos( bool bDisplay )
{
	IRenderer::DisplayDebugInfos( bDisplay );
}

bool CRenderer::IsDisplayDebugInfos() const
{
	return m_bDisplayDebugInfos;
}


void CRenderer::SetCurrentObjectLocalTM( CMatrix& m )
{
	IRenderer::SetCurrentObjectLocalTM( m );
}

void CRenderer::SetCurrentCameraMatrix( CMatrix& m )
{
	IRenderer::SetCurrentCameraMatrix( m );
}

void CRenderer::ReadPixels( int x, int y, int nWidth, int nHeight, vector< unsigned char >& vPixels, TPixelFormat format )
{
	//int nWidth, nHeight;
	//GetResolution( nWidth, nHeight );
	int size = 0;
	switch( format )
	{
	case TPixelFormat::T_RGB:
	case TPixelFormat::T_BGR:
		size = 3;
		break;
	case TPixelFormat::T_RGBA:
	case TPixelFormat::T_BGRA:
		size = 4;
		break;
	default:
		throw 1;
	}/*
	int viewport[ 4 ];
	glGetIntegerv( GL_VIEWPORT, viewport );
	int nWidth = viewport[ 3 ];
	int nHeight = viewport[ 0 ];*/
	vPixels.resize( nWidth * nHeight * size );
	glPixelStorei(GL_PACK_ALIGNMENT, 1 );	
	glReadPixels( x, y, nWidth, nHeight, m_mPixelFormat[ format ], GL_UNSIGNED_BYTE, &vPixels[ 0 ]);
}

extern "C" _declspec(dllexport) IRenderer* CreateRenderer( const IRenderer::Desc& oDesc )
{
	return new CRenderer( oDesc );
}

void CRenderer::Test( int nShader )
{
	//int nID = glGetUniformLocation( nShader, "matBones" );
	//float pArray[ 16 ] = { 0., 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	//glUniformMatrix4fv( nID, 1, false, pArray );

	int nID = glGetUniformLocation( nShader, "matBones" );
	float pArray[ 16 ] = { 0., 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 14., 15. };
	glUniformMatrix4fv( nID, 1, false, pArray );
}