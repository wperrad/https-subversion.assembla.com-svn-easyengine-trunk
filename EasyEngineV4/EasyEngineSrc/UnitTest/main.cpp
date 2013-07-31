// System
#include <windows.h>

// stl
#include <vector>
#include <string>

// Engine
//#include "../Utils2/Chunk.h"
//#include "../Loader2/LoaderManager.h"
//#include "../Ressource2/RessourceManager.h"
//#include "../Renderer2/Renderer.h"

using namespace std;

class IPlugin
{
public:
	virtual void UpdatePlugin() = 0;
};

class INode
{
public:
	virtual void GetLocalMatrix() = 0;
	virtual void Update() = 0;
};

class CNode : virtual public INode
{
public:
	void GetLocalMatrix(){ MessageBoxA( NULL, "CNode::GetLocalMatrix", "", MB_OK ); }
	void Update() { MessageBoxA( NULL, "CNode::Update()", "", MB_OK ); }
};

class ICamera : virtual public INode
{
public:
	virtual void Move() = 0;
};

class CLinkedCamera : virtual public ICamera, public CNode
{
public:
	void Move(){ MessageBoxA( NULL, "CLinkedCamera::Move()", "", MB_OK ); }
	void Update() { MessageBoxA( NULL, "CLinkedCamera::Update()", "", MB_OK ); }
};

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	return DefWindowProc( hWnd, msg, wParam, lParam );
}

int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
	try
	{
		CNode* pCamera = new CLinkedCamera;
		pCamera->GetLocalMatrix();
		pCamera->Update();
		ICamera* pLinkedCamera = new CLinkedCamera;
		pLinkedCamera->Move();
		pLinkedCamera->GetLocalMatrix();
		pLinkedCamera->Update();
		//pLinkedCamera->Move();
	}
	catch ( exception& e )
	{
		MessageBoxA( NULL, e.what(), "", MB_ICONERROR );
	}
	return TRUE;
}
