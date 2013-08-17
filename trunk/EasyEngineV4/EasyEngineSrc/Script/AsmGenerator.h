#ifndef CODEGEN_H
#define CODEGEN_H

#include <vector>
#include "LexAnalyser.h"
#include "SemanticAnalyser.h"

using namespace std;

class CSyntaxNode;

class IOperand
{
public:
	enum TOperandType
	{
		eNone = -1,
		eRegister,
		eNumeric,
		eMemory
	};


	virtual void			GetStringName( string& sName ) = 0;
	virtual TOperandType	GetType()const = 0;

	
};

class CRegister : public IOperand
{
public:
	enum TType
	{
		eNone = -1,
		eax = 0,
		ebx,
		ecx,
		edx,
		esi,
		edi,
		ebp,
		esp
	};
	
	TType	m_eValue;
	CRegister() : m_eValue( eNone ){}
	CRegister( TType value ) : m_eValue( value ){}

	void			GetStringName( string& sName );
	TOperandType	GetType()const{ return eRegister;  }
	static void	InitRegisterToStringMap();

private:
	 static map< TType, string >	s_mRegisterToString;	 
};

struct CNumeric : public IOperand
{
	double m_fValue;
	CNumeric() : m_fValue( 0.f ){}
	CNumeric( double value ) : m_fValue( value ){}
	void			GetStringName( string& sName );
	TOperandType	GetType()const{ return eNumeric; }
};

struct CMemory : public IOperand
{
	CRegister	m_oBase;
	CRegister	m_oIndex;
	int			m_nOffset;
	TOperandType GetType() const { return eMemory; }
	void		GetStringName( string& sName );
};

struct CPreprocessorString : public IOperand
{
	string	m_sValue;
	CPreprocessorString( string sValue ) : m_sValue( sValue ){}
	void			GetStringName( string& sName );
	TOperandType	GetType()const{ return eNone;}
};

class CAsmGenerator
{
public:

	enum TMnemonic
	{
		eMov = 0,
		eAdd,
		eMul,
		eSub,
		eDiv,
		eCall,
		eInt,
		ePush,
		ePop,
		eRet,
		eDB,
		eMnemonicCount
	};

	struct CInstr
	{
		TMnemonic					m_eMnem;
		vector < IOperand* >		m_vOperand;
	};

	CAsmGenerator();
	void GenReverseAssembler( const CSyntaxNode& oTree, vector< CInstr >& vCodeOut, const map<string, int>& mFuncAddr );
	void CreateAssemblerListing( vector< CInstr >& vCodeOut, string sFileName );
	void GenAssembler( const CSyntaxNode& oTree, vector< CInstr >& vCodeOut, const map<string, int>& mFuncAddr, VarMap& mVar );

private:
	map< CLexAnalyser::CLexem::TLexem, TMnemonic >	m_mTypeToMnemonic;
	map< TMnemonic, string >						m_mMnemonicToString;
	map< string, vector< pair< int, int > >	>		m_mStringInstr; // Pour chaque string en dur, contient le num�ro de l'instruction et de l'op�rande
	int						m_nCurrentScopeNumber;


	void					GenOperation( CLexAnalyser::CLexem::TLexem, const CSyntaxNode& child1, const CSyntaxNode& child2, vector< CInstr >& vAssembler );
	void					GenOperation( CLexAnalyser::CLexem::TLexem, CRegister::TType, const CSyntaxNode&, vector< CInstr >& );
	void					GenOperation( CLexAnalyser::CLexem::TLexem, CRegister::TType reg1, CRegister::TType reg2, vector< CInstr >& );
	void					GenMov( CRegister::TType, const CSyntaxNode&, vector< CInstr >& );
	void					GenPush( CRegister::TType, vector< CInstr >& );
	void					GenPush( const CSyntaxNode&, vector< CInstr >& );
	void					GenPushImm( float val, vector< CInstr >& vAssembler );
	void					GenCall( const CSyntaxNode& oNode, vector< CInstr >& );
	void					GenMov( CRegister::TType a, CRegister::TType b, vector< CInstr >& );
	void					GenMovAddrImm( CRegister::TType eBase, CRegister::TType eIndex, int nOffset, VarMap& mVar, const CSyntaxNode& oImm, vector< CInstr >& vAssembler );
	void					GenPop( CRegister::TType, vector< CInstr >& );
	void					FillOperandFromSyntaxNode( CNumeric* oOperand, const CSyntaxNode& oTree );
	void					ResolveAddresses( vector< CInstr >& vCodeOut );
	void					GenAssemblerFirstPass( const CSyntaxNode& oTree, vector< CInstr >& vCodeOut, const map<string, int>& mFuncAddr, VarMap& mVar );
};

#endif // CODEGEN_H