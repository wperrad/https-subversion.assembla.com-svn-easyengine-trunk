#include "BinGenerator.h"
#include "AsmGenerator.h"

int CBinGenerator::s_tabInstr[ CAsmGenerator::eMnemonicCount ][ eTypeInstrCount ][ eTypeInstrCount ] = {{{-1}}};
//int CBinGenerator::s_InstrSize[ eTypeInstrCount ][ eTypeInstrCount ] = {{-1}};
vector< int > CBinGenerator::s_vInstrSize;

void CBinGenerator::AddImmToByteArray( float nImm, vector< unsigned char >& vBin )
{
	vBin.resize( vBin.size() + 4 );
	memcpy( &vBin[ 0 ] + vBin.size() - 4, &nImm, 4 );
}

CBinGenerator::CBinGenerator()
{
	int iInstrNum = 1;
	s_tabInstr[ CAsmGenerator::eMov ][ eReg ][ eReg ] = iInstrNum++;
	s_tabInstr[ CAsmGenerator::eMov ][ eReg ][ eImm ] = iInstrNum++;
	s_tabInstr[ CAsmGenerator::eMov ][ eReg ][ eAddr ] = iInstrNum++;
	s_tabInstr[ CAsmGenerator::eMov ][ eAddr ][ eReg ] = iInstrNum++;
	s_tabInstr[ CAsmGenerator::eMov ][ eAddr ][ eImm ] = iInstrNum++;

	for( int i = CAsmGenerator::eAdd; i <= CAsmGenerator::eDiv; i++ )
	{
		s_tabInstr[ i ][ eReg ][ eReg ] = iInstrNum++;
		s_tabInstr[ i ][ eReg ][ eImm ] = iInstrNum++;
		s_tabInstr[ i ][ eReg ][ eAddr ] = iInstrNum++;
		s_tabInstr[ i ][ eImm ][ eReg ] = iInstrNum++;
		s_tabInstr[ i ][ eAddr ][ eReg ] = iInstrNum++;
		s_tabInstr[ i ][ eAddr ][ eImm ] = iInstrNum++;
	}

	s_tabInstr[ CAsmGenerator::ePush ][ eReg ][ 0 ] = iInstrNum++;
	s_tabInstr[ CAsmGenerator::ePush ][ eImm ][ 0 ] = iInstrNum++;
	s_tabInstr[ CAsmGenerator::ePush ][ eAddr ][ 0 ] = iInstrNum++;

	s_tabInstr[ CAsmGenerator::ePop ][ eReg ][ 0 ] = iInstrNum++;
	s_tabInstr[ CAsmGenerator::ePop ][ eAddr ][ 0 ] = iInstrNum++;

	s_tabInstr[ CAsmGenerator::eCall ][ eReg ][ 0 ] = iInstrNum++;
	s_tabInstr[ CAsmGenerator::eCall ][ eImm ][ 0 ] = iInstrNum++;
	s_tabInstr[ CAsmGenerator::eCall ][ eAddr ][ 0 ] = iInstrNum++;

	s_tabInstr[ CAsmGenerator::eInt ][ eReg ][ 0 ] = iInstrNum++;
	s_tabInstr[ CAsmGenerator::eInt ][ eImm ][ 0 ] = iInstrNum++;
	s_tabInstr[ CAsmGenerator::eInt ][ eAddr ][ 0 ] = iInstrNum++;

	s_tabInstr[ CAsmGenerator::eRet ][ 0 ][ 0 ] = iInstrNum++;	

	s_vInstrSize.push_back( -1 ); // pour démarrer à 1

	s_vInstrSize.push_back( 2 );
	s_vInstrSize.push_back( 6 );
	s_vInstrSize.push_back( 6 );
	s_vInstrSize.push_back( 6 );
	s_vInstrSize.push_back( 10 );

	for( int i = CAsmGenerator::eAdd; i <= CAsmGenerator::eDiv; i++ )
	{
		s_vInstrSize.push_back( 2 );
		s_vInstrSize.push_back( 6 );
		s_vInstrSize.push_back( 6 );
		s_vInstrSize.push_back( 6 );
		s_vInstrSize.push_back( 6 );
		s_vInstrSize.push_back( 10 );
	}
	
	s_vInstrSize.push_back( 2 );
	s_vInstrSize.push_back( 5 );
	s_vInstrSize.push_back( 6 );

	s_vInstrSize.push_back( 2 );
	s_vInstrSize.push_back( 6 );

	s_vInstrSize.push_back( 2 );
	s_vInstrSize.push_back( 5 );
	s_vInstrSize.push_back( 6 );

	s_vInstrSize.push_back( 2 );
	s_vInstrSize.push_back( 5 );
	s_vInstrSize.push_back( 6 );

	s_vInstrSize.push_back( 1 );
}

//int	CBinGenerator::GetInstrSize( int nInstrNum )
//{
//	return s_vInstrSize[ nInstrNum ];
//}

void CBinGenerator::GenBinary( const vector< CAsmGenerator::CInstr >& vAsmCode, vector< unsigned char >& vBin )
{
	for( unsigned int i = 0; i < vAsmCode.size(); i++ )
		GenOpcode( vAsmCode[ i ], vBin );
}


void CBinGenerator::GenOpcode( const CAsmGenerator::CInstr& oInstr, vector< unsigned char >& vBin )
{
	if( oInstr.m_vOperand.size() > 0 )
	{
		const CRegister* pReg1 = dynamic_cast< const CRegister* >( oInstr.m_vOperand[ 0 ] );
		if( pReg1 )
		{
			if( oInstr.m_vOperand.size() > 1 )
			{
				const CRegister* pReg2 = dynamic_cast< const CRegister* >( oInstr.m_vOperand[ 1 ] );
				if( pReg2 )
				{
					vBin.push_back( s_tabInstr[ oInstr.m_eMnem ][ eReg ][ eReg ] );
					int r1 = pReg1->m_eValue << 4;
					int r2 = pReg2->m_eValue;
					vBin.push_back( r1 | r2 );
				}
				else 
				{
					const CNumeric* pNum = dynamic_cast< const CNumeric* >( oInstr.m_vOperand[ 1 ] );
					if( pNum )
					{
						vBin.push_back( s_tabInstr[ oInstr.m_eMnem ][ eReg ][ eImm ] );
						vBin.push_back( pReg1->m_eValue );
						AddImmToByteArray( pNum->m_fValue, vBin );
					}
				}
			}
			else
			{
				vBin.push_back( s_tabInstr[ oInstr.m_eMnem ][ eReg ][ 0 ] );
				vBin.push_back( pReg1->m_eValue );
			}
		}
		else
		{
			const CNumeric* pNum1 = dynamic_cast< const CNumeric* >( oInstr.m_vOperand[ 0 ] );
			if( pNum1 )
			{
				if( oInstr.m_vOperand.size() > 1 )
				{
					const CRegister* pReg2 = dynamic_cast< const CRegister* >( oInstr.m_vOperand[ 1 ] );
					if( pReg2 )
					{
						vBin.push_back( s_tabInstr[ oInstr.m_eMnem ][ eImm ][ eReg ] );
						vBin.push_back( pReg2->m_eValue );
						AddImmToByteArray( pNum1->m_fValue, vBin );
					}
				}
				else
				{
					vBin.push_back( s_tabInstr[ oInstr.m_eMnem ][ eImm ][ 0 ] );
					AddImmToByteArray( pNum1->m_fValue, vBin );
				}
			}
		}
	}
	else
	{
		vBin.push_back( s_tabInstr[ oInstr.m_eMnem ][ 0 ][ 0 ] );
	}
}