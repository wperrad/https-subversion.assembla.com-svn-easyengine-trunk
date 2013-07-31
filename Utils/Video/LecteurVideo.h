#pragma once

#pragma comment(lib, "vfw32.lib")
#include <windows.h>
#include <vfw.h>

#include <string>

class CLecteurVideo
{
public:
	typedef enum TEtat
	{
		Null,
		Stoping,
		Pausing,
		Playing
	};
protected:
	HWND m_hParent;
	HWND m_hWnd;
	std::string	m_sFile;
	double m_dPosition;

	int m_iLargeur, m_iHauteur;
	int m_iNbImages;
	int m_iDebit;
	double m_dDuree;

	TEtat m_Etat;

public:
	CLecteurVideo(void);
	CLecteurVideo(const CLecteurVideo& Lecteur);
	CLecteurVideo(CLecteurVideo* pLecteur);
	~CLecteurVideo(void);

	BOOL OpenVideo( const std::string& sFileName, HWND hFenetre);
	void CloseVideo(void);

	BOOL IsOpen(void) const;
	double GetDuree(void) const;
	int GetNbImages(void) const;
	int GetDebit(void) const;
	void GetTaille(int& iLargeur, int& iHauteur) const;
	void GetNomVideo( std::string& sFichier ) const;

	void Lecture(void);
	void Lecture(int iDebut, int iFin);
	void Pause(void);
	void Arret(void);

	void SetPos(double iPosition);
	double GetPos(void);

	CLecteurVideo::TEtat GetEtat(void);

	void SetVitesse(int iVitesse);
	int GetVitesse(void);
	void SetZoom(int iZoom, BOOL bPleinEcran = FALSE);
	int GetZoom(void);
	void SetRepetition(BOOL bRepet);
	BOOL GetRepetition(void);
	void SetVolume(int iVolume);
	int GetVolume(void);
};
