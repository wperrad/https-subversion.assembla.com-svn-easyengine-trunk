#include "AsmGenerator.h"
#include "LexAnalyser.h"
#include "SyntaxAnalyser.h"
#include "ScriptException.h"

#include <sstream>
#include <algorithm>

map< CRegister::TType, string > CRegister::s_mRegisterToString;

void CRegister::InitRegisterToStringMap()
{
	s_mRegisterToString[ eax ] = "eax";
	s_mRegisterToString[ ebx ] = "ebx";
	s_mRegisterToString[ ecx ] = "ecx";
	s_mRegisterToString[ edx ] = "edx";
	s_mRegisterToString[ esi ] = "esi";
	s_mRegisterToString[ edi ] = "edi";
	s_mRegisterToString[ ebp ] = "ebp";
	s_mRegisterToString[ esp ] = "esp";
}

void CRegister::GetStringName( string& sName )
{ 
	sName = s_mRegisterToString[ m_eValue ]; 
}

void CPreprocessorString::GetStringName( string& sName )
{
	sName = string( "\"" ) + m_sValue + "\", 0";
}

void CNumeric::GetStringName( string& sName )
{
	ostringstream oss;
	oss << m_fValue;
	sName = oss.str();
}

void CMemory::GetStringName( string& sName )
{
	string sBase, sIndex;
	if( m_oBase.m_eValue != CRegister::eNone )
		m_oBase.GetStringName( sBase );
	if( m_oIndex.m_eValue != CRegister::eNone )
		m_oIndex.GetStringName( sIndex );

	ostringstream oss;
	oss << "[ ";
	if( sBase.size() > 0 )
		oss << sBase;
	if( sIndex.size() > 0 )
	{
		if( sBase.size() > 0 )
			oss << " + " << sIndex;
		else
			oss << sIndex;
	}
		
	if( m_nOffset != 0 )
	{
		if( sBase.size() > 0 || sIndex.size() > 0 )
		{
			if( m_nOffset > 0 )
				oss << " + " << m_nOffset;
			else
				oss << " - " << -m_nOffset;
		}
		else
			oss << m_nOffset;
	}
	oss << " ]";
	sName = oss.str();
}

CAsmGenerator::CAsmGenerator():
m_nCurrentScopeNumber( 0 )
{
	m_mTypeToMnemonic[ CLexAnalyser::CLexem::eAdd ] = eAdd;
	m_mTypeToMnemonic[ CLexAnalyser::CLexem::eSub ] = eSub;
	m_mTypeToMnemonic[ CLexAnalyser::CLexem::eMult ] = eMul;
	m_mTypeToMnemonic[ CLexAnalyser::CLexem::eDiv ] = eDiv;

	m_mMnemonicToString[ eMov ] = "mov";
	m_mMnemonicToString[ eAdd ] = "add";
	m_mMnemonicToString[ eMul ] = "mul";
	m_mMnemonicToString[ eSub ] = "sub";
	m_mMnemonicToString[ eDiv ] = "div";
	m_mMnemonicToString[ eCall ] = "call";
	m_mMnemonicToString[ ePush ] = "push";
	m_mMnemonicToString[ ePop ] = "pop";
	m_mMnemonicToString[ eInt ] = "int";
	m_mMnemonicToString[ eDB ] = "db";
	m_mMnemonicToString[ eRet ] = "ret";
	CRegister::InitRegisterToStringMap();
}

void CAsmGenerator::CreateAssemblerListing( vector< CInstr >& vCodeOut, string sFileName )
{
	FILE* pFile = NULL;
	fopen_s( &pFile, sFileName.c_str(), "w" );
	ostringstream oss;
	for( unsigned int i = 0; i < vCodeOut.size(); i++ )
	{
		oss << m_mMnemonicToString[ vCodeOut[ i ].m_eMnem ] << " ";
		for( unsigned int j = 0; j < vCodeOut[ i ].m_vOperand.size(); j++ )
		{
			string s;
			vCodeOut[ i ].m_vOperand[ j ]->GetStringName( s );
			oss << s;
			if( j < vCodeOut[ i ].m_vOperand.size() - 1 )
				oss << ", ";
			else
				oss << "\n";
		}
		if( vCodeOut[ i ].m_vOperand.size() == 0 )
			oss << "\n";
	}
	fwrite( oss.str().c_str(), sizeof( char ), oss.str().size(), pFile );
	fclose( pFile );
}

void CAsmGenerator::FillOperandFromSyntaxNode( CNumeric* pOperand, const CSyntaxNode& oTree )
{
	switch ( oTree.m_Lexem.m_eType )
	{
	case CLexAnalyser::CLexem::eFloat:
		pOperand->m_fValue = oTree.m_Lexem.m_fValue;
		break;
	case CLexAnalyser::CLexem::eInt:
		pOperand->m_fValue = oTree.m_Lexem.m_nValue;
		break;
	case CLexAnalyser::CLexem::eString:
		pOperand->m_fValue = oTree.m_nAddress;
		break;
	}	
}

void CAsmGenerator::GenAssembler( const CSyntaxNode& oTree, vector< CInstr >& vCodeOut, const map<string, int>& mFuncAddr, VarMap& mVar )
{
	GenAssemblerFirstPass( oTree, vCodeOut, mFuncAddr, mVar );
	CInstr oIntr;
	oIntr.m_eMnem = eRet;
	vCodeOut.push_back( oIntr );
	ResolveAddresses( vCodeOut );
}

void CAsmGenerator::GenAssemblerFirstPass( const CSyntaxNode& oTree, vector< CInstr >& vAssembler, const map<string, int>& mFuncAddr, VarMap& mVar )
{
	if( oTree.m_Lexem.IsOperation() )
	{
		if( oTree.m_vChild[ 0 ].m_vChild.size() == 0 && oTree.m_vChild[ 1 ].m_vChild.size() == 0 )
			GenOperation( oTree.m_Lexem.m_eType, oTree.m_vChild[ 0 ], oTree.m_vChild[ 1 ], vAssembler );
		else 
		{
			bool eaxBusy = false;
			if( oTree.m_vChild[ 0 ].m_vChild.size() > 0 )
			{
				GenAssemblerFirstPass(oTree.m_vChild[ 0 ], vAssembler, mFuncAddr, mVar );
				eaxBusy = true;
			}
			if( oTree.m_vChild[ 1 ].m_vChild.size() == 0 )
			{
				if( !eaxBusy )
					GenMov( CRegister::eax, oTree.m_vChild[ 0 ], vAssembler );
				GenOperation( oTree.m_Lexem.m_eType, CRegister::eax, oTree.m_vChild[ 1 ], vAssembler );
			}
			else
			{
				if( eaxBusy )
				{
					GenPush( CRegister::eax, vAssembler );
					GenAssemblerFirstPass( oTree.m_vChild[ 1 ], vAssembler, mFuncAddr, mVar );
					GenMov( CRegister::ebx, CRegister::eax, vAssembler );
					GenPop( CRegister::eax, vAssembler );
					GenOperation( oTree.m_Lexem.m_eType, CRegister::eax, CRegister::ebx, vAssembler );
				}
				else
				{
					GenAssemblerFirstPass(oTree.m_vChild[ 1 ], vAssembler, mFuncAddr, mVar );
					GenMov( CRegister::ebx, CRegister::eax, vAssembler );
					GenMov( CRegister::eax, oTree.m_vChild[ 0 ], vAssembler );
					GenOperation( oTree.m_Lexem.m_eType, CRegister::eax, CRegister::ebx, vAssembler );
				}
			}
		}
	}
	else if( oTree.m_Lexem.m_eType == CLexAnalyser::CLexem::eFunction )
	{
		for( unsigned int i = 0; i < oTree.m_vChild.size(); i++ )
		{
			int nIndex = (int)oTree.m_vChild.size() - i - 1;
			if( oTree.m_vChild[ nIndex ].m_Lexem.IsNumeric() )
				GenPush( oTree.m_vChild[ nIndex ], vAssembler );
			else
			{
				GenAssemblerFirstPass( oTree.m_vChild[ nIndex ], vAssembler, mFuncAddr, mVar );
				GenPush( CRegister::eax, vAssembler );
			}
		}
		GenCall( oTree, vAssembler );
	}
	else if( oTree.m_Lexem.m_eType == CLexAnalyser::CLexem::eAffect )
	{
		CVar& v = mVar[ m_nCurrentScopeNumber ][ oTree.m_vChild[ 0 ].m_Lexem.m_sValue ];
		if( !v.m_bIsDeclared )
		{
			GenPushImm( 0, vAssembler );
			v.m_bIsDeclared = true;
		}
		if( oTree.m_vChild[ 1 ].m_vChild.size() == 0 )
			GenMovAddrImm( CRegister::ebp, CRegister::TType::eNone, 4 * ( v.m_nScopePos - 1 ), mVar, oTree.m_vChild[ 1 ], vAssembler );
		else
			throw 1;
	}
	else
	{
		for( unsigned int i = 0; i < oTree.m_vChild.size(); i++ )
			GenAssemblerFirstPass( oTree.m_vChild[ i ], vAssembler, mFuncAddr, mVar );
	}
}

void CAsmGenerator::ResolveAddresses( vector< CInstr >& vCodeOut )
{
#if STRING_IN_BIN
	map< string, vector< pair< int, int > > >::iterator itString = m_mStringInstr.begin();
	for ( ; itString != m_mStringInstr.end(); ++itString )
	{
		for( unsigned int i = 0; i < itString->second.size(); i++ )
		{
			pair< int, int >& p = itString->second[ i ];
			CNumeric* pStringAddress = static_cast< CNumeric* >( vCodeOut[ p.first ].m_vOperand[ p.second ] );
			pStringAddress->m_fValue = vCodeOut.size();
		}
		CInstr oInstr;
		oInstr.m_eMnem = eDB;
		CPreprocessorString* pString = new CPreprocessorString( itString->first );
		oInstr.m_vOperand.push_back( pString );
		vCodeOut.push_back( oInstr );
	}
#endif // 0
}

void CAsmGenerator::GenCall( const CSyntaxNode& oNode, vector< CInstr >& vAssembler )
{
	CInstr oInstr;
	if( oNode.m_Type == CSyntaxNode::eAPIFunction )
		oInstr.m_eMnem = CAsmGenerator::eInt;
	else
		oInstr.m_eMnem = CAsmGenerator::eCall;
	CNumeric* pNumeric = new CNumeric( oNode.m_nAddress );
	oInstr.m_vOperand.push_back( pNumeric );
	vAssembler.push_back( oInstr );
}

void CAsmGenerator::GenPop( CRegister::TType reg, vector< CInstr >& vAssembler )
{
	CInstr oInstr;
	oInstr.m_eMnem = CAsmGenerator::ePop;
	CRegister* pRegister = new CRegister( reg );
	oInstr.m_vOperand.push_back( pRegister );
	vAssembler.push_back( oInstr );
}


void CAsmGenerator::GenMov( CRegister::TType reg, const CSyntaxNode& oNode, vector< CInstr >& vAssembler )
{
	CInstr oInstr;
	oInstr.m_eMnem = CAsmGenerator::eMov;
	CRegister* pReg = new CRegister( reg );
	CNumeric* pNum = new CNumeric;
	FillOperandFromSyntaxNode( pNum, oNode );
	oInstr.m_vOperand.push_back( pReg );
	oInstr.m_vOperand.push_back( pNum );
	vAssembler.push_back( oInstr );
	if( oNode.m_Lexem.m_eType == CLexAnalyser::CLexem::eString )
	{
		m_mStringInstr[ oNode.m_Lexem.m_sValue ].push_back( pair< int, int >::pair( (int)vAssembler.size() - 1, 1 ) );
	}
}

void CAsmGenerator::GenMov( CRegister::TType a, CRegister::TType b, vector< CInstr >& vAssembler )
{
	CInstr oInstr;
	oInstr.m_eMnem = CAsmGenerator::eMov;
	CRegister* pRegister1 = new CRegister( a );
	CRegister* pRegister2 = new CRegister( b );
	oInstr.m_vOperand.push_back( pRegister1 );
	oInstr.m_vOperand.push_back( pRegister2 );
	vAssembler.push_back( oInstr );
}

void CAsmGenerator::GenMovAddrImm( CRegister::TType eBase, CRegister::TType eIndex, int nOffset, VarMap& mVar, const CSyntaxNode& oImm, vector< CInstr >& vAssembler )
{
	CInstr oInstr;
	oInstr.m_eMnem = CAsmGenerator::eMov;

	CRegister r0( eBase ), r1( eIndex );
	CMemory* pMem = new CMemory;
	pMem->m_oBase = r0;
	pMem->m_oIndex = r1;
	pMem->m_nOffset = nOffset;
	
	CNumeric* pNum = new CNumeric;
	FillOperandFromSyntaxNode( pNum, oImm );

	oInstr.m_vOperand.push_back( pMem );
	oInstr.m_vOperand.push_back( pNum );
	vAssembler.push_back( oInstr );
}

void CAsmGenerator::GenPush( const CSyntaxNode& oNode, vector< CInstr >& vAssembler )
{
	CInstr oInstr;
	oInstr.m_eMnem = CAsmGenerator::ePush;
	CNumeric* pNumeric = new CNumeric;
	FillOperandFromSyntaxNode( pNumeric, oNode );
	oInstr.m_vOperand.push_back( pNumeric );
	vAssembler.push_back( oInstr );
	if( oNode.m_Lexem.m_eType == CLexAnalyser::CLexem::eString )
	{
		m_mStringInstr[ oNode.m_Lexem.m_sValue ].push_back( pair< int, int >::pair( (int)vAssembler.size() - 1, 0 ) );
	}
}

void CAsmGenerator::GenPush( CRegister::TType reg, vector< CInstr >& vAssembler )
{
	CInstr oInstr;
	oInstr.m_eMnem = CAsmGenerator::ePush;
	CRegister* pRegister = new CRegister( reg );
	oInstr.m_vOperand.push_back( pRegister );
	vAssembler.push_back( oInstr );
}

void CAsmGenerator::GenPushImm( float val, vector< CInstr >& vAssembler )
{
	CInstr oInstr;
	oInstr.m_eMnem = CAsmGenerator::ePush;
	CNumeric* pNumeric = new CNumeric( val );
	oInstr.m_vOperand.push_back( pNumeric );
	vAssembler.push_back( oInstr );
}

void CAsmGenerator::GenOperation( CLexAnalyser::CLexem::TLexem optype, CRegister::TType reg1, CRegister::TType reg2, vector< CInstr >& vAssembler )
{
	CInstr oInstr;
	CRegister* pReg1 = new CRegister( reg1 );
	CRegister* pReg2 = new CRegister( reg2 );
	oInstr.m_vOperand.push_back( pReg1 );
	oInstr.m_vOperand.push_back( pReg2 );
	oInstr.m_eMnem = m_mTypeToMnemonic[ optype ];
	vAssembler.push_back( oInstr );
}

void CAsmGenerator::GenOperation( CLexAnalyser::CLexem::TLexem optype, const CSyntaxNode& child1, const CSyntaxNode& child2, vector< CInstr >& vAssembler )
{
	CInstr oInstr;
	GenMov( CRegister::eax, child1, vAssembler );
	CRegister* pOp1 = new CRegister( CRegister::eax );
	CNumeric* pOp2 = new CNumeric();
	FillOperandFromSyntaxNode( pOp2, child2 );
	oInstr.m_vOperand.push_back( pOp1 );
	oInstr.m_vOperand.push_back( pOp2 );
	oInstr.m_eMnem = m_mTypeToMnemonic[ optype ];
	vAssembler.push_back( oInstr );
}

void CAsmGenerator::GenOperation( CLexAnalyser::CLexem::TLexem optype, CRegister::TType reg, const CSyntaxNode& oNode, vector< CInstr >& vAssembler )
{
	CInstr oInstr;
	oInstr.m_eMnem = m_mTypeToMnemonic[ optype ];
	CRegister* pRegister = new CRegister( reg );
	CNumeric* pNumeric = new CNumeric;
	FillOperandFromSyntaxNode( pNumeric, oNode );
	oInstr.m_vOperand.push_back( pRegister );
	oInstr.m_vOperand.push_back( pNumeric );
	vAssembler.push_back( oInstr );
}