#if 0
#ifndef ITESTSHADERMANAGER_H
#define ITESTSHADERMANAGER_H

#include "EEPlugin.h"

#include <string>
#include <vector>

class CMatrix;
class CVector;
class CChunk;

class ITestShaderManager : public CPlugin
{
protected:
	ITestShaderManager( const Desc& oDesc ) : CPlugin( oDesc.m_pParent, oDesc.m_sName ){}

public:
	virtual void		SetCurrentShader( const std::string& sShaderName ) = 0;
	virtual void		SetModelView( const CMatrix& oModelViewMatrix ) = 0;
	virtual void		SetProjection( const CMatrix& oProjectionMatrix ) = 0; 
	virtual void		Process( const CVector& vVertex, CVector& vPos, CVector& vColor, int nVertexNum  )const = 0;
	virtual void		SendAttributeArray( const std::string& sAttribName, const std::vector< float > vAttrib, int nAttribSize ) = 0;
	virtual void		SendUniformMatrix( const std::string& sMatrixArrayName, const std::vector< CMatrix >& vMatrixArray ) = 0;
};

#endif // ITESTSHADERMANAGER_H

#endif // 0