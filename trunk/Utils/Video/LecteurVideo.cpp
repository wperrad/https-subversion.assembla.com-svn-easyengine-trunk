#include "LecteurVideo.h"


using namespace std;

CLecteurVideo::CLecteurVideo(void)
{
	m_hWnd = NULL;
	m_hParent = NULL;
	m_dPosition = 0;

	m_Etat = Null;

	m_iLargeur = 0;
	m_iHauteur = 0;
	m_iNbImages = 0;
	m_iDebit = 1;
	m_dDuree = 0;
}

CLecteurVideo::CLecteurVideo(const CLecteurVideo& Lecteur)
{
	m_sFile = Lecteur.m_sFile;

	m_hWnd = Lecteur.m_hWnd;
	m_hParent = Lecteur.m_hParent;
	m_dPosition = Lecteur.m_dPosition;

	m_Etat = Lecteur.m_Etat;

	m_iLargeur = Lecteur.m_iLargeur;
	m_iHauteur = Lecteur.m_iHauteur;
	m_iNbImages = Lecteur.m_iNbImages;
	m_iDebit = Lecteur.m_iDebit;
	m_dDuree = Lecteur.m_dDuree;
}

CLecteurVideo::CLecteurVideo(CLecteurVideo* pLecteur)
{
	if(pLecteur)
	{
		m_sFile = pLecteur->m_sFile;

		m_hWnd = pLecteur->m_hWnd;
		m_hParent = pLecteur->m_hParent;
		m_dPosition = pLecteur->m_dPosition;

		m_Etat = pLecteur->m_Etat;

		m_iLargeur = pLecteur->m_iLargeur;
		m_iHauteur = pLecteur->m_iHauteur;
		m_iNbImages = pLecteur->m_iNbImages;
		m_iDebit = pLecteur->m_iDebit;
		m_dDuree = pLecteur->m_dDuree;
	}else
	{
		m_hWnd = NULL;
		m_hParent = NULL;
		m_dPosition = 0;

		m_Etat = Null;

		m_iLargeur = 0;
		m_iHauteur = 0;
		m_iNbImages = 0;
		m_iDebit = 1;
		m_dDuree = 0;
	}
}

CLecteurVideo::~CLecteurVideo(void)
{
	if(this->IsOpen())
		this->CloseVideo();
}

BOOL CLecteurVideo::OpenVideo( const string& strFichier, HWND hFenetre)
{
	BOOL bResult = FALSE;

	if(this->IsOpen())
		this->CloseVideo();

	m_hParent = hFenetre;
	m_sFile = strFichier;

	m_hWnd =  MCIWndCreate(hFenetre,NULL,WS_CHILD | WS_VISIBLE| MCIWNDF_NOMENU | MCIWNDF_NOPLAYBAR,NULL);


	bResult = !MCIWndOpen( m_hWnd, strFichier.c_str(),0 );
	DWORD err = GetLastError();

	if( bResult )
	{
		MCIWndSetOwner(m_hWnd,m_hParent);

		MCIWndSetActiveTimer(m_hWnd,20);

		RECT RectVid;
		MCIWndGetSource(m_hWnd,&RectVid);

		m_iLargeur = RectVid.right - RectVid.left;
		m_iHauteur = RectVid.bottom - RectVid.top;

		MCIWndUseTime(m_hWnd);
		m_dDuree = MCIWndGetLength(m_hWnd) / 1000.0;

		MCIWndUseFrames(m_hWnd);
		m_iNbImages = MCIWndGetLength(m_hWnd);

		m_iDebit = m_iNbImages / m_dDuree;

		RECT Rect;
		GetClientRect(m_hParent,&Rect);
		int iX = (Rect.right - m_iLargeur) / 2;
		int iY = (Rect.bottom - m_iHauteur) / 2;
		MoveWindow(m_hWnd,iX,iY,m_iLargeur,m_iHauteur,TRUE);
		MCIWndPutDest(m_hWnd,&Rect);

		m_Etat = Stoping;
	}
	else
	{
		MCIWndClose(m_hWnd);
		MCIWndDestroy(m_hWnd);
		m_hWnd = NULL;
		m_hParent = NULL;
	}

	return bResult;
}

void CLecteurVideo::CloseVideo(void)
{
	if( IsOpen() )
	{
		MCIWndStop(m_hWnd);
		MCIWndClose(m_hWnd);
		MCIWndDestroy(m_hWnd);
	}
	m_hWnd = NULL;
	m_hParent = NULL;
	m_dPosition = 0;

	m_Etat = Null;

	m_iLargeur = 0;
	m_iHauteur = 0;
	m_iNbImages = 0;
	m_iDebit = 0;
	m_dDuree = 0;
}

BOOL CLecteurVideo::IsOpen(void) const
{
	return (m_hWnd!=NULL);
}

void CLecteurVideo::Lecture(void)
{
	if( IsOpen() )
	{
		MCIWndPlayFrom(m_hWnd,m_dPosition);
		m_Etat = Playing;
	}
}

void CLecteurVideo::Lecture(int iDebut, int iFin)
{
	if(IsOpen())
	{
		MCIWndPlayFromTo(m_hWnd,iDebut,iFin);
		m_Etat = Playing;
	}
}

double CLecteurVideo::GetDuree(void) const
{
	return m_dDuree;
}

int CLecteurVideo::GetNbImages(void) const
{
	return m_iNbImages;
}

int CLecteurVideo::GetDebit(void) const
{
	return m_iDebit;
}

void CLecteurVideo::GetTaille(int& iLargeur, int& iHauteur) const
{
	iLargeur = m_iLargeur;
	iHauteur = m_iHauteur;
}

void CLecteurVideo::GetNomVideo( string& sFichier ) const
{
	sFichier = m_sFile;
}

void CLecteurVideo::Pause(void)
{
	if(this->IsOpen())
	{
		MCIWndPause(m_hWnd);
		m_dPosition = MCIWndGetPosition(m_hWnd);
		m_Etat = Pausing;
	}
}

void CLecteurVideo::Arret(void)
{
	if(this->IsOpen())
	{
		MCIWndStop(m_hWnd);
		m_dPosition = 0;
		m_Etat = Stoping;
	}
}

CLecteurVideo::TEtat CLecteurVideo::GetEtat(void)
{
	return m_Etat;
}

void CLecteurVideo::SetPos(double dPosition)
{
	if(this->IsOpen())
	{
		m_dPosition = dPosition;
		MCIWndSeek(m_hWnd,m_dPosition);
	}
}

double CLecteurVideo::GetPos(void)
{
	m_dPosition = MCIWndGetPosition(m_hWnd);
	return m_dPosition;
}

void CLecteurVideo::SetVitesse(int iVitesse)
{
	if(this->IsOpen())
		MCIWndSetSpeed(m_hWnd,iVitesse*10);
}

void CLecteurVideo::SetZoom(int iZoom, BOOL bPleinEcran)
{
	if(this->IsOpen())
	{
		if(bPleinEcran)
		{
			RECT Rect;
			GetClientRect(m_hParent,&Rect);
			float fRappLarg = ((float)Rect.right) / ((float)m_iLargeur);
			float fRappHaut = ((float)Rect.bottom) / ((float)m_iHauteur);
			if(fRappLarg<fRappHaut)
			{
				int iX = 0;
				int iY = (Rect.bottom - m_iHauteur*Rect.right/m_iLargeur) / 2;
				MoveWindow(m_hWnd,iX,iY,Rect.right,m_iHauteur*Rect.right/m_iLargeur,TRUE);
			}else
			{
				int iX = (Rect.right - m_iLargeur*Rect.bottom/m_iHauteur) / 2;
				int iY = 0;
				MoveWindow(m_hWnd,iX,iY,m_iLargeur*Rect.bottom/m_iHauteur,Rect.bottom,TRUE);
			}
		}else
		{
			MCIWndSetZoom(m_hWnd,iZoom);

			RECT Rect;
			GetClientRect(m_hParent,&Rect);
			int iX = (Rect.right - m_iLargeur*iZoom/100) / 2;
			int iY = (Rect.bottom - m_iHauteur*iZoom/100) / 2;
			MoveWindow(m_hWnd,iX,iY,m_iLargeur*iZoom/100,m_iHauteur*iZoom/100,TRUE);
		}
	}
}

void CLecteurVideo::SetRepetition(BOOL bRepet)
{
	if(this->IsOpen())
		MCIWndSetRepeat(m_hWnd,bRepet);
}

void CLecteurVideo::SetVolume(int iVolume)
{
	if(this->IsOpen())
		MCIWndSetVolume(m_hWnd,iVolume*10);
}

int CLecteurVideo::GetVitesse(void)
{
	int iVitesse = 1000;

	if(this->IsOpen())
		iVitesse = MCIWndGetSpeed(m_hWnd);

	return iVitesse / 10;
}

int CLecteurVideo::GetZoom(void)
{
	int iZoom = 100;

	if(this->IsOpen())
		iZoom = MCIWndGetZoom(m_hWnd);

	return iZoom;
}

BOOL CLecteurVideo::GetRepetition(void)
{
	BOOL bRepet = FALSE;

	if(this->IsOpen())
		bRepet = MCIWndGetRepeat(m_hWnd);

	return bRepet;
}

int CLecteurVideo::GetVolume(void)
{
	int iVolume = 1000;

	if(this->IsOpen())
		iVolume = MCIWndGetVolume(m_hWnd);

	return iVolume / 10;
}