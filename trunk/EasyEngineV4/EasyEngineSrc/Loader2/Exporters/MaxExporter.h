//************************************************************************** 
//* Asciiexp.h	- Ascii File Exporter
//* 
//* By Christer Janson
//* Kinetix Development
//*
//* January 20, 1997 CCJ Initial coding
//*
//* Class definition 
//*
//* Copyright (c) 1997, All Rights Reserved. 
//***************************************************************************

#ifndef __ASCIIEXP__H
#define __ASCIIEXP__H

// stl
#include <map>
#include <string>
#include <vector>

// Autodesk
#include "Max.h"

// Engine
//#include "../../Utils2/chunk.h"
#include "math/matrix.h"
#include "math/quaternion.h"
#include "ExportDialog.h"

class CNode;

extern ClassDesc* GetAsciiExpDesc();
extern HINSTANCE hInstance;

#define VERSION			200
#define EASYENGINEEXP_CLASS_ID	Class_ID(0x85548e0c, 0x4a26450e)

class CWeightTable;
class CWeightTable2;
class CChunk;
class CMatrix;

struct CWeightVertex
{
	float m_x,m_y,m_z;
	std::map< int, float > m_mWeight;
	CWeightVertex( float x, float y, float z, const std::map< int, float >& mWeight ) : m_x(x), m_y(y), m_z(z), m_mWeight( mWeight ){}
};

struct CKey
{
	int					m_nTime;
	CMatrix				m_oWorldTM;
	CMatrix				m_oLocalTM;
	AngAxis				m_oAngleAxis;
	CQuaternion			m_qLocal;
};


INT_PTR CALLBACK ExportOptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;

class MaxExporter : public SceneExport 
{
	
	bool				m_bMultipleSmGroup;
	int					m_nCurrentSmGroup;
	void				GetGeometry( CChunk& oChunk, INode* pRoot );
	void				DumpWeightedVertex( const std::string& sFilePath, const std::vector< CWeightVertex >& vWVertex );	
	void				DumpModel( const std::string& sFilePath, const CChunk& oChunk, const std::map< int, std::map< int, float > >& oWeightVertexID, const std::map< int, INode* >& mNodeID  );
	void				DumpAnimation( const std::string& sFilePath, const std::map< int, std::vector< CKey > >& vBones );
	static void			DumpMatrix( FILE* pFile, const Matrix3& m );
	void				DumpMatrix( FILE* pFile, const CMatrix& m ) const;
	void				DumpQuaternion( FILE* pFile, const CQuaternion& q ) const;
	void				DumpAngleAxis( FILE* pFile, const AngAxis& a ) const;
	static void			DumpSkeleton( FILE* pFile, const std::map< int, INode* >& mNodeID );
	static void			DumpSkinning( FILE* pFile, const std::map< int, std::map< int, float > >& mWeightVertexID );

	static void			GetSkeleton( INode* pRoot, std::map< std::string, INode* >& mBone );
	static void			GetSkeleton( INode* pRoot, std::map< std::string, CNode* >& mBone );
	void				GetWeightTable( CWeightTable2& oWeightTable, const std::map< std::string, int >& mBoneID );
	void				GetAnimation( Interface* pInterface, const std::map< int, INode* >& mBone, std::map< int, std::vector< CKey > >& vBones );
	Point3				GetVertexNormal( Mesh& oMesh, int faceNo, RVertex* rv );
	BOOL				TMNegParity( Matrix3 &m );
	void				GetBonesIDByName( INode* pRoot, std::map< std::string, int >& mBoneIDByName ) const;
	void				GetBoneByID( const std::map< std::string, INode* >& mBoneByName, const std::map< std::string, int >& mBoneIDByName, std::map< int, INode* >& mBoneByID );

	static void			MaxMatrixToEngineMatrix( const Matrix3& mMax, CMatrix& mEngine );
	static void			MaxNodeToEngineNode( INode& oMaxNode, CNode& oNode );

public:
	MaxExporter();
	~MaxExporter();

	int					ExtCount();
	const TCHAR* 		Ext(int n);
	const TCHAR* 		LongDesc();
	const TCHAR* 		ShortDesc();
	const TCHAR* 		AuthorName();
	const TCHAR* 		CopyrightMessage();
	const TCHAR* 		OtherMessage1();
	const TCHAR* 		OtherMessage2();
	unsigned int		Version();
	void				ShowAbout(HWND hWnd);  // Show DLL's "About..." box
	int					DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0); // Export	file
	BOOL				SupportsOptions(int ext, DWORD options);
	bool				m_bFlipNormal;	
};

class CWeightTable2
{
public:
	std::map< int, std::map< int, float > >	m_mVertex;
	CWeightTable2();
	~CWeightTable2();
	void	Add( int iVertexIndex, int nBoneIDe, float fBoneWeight );
	void	Get( int iVertexIndex, std::map< int, float >& mWeight ) const;
	int		GetVertexCount() const;
};

class CWeightTable
{
	std::map< int, std::map< std::string, float > >	m_mVertex;

public:
	CWeightTable();
	void	Add( int iVertexIndex, std::string sBoneName, float fBoneWeight );
};

#endif // __ASCIIEXP__H

