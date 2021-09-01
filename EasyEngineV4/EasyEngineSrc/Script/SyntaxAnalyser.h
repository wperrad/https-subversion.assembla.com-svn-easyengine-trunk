#ifndef SYNTAXANALYSER_H
#define SYNTAXANALYSER_H

#define VERSION2

#include "LexAnalyser.h"

#include <vector>

using namespace std;

class CSyntaxNode
{
public:
	enum NODE_TYPE
	{
		eNone = -1,
		eVal = 0,
		eOp,
		eInstr,
		eInt,
		eFloat,
		eString,
		eAPIFunction,
		eProg,
		eVecArgs,
		ePar,
		eCommand
	};

	CSyntaxNode();
	CSyntaxNode( CLexAnalyser::CLexem );
	static bool				IsValue(NODE_TYPE node);
	CLexAnalyser::CLexem	m_Lexem;
	vector< CSyntaxNode >	m_vChild;
	NODE_TYPE				m_Type;
	unsigned int			m_nAddress;
	void*					m_pUserData;
	int						m_nScope;
};

enum TParenthesisReductionType
{
	eNormal = 0,
	eFunc
};

class CSyntaxAnalyser
{
	void ReduceParenthesis( CSyntaxNode& oTree );
	void ReduceInstruction( CSyntaxNode& oTree );
	void ReduceAllOperations(  CSyntaxNode& oNode );
	void ReduceOperations( CSyntaxNode& oNode, const vector< CLexAnalyser::CLexem::TLexem >& vType );
	void DeleteTempNodes( CSyntaxNode& oNode );
	void ReduceVecArgs( CSyntaxNode& oNode );
	void DeleteParNodes( CSyntaxNode& oNode );

public:
	void GetSyntaxicTree( const vector< CLexAnalyser::CLexem >& vLexem, CSyntaxNode& oTree );
};

#endif //SYNTAXANALYSER_H