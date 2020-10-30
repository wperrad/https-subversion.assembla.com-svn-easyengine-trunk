#include "Console.h"

// System
#include <windows.h>
#include <string.h>

// stl
#include <sstream>
#include <algorithm>

// Engine
#include "IInputManager.h"
#include "ActionManager.h"
#include "IScriptManager.h"
#include "Exception.h"
#include "IGUIManager.h"

using namespace std;



CConsole::CConsole( const IConsole::Desc& oDesc ):
IConsole( oDesc ),
m_bIsOpen(false),
m_xPos( 0 ),
m_yPos( 0 ),
m_oInputManager( oDesc.m_oInputManager ),
m_oGUIManager( oDesc.m_oGUIManager ),
m_oScriptManager( oDesc.m_oScriptManager ),
m_nWidth( oDesc.m_nWidth ),
m_nHeight( oDesc.m_nHeight ),
m_nCurrentLineWidth( 0 ),
m_nCurrentHeight( 0 ),
m_nCursorPos( 0 ),
m_nCursorBlinkRate( 500 ),
m_bCursorBlinkState( true ),
m_nLastMillisecondCursorStateChanged( 0 ),
m_nLastTickCount( 0 ),
m_bBlink( false ),
m_nStaticTextID( -1 ),
m_nConsoleShortCut( 0 ),
m_bHasToUpdateStaticTest(false),
m_bInputEnabled(true)
{
	m_oInputManager.AbonneToKeyEvent( static_cast< CPlugin* > ( this ), OnKeyAction );
	m_xPos = oDesc.xPos;
	m_yPos = oDesc.yPos;
	m_sLinePrefix = "> ";
	NewLine();
}

CConsole::~CConsole(void)
{
}

void CConsole::SetBlink( bool blink )
{
	m_bBlink = blink;
	if( !m_bBlink )
		m_bCursorBlinkState = true;
}

void CConsole::Open( bool bOpen )
{
	m_bIsOpen = bOpen;
	m_oInputManager.SetEditionMode( bOpen );
	m_oGUIManager.EnableStaticText( m_nStaticTextID, bOpen );
}

void CConsole::UpdateBlink( int nFontHeight )
{
	int nTickCount = GetTickCount();
	if( !m_bBlink )
		m_nLastTickCount = nTickCount;
	m_nLastMillisecondCursorStateChanged += ( nTickCount - m_nLastTickCount );
	m_nLastTickCount = nTickCount;
	if( m_nLastMillisecondCursorStateChanged > m_nCursorBlinkRate )
	{
		m_nLastMillisecondCursorStateChanged = 0;
		m_bCursorBlinkState = !m_bCursorBlinkState;
	}

	if( m_bCursorBlinkState )
	{
		int nLastIndice = m_vLines.size() - 1;
		unsigned int PixelCursorPos = ComputePixelCursorPos() - 1;
		m_oGUIManager.Print( '|', m_xPos + PixelCursorPos, m_yPos + nLastIndice * nFontHeight );
	}
}

void CConsole::Update()
{
	if ( m_bIsOpen )
	{
		int nFontHeight = m_oGUIManager.GetCurrentFontHeight();
		if (m_nStaticTextID == -1 || m_bHasToUpdateStaticTest) {
			if (m_nStaticTextID != -1)
				m_oGUIManager.DestroyStaticTest(m_nStaticTextID);
			m_nStaticTextID = m_oGUIManager.CreateStaticText(m_vLines, m_xPos, m_yPos);
			m_bHasToUpdateStaticTest = false;
		}
		m_oGUIManager.PrintStaticText( m_nStaticTextID );
		string sLine = m_sLinePrefix + m_vLines[ m_vLines.size() - 1 ];
		if(m_bInputEnabled || (sLine.size()!=2) )
			m_oGUIManager.Print( sLine, m_xPos, m_yPos + ( m_vLines.size() - 1 ) * nFontHeight );
		
		if(m_bInputEnabled)
			UpdateBlink( nFontHeight );
	}
}

unsigned int CConsole::ComputePixelCursorPos()
{
	int nLastIndice = m_vLines.size() - 1;
	string sLastLine = m_sLinePrefix + m_vLines[ nLastIndice ];
	int nCursorPosInPixel = 0;
	for( int i = 0; i < m_nCursorPos + m_sLinePrefix.size(); i++ )
		nCursorPosInPixel += m_oGUIManager.GetCurrentFontWidth( sLastLine[ i ] ) + m_oGUIManager.GetCharSpace();
	return nCursorPosInPixel;
}

void CConsole::OnKeyAction( CPlugin* pPlugin, unsigned int key, IInputManager::KEY_STATE state )
{
	CConsole* pConsole = static_cast< CConsole* >( pPlugin );
	if ( !pConsole->m_bIsOpen )
		return;
	switch( state )
	{
	case IInputManager::JUST_PRESSED:
	case IInputManager::PRESSED:
		pConsole->OnKeyPress( key );
		break;
	case IInputManager::JUST_RELEASED:
	case IInputManager::RELEASED:
		pConsole->OnKeyRelease( key );
		break;
	}
}

void CConsole::OnPressEnter()
{
	string sCommand = m_vLines.back();
	m_vLines.back() = m_sLinePrefix + m_vLines.back();
	m_vLines.resize( m_vLines.size() + 1 );
	m_nCursorPos = 0;
	try
	{
		if( sCommand.size() > 0 )
		{
			m_vLastCommand.push_back( sCommand );
			m_nCurrentCommandOffset = m_vLastCommand.size();
			m_oScriptManager.ExecuteCommand( sCommand );
		}
	}
	catch( CCompilationErrorException& e )
	{
		ostringstream ossMessage;
		string sErrorType;
		string sMessage;
		e.GetErrorMessage( sMessage );
		Println( sMessage );
	}
	catch( CScriptException& e )
	{
		AddString( "Erreur : " );
		AddString( e.what() );
	}

	if( m_nStaticTextID != -1 )
		m_oGUIManager.DestroyStaticTest( m_nStaticTextID );
	m_nStaticTextID = m_oGUIManager.CreateStaticText( m_vLines, m_xPos, m_yPos );
}

void CConsole::OnKeyPress( unsigned char key )
{
	if (!m_bInputEnabled)
		return;
	SetBlink( false );
	string& sLine = m_vLines[ m_vLines.size() - 1 ];
	if ( key == VK_BACK )
	{
		if ( sLine.size() > 0 && m_nCursorPos > 0 )
		{
			sLine.erase( sLine.begin() + m_nCursorPos - 1 );
			m_nCursorPos--;
		}
	}
	else if ( key == VK_RETURN )
		OnPressEnter();
	else if( key == VK_ESCAPE )
	{
		m_nCursorPos = 0;
		sLine.clear();
	}
	else if( key == VK_END )
	{
		m_nCursorPos = sLine.size();
	}
	else if( key == VK_HOME )
	{
		m_nCursorPos = 0;
	}
	else if( key == VK_LEFT )
	{
		if( m_nCursorPos > 0 )
			m_nCursorPos--;		
	}
	else if( key == VK_RIGHT )
	{
		if( m_nCursorPos < sLine.size() )
			m_nCursorPos++;
	}
	else if( key == VK_UP || key == VK_DOWN ) // UP || DOWN
	{
		if( m_vLastCommand.size() > 0 )
		{
			int nCurrentCommand;
			if( key == 38 )
				--m_nCurrentCommandOffset;
			else
				++m_nCurrentCommandOffset;
			if( m_nCurrentCommandOffset >= (int)m_vLastCommand.size() )
				m_nCurrentCommandOffset = m_vLastCommand.size() - 1;
			if( m_nCurrentCommandOffset < 0 )
				m_nCurrentCommandOffset = 0;
			nCurrentCommand = m_nCurrentCommandOffset;
			ReplaceString( m_vLastCommand[ nCurrentCommand ] );
			m_nCursorPos = m_vLastCommand[ nCurrentCommand ].size();
		}
	}
	else if( key == VK_DELETE )
	{
		sLine.erase( sLine.begin() + m_nCursorPos );
	}
	else if ( key != VK_SHIFT && key != VK_CONTROL && key != VK_MENU && key != m_nConsoleShortCut)
	{
		IInputManager::KEY_STATE LCtrlPressed = m_oInputManager.GetKeyState( VK_CONTROL );
		unsigned char c = 0;
		if( (LCtrlPressed == IInputManager::JUST_PRESSED || LCtrlPressed == IInputManager::PRESSED) &&
			( m_oInputManager.GetKeyState( VK_SPACE ) == IInputManager::JUST_PRESSED ))
			ManageAutoCompletion();
		else
		{
			unsigned char kbs[256];
			GetKeyboardState(kbs);
			unsigned short ch;
			ToAscii(key, MapVirtualKey(key, MAPVK_VK_TO_VSC), kbs, &ch, 0);
			c = char(ch);
		}
		if ( c != 0 )
		{
			sLine.insert( sLine.begin() + m_nCursorPos, c );
			m_nCursorPos++;
		}
	}
}

void CConsole::ManageAutoCompletion()
{
	vector< string > vFuncNames;
	m_oScriptManager.GetRegisteredFunctions( vFuncNames );
	string sBegin = m_vLines[ m_vLines.size() - 1 ];
	string sBeginLow = sBegin;
	transform( sBegin.begin(), sBegin.end(), sBeginLow.begin(), tolower );
	bool bFind = false;
	int nIndiceFind = -1;
	for( int i = 0; i < vFuncNames.size(); i++ )
	{
		string sFuncNameLow = vFuncNames[ i ];
		transform( vFuncNames[ i ].begin(), vFuncNames[ i ].end(), sFuncNameLow.begin(), tolower );
		if( sFuncNameLow.find( sBeginLow ) == 0 )
		{
			if( bFind )
			{
				bFind = false;
				break;
			}
			else
			{
				bFind = true;
				nIndiceFind = i;
			}
		}
	}
	if( bFind )
	{
		m_vLines[ m_vLines.size() - 1 ] = vFuncNames[ nIndiceFind ];
		m_nCursorPos = m_vLines[ m_vLines.size() - 1 ].size();
	}
}

void CConsole::OnKeyRelease( unsigned char key )
{
	SetBlink( true );
}

void CConsole::ReplaceString( string s , int nLine )
{
	if( nLine == -1 )
		nLine = m_vLines.size() - 1;
	m_vLines[ nLine ] = s;
}

void CConsole::Close()
{
	//m_vLines[ m_vLines.size() - 1 ] = "";
	m_oGUIManager.EnableStaticText( m_nStaticTextID, false );
}

void CConsole::AddString( string s )
{
	if( s.size() == 0 )
		return;
	string sTemp;
	int nEndlnIndex = s.find( "\n" );
	if( nEndlnIndex != -1 )
	{
		sTemp = s.substr( 0, nEndlnIndex );
		m_vLines.back().insert( m_vLines.back().end(), sTemp.begin(), sTemp.end() );
		string sQueue = s.substr( nEndlnIndex + 1 );
		if( sQueue.size() > 0 )
		{
			NewLine();
			AddString( sQueue );
		}
	}
	else
		m_vLines.back().insert( m_vLines.back().end(), s.begin(), s.end() );
}

void CConsole::NewLine()
{
	m_vLines.resize( m_vLines.size() + 1 );
	m_nCurrentHeight = m_vLines.size() * m_oGUIManager.GetCurrentFontHeight();
	m_nCurrentLineWidth = 0;
	while( m_nCurrentHeight > m_nHeight )
	{
		m_vLines.erase( m_vLines.begin() );
		m_nCurrentHeight = (int)m_vLines.size() * m_oGUIManager.GetCurrentFontHeight();
	}
	
}

bool CConsole::IsOpen()
{
	return m_bIsOpen;
}

void CConsole::Cls()
{
	m_vLines.clear();
	m_vLines.push_back("");
	m_oGUIManager.DestroyStaticTest( m_nStaticTextID );
}

void CConsole::Print( string s )
{
	AddString(s);
	m_bHasToUpdateStaticTest = true;
}

void CConsole::Println(string s)
{
	Print(s);
	NewLine();
}

int CConsole::GetConsoleShortCut()
{
	return m_nConsoleShortCut;
}

void CConsole::SetConsoleShortCut(int key)
{
	m_nConsoleShortCut = key;
}

void CConsole::EnableInput(bool enable)
{
	m_bInputEnabled = enable;
}

extern "C" _declspec(dllexport) IConsole* CreateConsole( IConsole::Desc& oDesc )
{
	return new CConsole( oDesc );
}