#include <vector>
#include <map>
#include "BinGenerator.h"
#include "SemanticAnalyser.h"

using namespace std;

#define MEM_SIZE 128

class CVirtualProcessor
{
	typedef void (*TInstrFunc)( unsigned char* );
	float	m_nEip;
	float	m_nEax;
	float	m_nEbx;
	float	m_nEcx;
	float	m_nEdx;
	float 	m_nEsi;
	float 	m_nEdi;
	float 	m_nEbp;
	float 	m_nEsp;

	map<int, int> m_mEbpValueByScope;


	float	m_pMemory[ MEM_SIZE ];

	map< CBinGenerator::TProcInstr, TInstrFunc > m_mInstrFunc;
	vector< float* >	m_vRegAddr;
	bool				m_bEnd;


	void DecodeInstruction( unsigned char* pInstr, int nSize );
	void ExecuteInstruction( unsigned char* pOperand, int nSize );

	static void MovRegReg( unsigned char* pOperand );
	static void MovRegImm( unsigned char* pOperand );
	static void MovRegAddr( unsigned char* pOperand );
	static void MovAddrReg( unsigned char* pOperand );
	static void MovAddrImm( unsigned char* pOperand );

	static void AddRegReg( unsigned char* pOperand );
	static void AddRegImm( unsigned char* pOperand );

	static void MulRegReg( unsigned char* pOperand );
	static void MulRegImm( unsigned char* pOperand );

	static void DivRegReg( unsigned char* pOperand );
	static void DivRegImm( unsigned char* pOperand );

	static void SubRegImm( unsigned char* pOperand );

	static void PushReg( unsigned char* pOperand );
	static void PushImm( unsigned char* pOperand );
	static void PushAddr( unsigned char* pOperand );

	static void PopReg( unsigned char* pOperand );

	static void IntImm( unsigned char* pOperand );

	static void Ret( unsigned char* pOperand );

	static CVirtualProcessor* s_pCurrentInstance;
	static CSemanticAnalyser* s_pSemanticAnalyser;

	static int GetMemRegisterAddress(unsigned char* pOperand);


public:
	CVirtualProcessor( CSemanticAnalyser* pSemanticAnalyser );
	void	Execute( const vector< unsigned char >& vBinary, const vector< int >& vInstrSize );
	float	GetVariableValue(string varName);
	float	GetRegisterValue(CRegister::TType reg);
};