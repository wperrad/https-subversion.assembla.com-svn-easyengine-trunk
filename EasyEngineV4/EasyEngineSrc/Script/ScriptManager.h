#include "IScriptManager.h"
#include "IFileSystem.h"

#include <vector>

class CLexAnalyser;
class CSyntaxAnalyser;
class CSemanticAnalyser;
class CAsmGenerator;
class CBinGenerator;
class CVirtualProcessor;

using namespace std;

class CScriptManager : public IScriptManager
{
	CLexAnalyser*					m_pLexAnalyser;
	CSyntaxAnalyser*				m_pSyntaxAnalyser;
	CSemanticAnalyser*				m_pSemanticAnalyser;
	CAsmGenerator*					m_pCodeGenerator;
	CBinGenerator*					m_pBinGenerator;
	CVirtualProcessor*				m_pProc;
	
public:
	CScriptManager( const Desc& oDesc );
	~CScriptManager();
	void	RegisterFunction( std::string sFunctionName, ScriptFunction Function, const vector< TFuncArgType >& vArgsType );
	void	ExecuteCommand( std::string sCommand );
	void	GetRegisteredFunctions( vector< string >& vFuncNames );
	float	GetVariableValue(string variableName);
};

extern "C" _declspec(dllexport) IScriptManager* CreateScriptManager( IScriptManager::Desc& oDesc );