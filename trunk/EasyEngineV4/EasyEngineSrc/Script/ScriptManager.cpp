#include "ScriptManager.h"
#include "LexAnalyser.h"
#include "SyntaxAnalyser.h"
#include "SemanticAnalyser.h"
#include "AsmGenerator.h"
#include "BinGenerator.h"
#include "VirtualProcessor.h"
#include "Exception.h"

//#define CREATE_ASSEMBLING_LISTING

CScriptManager::CScriptManager( const Desc& oDesc ):
IScriptManager( oDesc )
{
	m_pLexAnalyser = new CLexAnalyser( "lexanalyser.csv", &oDesc.m_oFileSystem );
	m_pSyntaxAnalyser = new CSyntaxAnalyser;
	m_pSemanticAnalyser = new CSemanticAnalyser;
	m_pCodeGenerator = new CAsmGenerator;
	m_pBinGenerator = new CBinGenerator;
	m_pProc = new CVirtualProcessor( m_pSemanticAnalyser );

	m_mRegisterFromName["eax"] = CRegister::eax;
	m_mRegisterFromName["ebx"] = CRegister::ebx;
	m_mRegisterFromName["ecx"] = CRegister::ecx;
	m_mRegisterFromName["edx"] = CRegister::edx;
	m_mRegisterFromName["esp"] = CRegister::esp;
	m_mRegisterFromName["ebp"] = CRegister::ebp;
	m_mRegisterFromName["esi"] = CRegister::esi;
	m_mRegisterFromName["edi"] = CRegister::edi;
	
}

CScriptManager::~CScriptManager()
{
	delete m_pLexAnalyser;
	delete m_pSyntaxAnalyser;
	delete m_pSemanticAnalyser;
	delete m_pCodeGenerator;
	delete m_pBinGenerator;
	delete m_pProc;
}

void CScriptManager::ExecuteCommand( std::string sCommand )
{
	string s;
	vector< CLexAnalyser::CLexem > vLexem;
	m_pLexAnalyser->GetLexemArrayFromScript( sCommand, vLexem );
	if (vLexem.size() == 0)
		return;
	CSyntaxNode oTree;
	m_pSyntaxAnalyser->GetSyntaxicTree( vLexem, oTree );
	m_pSemanticAnalyser->CompleteSyntaxicTree( oTree );
	vector< CAsmGenerator::CInstr > vAssembler;
	map< string, int > mFuncAddr;
	m_pSemanticAnalyser->GetFunctionAddress( mFuncAddr );
	
	m_pCodeGenerator->GenAssembler( oTree, vAssembler, mFuncAddr, m_pSemanticAnalyser->GetVarMap() );
#ifdef CREATE_ASSEMBLING_LISTING
	m_pCodeGenerator->CreateAssemblerListing( vAssembler, "test.asm" );
#endif // CREATE_ASSEMBLING_LISTING
	vector< unsigned char > vBin;
	m_pBinGenerator->GenBinary( vAssembler, vBin );
	m_pProc->Execute( vBin, CBinGenerator::s_vInstrSize );
}

void CScriptManager::GetRegisteredFunctions( vector< string >& vFuncNames )
{
	m_pSemanticAnalyser->GetRegisteredFunctions( vFuncNames );
}

float CScriptManager::GetVariableValue(string variableName)
{
	return m_pProc->GetVariableValue(variableName);
}

float CScriptManager::GetRegisterValue(string sRegisterName)
{
	return m_pProc->GetRegisterValue(m_mRegisterFromName[sRegisterName]);
}

void CScriptManager::RegisterFunction( std::string sFunctionName, ScriptFunction Function, const vector< TFuncArgType >& vArgsType )
{
	m_pSemanticAnalyser->RegisterFunction( sFunctionName, Function, vArgsType );
}

extern "C" _declspec(dllexport) IScriptManager* CreateScriptManager( IScriptManager::Desc& oDesc )
{
	return new CScriptManager( oDesc );
}
