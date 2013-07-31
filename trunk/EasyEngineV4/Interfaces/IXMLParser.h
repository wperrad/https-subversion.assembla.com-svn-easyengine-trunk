#ifndef IXMLPARSER_H
#define IXMLPARSER_H

// stl
#include <string>
#include <vector>

// Engine
#include "EEPlugin.h"

class IFileSystem;
class CRectangle;
class CPosition;
class CDimension;

class  IXMLInfo
{
public:
	IXMLInfo( const std::string& sProperty, const CRectangle& rect ){}
	IXMLInfo( const std::string& szProperty, float x, float y, float width, float height ){}
	virtual void		GetRectangle( CRectangle& rect ) = 0;
	virtual void		GetName( std::string& sName ) = 0;
};

class IXMLParser : public CPlugin
{
protected:
	IXMLParser( const Desc& oDesc ) : CPlugin( oDesc.m_pParent, oDesc.m_sName ){}
public:
	struct Desc : public CPlugin::Desc
	{ 
		IFileSystem&	m_oFileSystem;
		Desc( IFileSystem& oFileSystem ) : 
			CPlugin::Desc( NULL, "" ),
			m_oFileSystem( oFileSystem ){}
	};
	virtual void		OpenFile( const std::string& sXMLFileName ) = 0;
	virtual void		CloseFile() = 0;
	virtual void		GetProperty( const std::string& sFieldName, CPosition& Pos, int& nDimWidth, int& nDimHeight ) = 0;
	virtual void		ParseFile( const std::string& sFileName, std::vector< IXMLInfo* >& vInfos ) = 0;
};

#endif // IXMLPARSER_H