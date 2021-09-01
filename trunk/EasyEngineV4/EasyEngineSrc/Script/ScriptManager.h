#include "IScriptManager.h"
#include "IFileSystem.h"
#include "AsmGenerator.h"

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
	map<string, CRegister::TType>	m_mRegisterFromName;
	bool							m_bGeneratedAssemblerListing;

public:
	CScriptManager(EEInterface& oInterface);
	~CScriptManager();
	void	RegisterFunction( std::string sFunctionName, ScriptFunction Function, const vector< TFuncArgType >& vArgsType ) override;
	void	ExecuteCommand( std::string sCommand ) override;
	void	GetRegisteredFunctions( vector< string >& vFuncNames ) override;
	float	GetVariableValue(string variableName) override;
	float	GetRegisterValue(string sRegisterName) override;
	string	GetName() override;
	void	GenerateAssemblerListing(bool generate) override;
};

extern "C" _declspec(dllexport) IScriptManager* CreateScriptManager(EEInterface& oInterface);