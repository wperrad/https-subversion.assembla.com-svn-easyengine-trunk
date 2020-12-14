#ifndef MAX_EXPORTER_H
#define MAX_EXPORTER_H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "stdmat.h"
#include "decomp.h"
#include "shape.h"
#include "interpik.h"
#include "IGame.h"
#include "IGameModifier.h"

// math
#include "math/matrix.h"
#include "math/quaternion.h"

// stl
#include <vector>
#include <string>
#include <map>

using namespace std;

TCHAR* GetString(int id);

class IFileSystem;
class IGeometryManager;
class ILoaderManager;

struct CKey
{
	enum TKey{ eRotKey = 0, ePosKey };
	CKey() : m_nTimeValue( 0 ){}
	int					m_nTimeValue;
	CMatrix				m_oWorldTM;
	CMatrix				m_oLocalTM;
	AngAxis				m_oAngleAxis;
	CQuaternion			m_qLocal;
	TKey				m_eType;
};

// Class de base pour tous les plugins d'export 3DSMAX
class CMaxExporter : public SceneExport 
{
	wstring				m_wVersion;

protected:
	vector< wstring >	m_vNonTCBBoneNames;
	bool				g_bInterruptExport;
	bool				m_bOpenglCoord;
	IFileSystem*		m_pFileSystem;
	IGeometryManager*	m_pGeometryManager;
	ILoaderManager*		m_pLoaderManager;

	static bool			IsBone( Object* pObject );
	static bool			IsBone( INode* pNode );
	static void			MaxMatrixToEngineMatrix( const Matrix3& mMax, CMatrix& mEngine );
	static void			EngineMatrixToMaxMatrix( const CMatrix& oEngine, Matrix3& mMax );

	void				GetAnimation( Interface* pInterface, const std::map< int, INode* >& mBone, std::map< int, std::vector< CKey > >& vBones );
	Mesh&				GetMeshFromNode(INode* pMesh);
	void				GetVertexArrayFromMesh(Mesh& mesh, vector<float>& vertex);

public:
	CMaxExporter();
	~CMaxExporter();

	// SceneExport methods
	virtual int			ExtCount() = 0;     // Number of extensions supported 
	virtual const 		TCHAR * Ext(int n) = 0;     // Extension #n (i.e. "ASC")
	const 				TCHAR * LongDesc();     // Long ASCII description (i.e. "Ascii Export") 
	const 				TCHAR * ShortDesc();    // Short ASCII description (i.e. "Ascii")
	const 				TCHAR * AuthorName();    // ASCII Author name
	const 				TCHAR * CopyrightMessage();   // ASCII Copyright message 
	const 				TCHAR * OtherMessage1();   // Other message #1
	const 				TCHAR * OtherMessage2();   // Other message #2
	unsigned int		Version();     // Version number * 100 (i.e. v3.01 = 301) 
	void				ShowAbout(HWND hWnd);  // Show DLL's "About..." box
	virtual int			DoExport(const TCHAR *name,ExpInterface *ei, Interface *i, BOOL suppressPrompts=FALSE, DWORD options = 0 ) = 0;

	static std::vector<ClassDesc*>	m_vLibClassDesc;
};

class CMaxExporterClassDesc : public ClassDesc 
{
public:
	CMaxExporterClassDesc();
	int					IsPublic();
	virtual void*		Create(BOOL loading = FALSE) = 0;
	const TCHAR*		ClassName();
	SClass_ID			SuperClassID();
	virtual Class_ID	ClassID() = 0;
	const TCHAR*		Category();
};


#endif // MAX_EXPORTER_H

