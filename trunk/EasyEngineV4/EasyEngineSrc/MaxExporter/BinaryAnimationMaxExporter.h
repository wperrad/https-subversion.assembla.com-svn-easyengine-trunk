#include "MaxExporter.h"

// stl
#include <map>

class IFileStorage;

class CBinaryAnimationMaxExporterClassDesc : public CMaxExporterClassDesc
{
	void*		Create( BOOL loading = FALSE );
	Class_ID	ClassID();
};

const int g_nTickPerFrame = 160;

class CBinaryAnimationMaxExporter : public CMaxExporter
{
	void			GetBonesIDByName( INode* pRoot, std::map< std::string, int >& mBoneIDByName ) const;
	void			GetBoneByID( const std::map< std::string, INode* >& mBoneByName, const std::map< std::string, int >& mBoneIDByName, std::map< int, INode* >& mBoneByID );
	void			DumpAnimation( std::string sFilePath, const std::map< int, std::vector< CKey > >& vBones );
	
	static void		DumpAngleAxis( IFileStorage& fs, const AngAxis& a );
	static void		GetSkeleton( INode* pRoot, std::map< std::string, INode* >& mBone );

	static INT_PTR CALLBACK OnExportAnim(HWND, UINT, WPARAM, LPARAM );

public:
	int			m_nExportAnimationStart;
	int			m_nExportAnimationEnd;
	TimeValue	m_nAnimationStart;
	TimeValue	m_nAnimationEnd;

	CBinaryAnimationMaxExporter();
	int			ExtCount();
	const 		TCHAR * Ext(int n);
	int			DoExport(const TCHAR *name,ExpInterface *ei, Interface *i, BOOL suppressPrompts=FALSE, DWORD options = 0 );
	
};