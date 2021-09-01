#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "../Utils2/EasyFile.h"
#include "../Utils2/Rectangle.h"

#include <vector>
#include "IXMLParser.h"

class IFileSystem;
class CPosition;
class CDimension;
class EEInterface;

class CXMLInfo : public IXMLInfo
{

	std::string			m_sName;
	CRectangle			m_Rect;

public:
						CXMLInfo( const std::string& sProperty, CRectangle rect );
						CXMLInfo( const std::string& sProperty, float x, float y, float width, float height );
	void				GetRectangle( CRectangle& rect );
	void				GetName( std::string& sName );

};

class CXMLParser : public IXMLParser
{
	CEasyFile			m_File;
	bool				m_bIsParsing;
	IFileSystem&		m_oFileSystem;

	void				OpenFile( const std::string& sXMLFileName );
	void				CloseFile();

public:
						CXMLParser(EEInterface& oInterface);
	
	void				GetProperty( const std::string& sFieldName, CPosition& Pos, int& nDimWidth, int& nDimHeight );
	void				ParseFile( const std::string& sFileName, std::vector< IXMLInfo* >& vInfos );
	string				GetName() override;
};

extern "C" _declspec(dllexport) IXMLParser* CreateXMLParser(EEInterface& oInterface);

#endif //XMLPARSER_H