//#include <windows.h>
#include <io.h>
#include "easyfile.h"
#include "Exception.h"
#include "../utils2/StringUtils.h"
#include "IFileSystem.h"


using namespace std;



CEasyFile::CEasyFile() :
m_pFile(NULL),
m_bEof( false )
{
	m_bOpen = false;
}

CEasyFile::~CEasyFile(void)
{
	Close();
}


//	Ouvre le fichier szName en lecture
void CEasyFile::Open( const string& sName, IFileSystem& oFileSystem )
{
	m_sName = sName;
	if ( m_bOpen == false )
	{
		m_pFile = oFileSystem.OpenFile( m_sName, "r" );
		if ( !m_pFile )
		{
			string strMessage = string("Echec � l'ouverture de ") + m_sName;
			CFileNotFoundException e(strMessage.c_str());
			e.m_sFileName = m_sName;
			throw e;
		}
	}
	else
	{	
		string strMessage = m_sName + " d�j� ouvert";
		CEasyFileException e(strMessage.c_str());
		throw e;		

	}
	m_bOpen = true;			
}


void CEasyFile::GetWord(char szBuffer[], unsigned int nWordLen)
{
	//m_pFile->get(szBuffer,0,nWordLen);
	fread( szBuffer, nWordLen, 1, m_pFile );
}


//	Renvoie vrai si la fin du fichier a �t� atteinte, faux sinon
bool CEasyFile::Eof()
{
	return ( feof( m_pFile ) != 0 );
}


//	Ferme le fichier
void CEasyFile::Close()
{
	if ( m_bOpen )
	{
		fclose( m_pFile );
		//delete( m_pFile );
		m_pFile = NULL;
		m_bOpen = false;
	}
}


void CEasyFile::Reopen( IFileSystem& oFileSystem )
{
	string sFileName = m_sName;
	Close();
	Open( sFileName, oFileSystem );
}

//	R�cup�re dans une chaine de caract�re 'buffer' la ligne � partir du pointeur de fichier 
//	jusqu'au caract�re de fin (si ce caract�re n'est pas pr�cis�, la fonction va jusqu'a la fin de la ligne)
void CEasyFile::GetLine( string& sBuffer, char End )
{
	sBuffer.clear();
	char szBuff[2];
	unsigned char c = 0;
	
	do
	{
		fread( szBuff, 1, 1, m_pFile );
		c = szBuff[ 0 ];
		sBuffer.push_back( c );
	}
	while ( ( c != End ) && ( c != '\n' ) && ( !feof( m_pFile ) ) );
	
	if ( feof( m_pFile ) )
	{
		string strMessage = m_sName + " : end Of File";
		CEOFException e(strMessage.c_str());
		throw e;
	}
}




//	La fonction recherche le fichier dans toute l'arborescence de racine 'szCurrDir'
//	et r�cup�re le nom du r�pertoire contenant ce fichier dans szFoundDir
void CEasyFile::_SearchFile( string sFileName,const char* szRoot,string& sFoundDir,bool& bFound)
{		
	if (bFound == true)
		return;
	intptr_t hfile;
	_finddata_t file;
	string strPath = string(szRoot) + "/*.*";
	strPath = GetValidDirectoryName(strPath.c_str());
	hfile = _findfirst(strPath.c_str(),&file);

	while (_findnext(hfile,&file) == 0)
	{				
		int nTest = file.attrib & 16;
		if ( nTest != 0)	// Si le fichier trouv� est un r�pertoire
		{					
			if ( (string(file.name) != "..") && (string(file.name) != ".") ) //	Si le r�pertoire trouv� est un r�pertoire valide
			{
				string strNextDir;
				if (strlen(szRoot) == 0)
					strNextDir = string(file.name) + "/";
				else
				{
					strNextDir = string(szRoot) + "/" + file.name;
					strNextDir = GetValidDirectoryName(strNextDir.c_str());
				}
				_SearchFile( sFileName.c_str(), strNextDir.c_str(), sFoundDir, bFound );
			}
		}
		else
		{
			if ( sFileName == file.name )
			{					
				size_t i=strlen(szRoot);
				while ( (szRoot[i] != '/') && i>=0)
				{
					i--;
				}
				string strRoot = szRoot;
				sFoundDir = strRoot.substr(i+1,strRoot.length());
				bFound = true;
				return ;
			}
		}
	}
}



void CEasyFile::SearchFile(const char* szRoot,const char* szFileName, string& sFoundDir)
{
	bool bFound = false;
	sFoundDir = "";
	_SearchFile(szFileName,szRoot,sFoundDir,bFound);
	if (!bFound)
	{
		string strPath = string(szRoot) + "/" + szFileName;		
		string strMessage = strPath + " not found";
		CFileNotFoundException e(strMessage.c_str());
		e.m_sFileName = strPath;
		throw e;
	}
}



void CEasyFile::SetPointerNext( const string& sWord )
{	
	if (m_bOpen)
	{
		int nPos;	

		while ( !feof( m_pFile ) )
		{
			string sBuffer;
			GetLine( sBuffer );
			//nPos = CStringManager::GetInstance()->FindFirstOf( (char*)szBuffer,szWord);
			nPos = (int)sBuffer.find( sWord );
			if ( nPos != -1 )
			{
				int n = (int)( nPos + sWord.size() ) - (int)sBuffer.size() - 2 ;
				fseek( m_pFile, n, SEEK_CUR );
				return ;
			}
		}
		m_bEof = true;		
	}
	else
	{
		string sMessage = m_sName + " n'est pas ouvert";
		CEasyFileException e( sMessage );
		throw e;
	}
}

void CEasyFile::GetLineNext( const string& sKeyWord, string& sBuffer )
{
	SetPointerNext( sKeyWord );
	GetLine( sBuffer );
}



void CEasyFile::operator ++()
{
	//m_pFile->seekg(1,ios::cur);
	fseek( m_pFile, 1, SEEK_CUR );
}


string CEasyFile::GetValidDirectoryName(const char* szDirectoryName)
{
	string strNewRep;
	string strOldRep = szDirectoryName;
	unsigned int iOldIndice=0;

	// Eliminer tous les '/' et les espaces du d�but
	while ( (szDirectoryName[iOldIndice] == '/' || szDirectoryName[iOldIndice] == '\\' 
		|| szDirectoryName[iOldIndice] == ' ') && iOldIndice < strlen(szDirectoryName) )
		iOldIndice++;

	while (iOldIndice < strOldRep.length())
	{			
		if (strOldRep[iOldIndice] != '/' && strOldRep[iOldIndice] != '\\')
		{
			if (strOldRep[iOldIndice] != ' ')
			{
				strNewRep.push_back(strOldRep[iOldIndice]);
			}
		}
		else
		{
			if (strOldRep[iOldIndice+1] != '/' && strOldRep[iOldIndice+1] != '\\')
			{
				strNewRep.push_back(strOldRep[iOldIndice]);
			}
		}
		iOldIndice++;			
	}
	return strNewRep;
}

void CEasyFile::GetName( string& sName )
{
	sName = m_sName;
}