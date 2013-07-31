#ifndef DRAWTOOL_CPP
#error
#endif //DRAWTOOL_CPP

#ifndef DRAWTOOL_H
#define DRAWTOOL_H

#include <map>
#include <vector>
#include <string>
#include "IDrawTool.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"

class CNode;
class ITestMesh;

class CDrawTool : public IDrawTool
{
	IRenderer&						m_oRenderer;
	IShaderManager&					m_oShaderManager;
	ICameraManager&					m_oCameraManager;
	std::map< CNode*, float >		m_mBase;
	std::map< CNode*, ITestMesh* >	m_mTestMesh;
	CTestShaderManager				m_oTestShaderManager;
	CMatrix							m_oProjectionMatrix;
	IRessourceManager&				m_oRessourceManager;

public:
	CDrawTool( const Desc& );
	void			DrawLine( const CVector& p1, const CVector& p2 );
	void			DrawBase( const CMatrix& m, float fAxisSize );
	CNode*			CreateBase( float fAxisSize, const CVector vColor = CVector( 1, 1, 1, 1 ) );
	CNode*			CreateTestMesh( const std::string& sFileName );
	void			DrawBase( CNode* pNode );
	void			DrawTestMesh( CNode* pNode, IRenderer::TDrawStyle style );
};

extern "C" _declspec(dllexport) IDrawTool* CreateDrawTool( const IDrawTool::Desc& );

#endif // DRAWTOOL_H