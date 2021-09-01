#ifndef _ICONSOLE_H_
#define _ICONSOLE_H_

#include "EEPlugin.h"
#include <string>

class IActionManager;
class IScriptManager;
class IGUIManager;
class IInputManager;

using namespace std;

class IConsole : public CPlugin
{
protected:
	IConsole() : CPlugin( nullptr, ""){}

public:
	typedef void(*ResumeProc)(void*);


	virtual void	Update() = 0;
	virtual bool	IsOpen() = 0;
	virtual void	Open( bool bOpen ) = 0;
	virtual void	Cls() = 0;
	virtual void	Print(string s) = 0;
	virtual void	Print(int i) = 0;
	virtual void	Println(string s) = 0;
	virtual void	Println(int i) = 0;
	virtual void	NewLine() = 0;
	virtual int		GetConsoleShortCut() = 0;
	virtual void	SetConsoleShortCut(int key) = 0;
	virtual void	EnableInput(bool enable) = 0;
	virtual int		GetLineHeight() = 0;
	virtual int		GetClientHeight() = 0;
	virtual void	SetPauseModeOn(ResumeProc callback, void* params) = 0;
};

#endif // _ICONSOLE_H_