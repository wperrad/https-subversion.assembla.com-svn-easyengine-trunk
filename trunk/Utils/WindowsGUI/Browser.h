#include <string>

using namespace std;


class CBrowser
{
public:
	static void SelectFile( HWND hOwner, string sTitle, string sMask, string sDefaultPath, string& sOutPath );
	static void SaveFile( HWND hOwner, string sTitle, string sMask, string sDefaultPath, string& sOutPath  );
	static void SelectFolder( HWND hOwner, string sTitle, string sDefaultPath, string& sOutPath );


private:
	static int CALLBACK pBrowseCallback(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData );

	//static string	s_sDefaultPath;
};