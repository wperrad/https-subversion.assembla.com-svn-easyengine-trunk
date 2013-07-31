#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <vector>
#include <string>

using namespace std;

class  CStringUtils
{
public:
	static void  	ExtractFloatFromString( const std::string& sString, std::vector< float >& vFloat, unsigned int nCount );
	static bool	 	IsFloat( char c );
	static bool	 	IsInteger( char c );
	static int		FindEndOf( const std::string& sString, const std::string& sWord );
	static void 	GetWordByIndex( const std::string& sString, const unsigned int nIndex, std::string& sRetWord );
	static void		GetExtension( std::string sFileName, std::string& sExtension );
	static void		GetFileNameWithoutExtension( string sFileName, string& sOut );
};


#endif // STRINGUTILS_H