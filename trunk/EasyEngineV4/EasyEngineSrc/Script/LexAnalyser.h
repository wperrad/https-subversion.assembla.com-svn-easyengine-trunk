#ifndef LEXANALYSER_H
#define LEXANALYSER_H

#include <exception>
#include <string>
#include <vector>
#include <map>
using namespace std;

class IFileSystem;
class CCSVReader;



class CLexAnalyser
{
public:
	struct CLexem
	{
		enum TLexem
		{
			eNone = -1,
			eVar = 0,
			eFunction,
			eInt,
			eFloat,
			eLPar,
			eRPar,
			eAffect,
			eVirg,
			eDot,
			eGuill,
			eSub,
			eAdd,
			eDiv,
			eMult,
			ePtVirg,
			eString,
			eIdentifier
		};
		string			m_sValue;
		int				m_nValue;
		float			m_fValue;
		TLexem			m_eType;

		CLexem() : m_eType( eNone ), m_nValue(0), m_fValue(0.f){}
		CLexem( TLexem t ) : m_eType( t ), m_nValue(0), m_fValue(0.f){}
		bool	IsOperation()const;
		bool	IsNumeric()const;
	};

private:
	int									m_nStateCount;
	vector< vector< int > >				m_vAutomate;
	map< int, CLexem::TLexem >		m_mFinalStates;
	map< string, CLexem::TLexem >	m_mStringToLexemType;

	void					CalculStateCount( CCSVReader& r );
	void					CalculLexicalArrayFromCSV( string sCSVName, IFileSystem* pFS );
	void					CalculFinalStates( CCSVReader& r );
	int						GenStringFromRegExpr(std::string sExpr, std::string& sOut);
	int						GenHookRegExpr(string sExpr, string& sOut);
	void					InitStringToLexemTypeArray();
	static void				ReadUntilEndComment(string sScript, unsigned int& startIndex, int& line);


public:
	CLexAnalyser( string sCVSConfigName, IFileSystem* );
	void	GetLexemArrayFromScript( string sScript, vector< CLexem >& vLexem );
	
};

#endif // LEXANALYSER_H