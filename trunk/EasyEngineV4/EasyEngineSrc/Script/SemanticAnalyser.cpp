#include "SemanticAnalyser.h"
#include "Exception.h"
#include <sstream>

void CScriptState::AddArg( IScriptFuncArg* pArg )
{
	m_vArg.push_back( pArg );
}

IScriptFuncArg* CScriptState::GetArg( int iIndex )
{
	if( iIndex >= m_vArg.size() )
	{
		CBadArgCountException e( iIndex );
		throw e;
	}
	return m_vArg[ iIndex ];
}

CSemanticAnalyser::CSemanticAnalyser():
m_nCurrentScopeNumber( 0 )
{
}

void CSemanticAnalyser::RegisterFunction( std::string sFunctionName, ScriptFunction Function, const vector< TFuncArgType >& vArgsType  )
{
	FuncMap::const_iterator it = m_mInterruption.find( sFunctionName );
	if( it == m_mInterruption.end() )
	{
		m_mInterruption[ sFunctionName ].first = Function;
		m_mInterruption[ sFunctionName ].second = vArgsType;
	}
	else
		throw 1;
}

void CSemanticAnalyser::GetRegisteredFunctions( vector< string >& vFuncNames )
{
	for( FuncMap::iterator itFunc = m_mInterruption.begin(); itFunc != m_mInterruption.end(); itFunc++ )
		vFuncNames.push_back( itFunc->first );
}

VarMap& CSemanticAnalyser::GetVarMap()
{
	return m_mVar;
}

const CVar* CSemanticAnalyser::GetVariable(string varName)
{
	CVar* ret = NULL;
	int nScope = m_nCurrentScopeNumber;

	while (!ret && nScope >= 0) {
		VarMap::iterator itVarMap = m_mVar.find(nScope);
		if (itVarMap != m_mVar.end()) {
			map<string, CVar>::iterator itVar = itVarMap->second.find(varName);
			if (itVar != itVarMap->second.end()) {
				ret = &itVar->second;
			}
		}
		nScope--;
	}
	return ret;
}

void CSemanticAnalyser::CompleteSyntaxicTree( CSyntaxNode& oTree )
{
	if( oTree.m_Lexem.m_eType == CLexAnalyser::CLexem::eFunction )
	{
		FuncMap::iterator it = m_mInterruption.find( oTree.m_Lexem.m_sValue );
		if( it == m_mInterruption.end() )
		{
			string sMessage = string( "Fonction \"" ) + oTree.m_Lexem.m_sValue + "\" non d�finie";
			CCompilationErrorException e( -1, -1 );
			e.SetErrorMessage( sMessage );
			throw e;
		}
		else
			oTree.m_Type = CSyntaxNode::eAPIFunction;
		if( it->second.second.size() != oTree.m_vChild.size() )
		{
			ostringstream oss;
			oss << "Fonction \"" << oTree.m_Lexem.m_sValue << "\" : nombre d'argument incorrect, " << it->second.second.size() << " arguments requis";
			CCompilationErrorException e( -1, -1 );
			e.SetErrorMessage( oss.str() );
			throw e;
		}
		oTree.m_nAddress = (unsigned int)std::distance( m_mInterruption.begin(), it );
		for( unsigned int i = 0; i < oTree.m_vChild.size(); i++ )
			CompleteSyntaxicTree( oTree.m_vChild[ i ] );
	}
	else if( oTree.m_Type == CSyntaxNode::eVal )
	{
		if( oTree.m_vChild.size() == 0 )
		{
			switch (oTree.m_Lexem.m_eType)
			{
			case CLexAnalyser::CLexem::eInt:
				oTree.m_Type = CSyntaxNode::eInt;
				break;
			case CLexAnalyser::CLexem::eFloat:
				oTree.m_Type = CSyntaxNode::eFloat;
				break;
			case CLexAnalyser::CLexem::eString:
			{
				oTree.m_Type = CSyntaxNode::eString;
#ifndef STRING_IN_BIN
				map< string, int >::iterator itString = m_mStringAddress.find(oTree.m_Lexem.m_sValue);
				if (itString == m_mStringAddress.end())
				{
					m_mStringAddress.insert(map< string, int >::value_type(oTree.m_Lexem.m_sValue, (int)m_mStringAddress.size()));
					oTree.m_nAddress = (unsigned int)m_mStringAddress.size() - 1;
					m_mAddressString[oTree.m_nAddress] = oTree.m_Lexem.m_sValue;
				}
				else
					oTree.m_nAddress = itString->second;
#endif // 0
				break;
			}
			case CLexAnalyser::CLexem::eVar:
				AddNewVariable(oTree);
				break;
			default:
				throw 1;
				break;
			}
		}
		else
		{
			SetTypeFromChildType( oTree );
			for( unsigned int i = 0; i < oTree.m_vChild.size(); i++ )
				CompleteSyntaxicTree( oTree.m_vChild[ i ] );
		}
	}
	else if( oTree.m_Lexem.m_eType == CLexAnalyser::CLexem::eVar )
	{
		AddNewVariable(oTree);
		
		for( unsigned int i = 0; i < oTree.m_vChild.size(); i++ )
			CompleteSyntaxicTree( oTree.m_vChild[ i ] );
	}
	else
	{
		for( unsigned int i = 0; i < oTree.m_vChild.size(); i++ )
			CompleteSyntaxicTree( oTree.m_vChild[ i ] );
	}
}

void CSemanticAnalyser::AddNewVariable(CSyntaxNode& oTree)
{
	string sVarName = oTree.m_Lexem.m_sValue;

	map< string, CVar >& mVar = m_mVar[m_nCurrentScopeNumber];
	map< string, CVar >::iterator itVar = mVar.find(sVarName);
	if (itVar == mVar.end())
	{
		int nPosition = mVar.size();
		CVar v;
		v.m_nScopePos = m_nCurrentScopeNumber;
		v.m_nRelativeStackPosition = nPosition;
		mVar[sVarName] = v;
	}
	oTree.m_nScope = m_nCurrentScopeNumber;
}

void CSemanticAnalyser::SetTypeFromChildType( CSyntaxNode& oTree )
{
	vector< int > vChildToAnalayse;
	if( oTree.m_vChild.size() == 0 )
	{
		switch( oTree.m_Lexem.m_eType )
		{
		case CLexAnalyser::CLexem::eInt:
			oTree.m_Type = CSyntaxNode::eInt;
			break;
		case CLexAnalyser::CLexem::eFloat:
			oTree.m_Type = CSyntaxNode::eFloat;
			break;
		}
	}
	else if( (	oTree.m_vChild[ 0 ].m_Type == CSyntaxNode::eInt ||
				oTree.m_vChild[ 0 ].m_Type == CSyntaxNode::eFloat ) 
			&&
			(	oTree.m_vChild[ 1 ].m_Type == CSyntaxNode::eInt ||
				oTree.m_vChild[ 1 ].m_Type == CSyntaxNode::eFloat )
		)
	{
		if( oTree.m_vChild[ 0 ].m_Type == CSyntaxNode::eInt &&
			oTree.m_vChild[ 1 ].m_Type == CSyntaxNode::eInt )
			oTree.m_Type = CSyntaxNode::eInt;
		else
			oTree.m_Type = CSyntaxNode::eFloat;
	}
	else
	{
		for( unsigned int i = 0; i < oTree.m_vChild.size(); i++ )
		{
			if( oTree.m_vChild[ i ].m_Type != CSyntaxNode::eInt &&
				oTree.m_vChild[ i ].m_Type != CSyntaxNode::eFloat )
			{
				SetTypeFromChildType( oTree.m_vChild[ i ] );
			}
		}
		if( oTree.m_Lexem.m_eType != CLexAnalyser::CLexem::eFunction )
			if( oTree.m_Lexem.m_eType == CLexAnalyser::CLexem::eAffect )
			{
				oTree.m_vChild[ 0 ].m_Type = oTree.m_vChild[ 1 ].m_Type;
				oTree.m_Type = oTree.m_vChild[ 1 ].m_Type;
			}
			else
				SetTypeFromChildType( oTree );
	}
}

void CSemanticAnalyser::GetFunctionAddress( map< string, int >& mFuncAddr )
{
	for( FuncMap::iterator it = m_mInterruption.begin(); it != m_mInterruption.end(); ++it )
		mFuncAddr[ it->first ] = (int)distance( m_mInterruption.begin(), it );
}

unsigned int CSemanticAnalyser::GetFuncArgsCount( int nFuncIndex )
{
	FuncMap::iterator itFunc = m_mInterruption.begin();
	std::advance( itFunc, nFuncIndex );
	return (unsigned int)itFunc->second.second.size();
}

void CSemanticAnalyser::CallInterruption( int nIntIndex, const vector< float >& vArgs )
{
	FuncMap::iterator itFunc = m_mInterruption.begin();
	std::advance( itFunc, nIntIndex );
	CScriptState* pState = new CScriptState;
	vector< TFuncArgType >& vArgType = itFunc->second.second;
	try
	{
		for( unsigned int i = 0; i < vArgs.size(); i++ )
		{
			IScriptFuncArg* pArg = NULL;
			switch( vArgType[ i ] )
			{
			case eFloat:
				pArg = new CScriptFuncArgFloat( vArgs[ i ] );
				break;
			case eInt:
				pArg = new CScriptFuncArgInt( (int)vArgs[ i ] );
				break;
			case eString:
				pArg = new CScriptFuncArgString( m_mAddressString[ (int)vArgs[ i ] ] );
				break;
			}
			pState->AddArg( pArg );
		}
		itFunc->second.first( pState );
	}
	catch( CBadArgCountException& e )
	{
		ostringstream oss;
		oss << itFunc->first + " : nombre d'arguments incorrect, " << itFunc->second.second.size() << " arguments requis";
		e.SetErrorMessage( oss.str() );
		throw e;
	}
}

