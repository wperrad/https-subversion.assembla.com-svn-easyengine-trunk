#include "StringUtils.h"
#include <algorithm>

using namespace std;


void CStringUtils::ExtractFloatFromString( const string& sString, vector< float >& vFloat, unsigned int nCount )
{
	vFloat.clear();
	size_t nIndiceWord = 0;
	unsigned int nCurrentIndex=0;
	while (nIndiceWord < sString.size() )
	{
		while( ( !IsFloat( sString[ nIndiceWord ] ) ) && ( nIndiceWord < sString.size() ) )
			nIndiceWord++;
		string sSubString;
		while ( IsFloat( sString[ nIndiceWord ] ) )
		{
			sSubString.push_back( sString[ nIndiceWord ] );			
			nIndiceWord++;
		} 		
		float fVal = static_cast<float> ( atof( sSubString.c_str() ) );		
		nIndiceWord++;
		vFloat.push_back( fVal );
		nCurrentIndex++;
		if ( nCount <= nCurrentIndex )
			return;
	}
}


bool CStringUtils::IsFloat(char c)
{
	bool res = false;
	if ( (c == '.') || (c == '-'))
		res = true;
	else
	{
		if (IsInteger(c))
			res = true;
	}
	return res;
}


bool CStringUtils::IsInteger(char c)
{
	return ( (c >= '0') && (c <= '9') );
}

int CStringUtils::FindEndOf( const string& sString, const string& sWord )
{
	int nRet = (int)sString.find_first_of( sWord );
	if (nRet != -1)	
		nRet += (int)sWord.size();
	return nRet;
}


void CStringUtils::GetWordByIndex( const string& sString, const unsigned int nNumWord, string& sRetWord )
{
	unsigned int nIndexString=0, nIndexRetWord=0, nCurrentNumWord=0;
	bool bBeginByBlank=false;
	do
	{
		if ( ( sString[ nIndexString ] == ' ') || ( sString[ nIndexString ] ==  '	' ) )
		{
			if (nIndexString ==0)
				bBeginByBlank = true;
			else
				bBeginByBlank = false;
			do
			{
				nIndexString++;
			}
			while ( ( sString[ nIndexString ] == ' ' ) || ( sString[ nIndexString ] ==  '	' ) );
			if ( !bBeginByBlank )
				nCurrentNumWord++;			
		}
		
		if (nNumWord == nCurrentNumWord)
		{
			while ( sString[ nIndexString ] != ' ' && sString[ nIndexString ] != '	' )
			{
				sRetWord.push_back( sString[ nIndexString ] );
				nIndexRetWord++;
				nIndexString++;
			}
			return;
		}
		nIndexString ++;
	} 
	while( nIndexString < sString.size() );
}

void CStringUtils::GetExtension( std::string sFileName, std::string& sExtension )
{
	int iDotPos = (int)sFileName.find_last_of( "." );
	string sExt = sFileName.substr( iDotPos + 1, sFileName.size() - iDotPos - 1 );
	sExtension = sExt;
	transform( sExt.begin(), sExt.end(), sExtension.begin(), tolower );
}

void CStringUtils::GetFileNameWithoutExtension( string sFileName, string& sOut )
{
	int nSlashPos = sFileName.find_last_of( "\\" );
	int nDotPos = sFileName.find_last_of( "." );
	sOut = sFileName.substr( nSlashPos + 1, nDotPos - nSlashPos - 1 );
}