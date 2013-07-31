#ifndef SEMANTICANALYSER_H
#define SEMANTICANALYSER_H

#include "SyntaxAnalyser.h"
#include "IScriptManager.h"

class CScriptState : public IScriptState
{
	vector< IScriptFuncArg* >	m_vArg;
public:
	IScriptFuncArg* GetArg( int iIndex );
	void			AddArg( IScriptFuncArg* pArg );
};

struct CVar
{
	int		m_nScopePos;
	bool	m_bIsDeclared;
	CVar() : m_bIsDeclared( false ){}
};


typedef map< int, map< string, CVar > >	VarMap;

class CSemanticAnalyser
{
	typedef map< string, pair< ScriptFunction, vector< TFuncArgType > > > FuncMap;
	
	FuncMap					m_mInterruption;
	map< string, int >		m_mStringAddress;
	map< int, string >		m_mAddressString;
	VarMap					m_mVar;
	int						m_nCurrentScopeNumber;

public:
	CSemanticAnalyser();
	void			RegisterFunction( std::string sFunctionName, ScriptFunction Function, const vector< TFuncArgType >& vArgsType );
	void			CompleteSyntaxicTree( CSyntaxNode& oTree );
	void			GetFunctionAddress( map< string, int >& mFuncAddr );
	void			SetTypeFromChildType( CSyntaxNode& oTree );
	unsigned int	GetFuncArgsCount( int nFuncIndex );
	void			CallInterruption( int nIndex, const vector< float >& vArgs );
	void			GetRegisteredFunctions( vector< string >& vFuncNames );
	VarMap&			GetVarMap();
};

#endif // SEMANTICANALYSER_H