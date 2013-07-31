#ifndef IDRAWTOOL_H
#define IDRAWTOOL_H

#include "math/Vector.h"
#include "EEPlugin.h"
#include "IRenderer.h"

class IRenderer;
class CVector;
class CMatrix;
class ICameraManager;
class CNode;
class IRessourceManager;

class IDrawTool : public CPlugin
{
protected:
	IDrawTool( const Desc& oDesc ) : CPlugin( oDesc.m_pParent, oDesc.m_sName ){}

public:
	struct Desc : public CPlugin::Desc
	{
		IRenderer&			m_oRenderer;
		ICameraManager&		m_oCameraManager;
		IRessourceManager&	m_oRessourceManager;
		Desc( IRenderer& oRenderer, ICameraManager& oCameraManager, IRessourceManager&	oRessourceManager ) : 
			CPlugin::Desc( NULL, "" ),
			m_oRenderer( oRenderer ), 
			m_oCameraManager( oCameraManager ),
			m_oRessourceManager( oRessourceManager ) {}
	};

	virtual CNode*		CreateBase( float fAxisSize, const CVector vColor = CVector( 1, 1, 1, 1 ) ) = 0;
	virtual void		DrawLine( const CVector& p1, const CVector& p2 ) = 0;
	virtual void		DrawBase( const CMatrix& m, float fAxisSize ) = 0;
	virtual void		DrawBase( CNode* pNode ) = 0;
	//virtual CNode*		CreateTestMesh( const std::string& sFileName ) = 0;
	virtual void		DrawTestMesh( CNode* pNode, IRenderer::TDrawStyle style ) = 0;
};

#endif // IDRAWTOOL_H