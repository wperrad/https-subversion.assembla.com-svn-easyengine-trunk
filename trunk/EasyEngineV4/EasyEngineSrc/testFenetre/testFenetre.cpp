// testFenetre.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "testFenetre.h"

#include "IPathFinder.h"
#include <map>
#include <string>

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


enum TGameState
{
	eInitDepDest = 0,
	eInitObstacle,
	eStarted
};

// variables
IGrid* grid = NULL;
int rowCount = 15; // 5;
int columnCount = 15; // 7;
const int CellSize = 50;
int leftCorner = 10;
int topCorner = 10;
TGameState g_currentState = eInitDepDest;
vector<IGrid::ICell*> g_vObstacles;
IPathFinder* g_pPathFinder = NULL;
COLORREF departColor = 0xffaaaa;
COLORREF destinationColor = 0xffaaaa;
COLORREF openColor = 0x00ff00;
COLORREF closeColor = 0xff;
COLORREF obstacleColor = 0x999999;
COLORREF pathColor = 0x0099ff;

map<string, CPlugin*> CPlugin::s_mPlugins;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TESTFENETRE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTFENETRE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTFENETRE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TESTFENETRE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void GetRectFromBox(int column, int row, RECT& rect)
{
	rect.left = leftCorner + column * CellSize;
	rect.right = rect.left + CellSize;
	rect.top = topCorner + row * CellSize;
	rect.bottom = rect.top + CellSize;
}

void ColorizeBox(HDC& hdc, int row, int column, COLORREF color)
{
	RECT rect;
	GetRectFromBox(row, column, rect);
	rect.left += 1;
	rect.top += 1;
	HBRUSH hbr = CreateSolidBrush(color);
	SelectObject(hdc, hbr);
	FillRect(hdc, &rect, hbr);
	DeleteObject(hbr);
}

void LabelizeBox(HDC& hdc, char* text, int backgroundColor, int column, int row)
{
	RECT rect;
	GetRectFromBox(column, row, rect);
	SetBkColor(hdc, backgroundColor);
	HFONT font = (HFONT)GetCurrentObject(hdc, OBJ_FONT);	
	TEXTMETRICA m;
	GetTextMetricsA(hdc, &m);
	TextOutA(hdc, rect.left + CellSize / 2 - m.tmAveCharWidth / 2, rect.top + CellSize / 2 - m.tmHeight / 2, text, strlen(text));
}

void LabelizeBoxCost(HDC& hdc, IGrid::ICell* cell, int backgroundColor, int column, int row)
{
	int margin = 3;
	RECT rect;
	GetRectFromBox(column, row, rect);
	SetBkColor(hdc, backgroundColor);
	HFONT font = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
	TEXTMETRICA m;
	GetTextMetricsA(hdc, &m);
	cell->GetGCost();
	char f[5], g[5], h[5];
	_itoa_s(cell->GetFCost(), f, 10);
	_itoa_s(cell->GetGCost(), g, 10);
	_itoa_s(cell->GetHCost(), h, 10);

	int hSize = strlen(h);
	int totalHSize = CellSize - m.tmAveCharWidth * strlen(h) - margin;

	TextOutA(hdc, rect.left + margin, rect.top + margin, g, strlen(g));
	TextOutA(hdc, rect.left + totalHSize, rect.top + margin, h, strlen(h));
	TextOutA(hdc, rect.left + CellSize / 2 - strlen(f) * m.tmAveCharWidth/2, rect.top + CellSize - m.tmHeight, f, strlen(f));
}

void GetCellFromCoordinates(int x, int y, int& row, int& column)
{
	column = (x - leftCorner) / CellSize;
	row = (y - topCorner) / CellSize;
	if ((column >= columnCount) || row >= rowCount) {
		row = column = -1;
	}
}



HBRUSH obstacleBrush;
POINT g_lastMouseClick;

#include <algorithm>

void ToggleObstacle(IGrid::ICell* cell)
{
	vector<IGrid::ICell*>::iterator it = std::find(g_vObstacles.begin(), g_vObstacles.end(), cell);
	int row, column;
	cell->GetCoordinates(row, column);
	if (it == g_vObstacles.end()) {
		g_vObstacles.push_back(cell);
		grid->AddObstacle(row, column);
	}
	else {
		g_vObstacles.erase(it);
		grid->RemoveObstacle(row, column);
	}
}

int MixColor(int color1, int color2, float mixFactor)
{
	int r1 = color1 & 0xff;
	int g1 = (color1 & 0xff00) >> 8;
	int b1 = (color1 & 0xff0000) >> 16;

	int r2 = color2 & 0xff;
	int g2 = (color2 & 0xff00) >> 8;
	int b2 = (color2 & 0xff0000) >> 16;

	int r = (int)(mixFactor * (float)r1 + (1.f - mixFactor) * (float)r2);
	int g = (int)(mixFactor * (float)g1 + (1.f - mixFactor) * (float)g2);
	int b = (int)(mixFactor * (float)b1 + (1.f - mixFactor) * (float)b2);
	int color = r + (g << 8) + (b << 16);
	return color;
}

void DisplayArrow(HDC& hdc, int rowDepart, int columnDepart, int rowDestination, int columnDestination)
{
	RECT rectDepart, rectDestination;
	GetRectFromBox(columnDepart, rowDepart, rectDepart);
	GetRectFromBox(columnDestination, rowDestination, rectDestination);

	int res = 10;
	int tipSize = 3 * res / 4;

	if ((rowDepart - rowDestination) > 0) {
		// haut
		if ((columnDepart - columnDestination) > 0) {
			// haut gauche
			MoveToEx(hdc, rectDepart.left + res, rectDepart.top + res, LPPOINT(NULL));
			LineTo(hdc, rectDestination.right - res, rectDestination.bottom - res);
			// tip
			MoveToEx(hdc, rectDestination.right - res, rectDestination.bottom - res, LPPOINT(NULL));
			LineTo(hdc, rectDestination.right - res, rectDestination.bottom - res + tipSize);
			MoveToEx(hdc, rectDestination.right - res, rectDestination.bottom - res, LPPOINT(NULL));
			LineTo(hdc, rectDestination.right - res + tipSize, rectDestination.bottom - res);
		}
		else if ((columnDepart - columnDestination) == 0) {
			// haut haut
			MoveToEx(hdc, rectDepart.left + CellSize / 2, rectDepart.top + res, LPPOINT(NULL));
			LineTo(hdc, rectDepart.left + CellSize / 2, rectDestination.bottom - res);
			// tip
			MoveToEx(hdc, rectDepart.left + CellSize / 2, rectDestination.bottom - res, LPPOINT(NULL));
			LineTo(hdc, rectDepart.left + CellSize / 2 - tipSize, rectDestination.bottom - res + tipSize);
			MoveToEx(hdc, rectDepart.left + CellSize / 2, rectDestination.bottom - res, LPPOINT(NULL));
			LineTo(hdc, rectDepart.left + CellSize / 2 + tipSize, rectDestination.bottom - res + tipSize);
		}
		else {
			// haut droite
			MoveToEx(hdc, rectDepart.right - res, rectDepart.top + res, LPPOINT(NULL));
			LineTo(hdc, rectDestination.left + res, rectDestination.bottom - res);
			// tip
			MoveToEx(hdc, rectDestination.left + res, rectDestination.bottom - res, LPPOINT(NULL));
			LineTo(hdc, rectDestination.left + res - tipSize, rectDestination.bottom - res);
			MoveToEx(hdc, rectDestination.left + res, rectDestination.bottom - res, LPPOINT(NULL));
			LineTo(hdc, rectDestination.left + res, rectDestination.bottom - res + tipSize);
		}
	}
	else if ((rowDepart - rowDestination) == 0) {
		if (columnDepart - columnDestination > 0) {
			// gauche gauche
			MoveToEx(hdc, rectDepart.left + res, rectDepart.top + CellSize / 2, LPPOINT(NULL));
			LineTo(hdc, rectDestination.right - res, rectDepart.top + CellSize / 2);

			// tip
			MoveToEx(hdc, rectDestination.right - res, rectDepart.top + CellSize / 2, LPPOINT(NULL));
			LineTo(hdc, rectDestination.right - res + tipSize/2, rectDepart.top + CellSize / 2 - tipSize/2);
			MoveToEx(hdc, rectDestination.right - res, rectDepart.top + CellSize / 2, LPPOINT(NULL));
			LineTo(hdc, rectDestination.right - res + tipSize/2, rectDepart.top + CellSize / 2 + tipSize/2);
		}
		else {
			// droite droite
			MoveToEx(hdc, rectDepart.right - res, rectDepart.top + CellSize / 2, LPPOINT(NULL));
			LineTo(hdc, rectDestination.left + res, rectDepart.top + CellSize / 2);

			// tip
			MoveToEx(hdc, rectDestination.left + res, rectDepart.top + CellSize / 2, LPPOINT(NULL));
			LineTo(hdc, rectDestination.left + res - tipSize/2, rectDepart.top + CellSize / 2 - tipSize / 2);
			MoveToEx(hdc, rectDestination.left + res, rectDepart.top + CellSize / 2, LPPOINT(NULL));
			LineTo(hdc, rectDestination.left + res - tipSize / 2, rectDepart.top + CellSize / 2 + tipSize / 2);
		}
	}
	else {
		// bas
		if ((columnDepart - columnDestination) > 0) {
			// bas gauche
			MoveToEx(hdc, rectDepart.left + res, rectDepart.bottom - res, LPPOINT(NULL));
			LineTo(hdc, rectDestination.right - res, rectDestination.top + res);
			// tip
			MoveToEx(hdc, rectDestination.right - res, rectDestination.top + res, LPPOINT(NULL));
			LineTo(hdc, rectDestination.right - res + tipSize, rectDestination.top + res);
			MoveToEx(hdc, rectDestination.right - res, rectDestination.top + res, LPPOINT(NULL));
			LineTo(hdc, rectDestination.right - res, rectDestination.top + res - tipSize);
		}
		else if ((columnDepart - columnDestination) == 0) {
			// bas bas
			MoveToEx(hdc, rectDepart.left + CellSize / 2, rectDepart.bottom - res, LPPOINT(NULL));
			LineTo(hdc, rectDepart.left + CellSize / 2, rectDestination.top + res);
			// tip
			MoveToEx(hdc, rectDepart.left + CellSize / 2, rectDestination.top + res, LPPOINT(NULL));
			LineTo(hdc, rectDepart.left + CellSize / 2 - tipSize / 2, rectDestination.top + res - tipSize);
			MoveToEx(hdc, rectDepart.left + CellSize / 2, rectDestination.top + res, LPPOINT(NULL));
			LineTo(hdc, rectDepart.left + CellSize / 2 + tipSize / 2, rectDestination.top + res - tipSize);
		}
		else {
			// bas droite
			MoveToEx(hdc, rectDepart.right - res, rectDepart.bottom - res, LPPOINT(NULL));
			LineTo(hdc, rectDestination.left + res, rectDestination.top + res);
			// tip
			MoveToEx(hdc, rectDestination.left + res, rectDestination.top + res, LPPOINT(NULL));
			LineTo(hdc, rectDestination.left + res, rectDestination.top + res - tipSize);
			MoveToEx(hdc, rectDestination.left + res, rectDestination.top + res, LPPOINT(NULL));
			LineTo(hdc, rectDestination.left + res - tipSize, rectDestination.top + res);
		}
	}
}

void DisplayGrid(HDC& hdc)
{
	// Draw grid
	for (int i = 0; i < grid->ColumnCount() + 1; i++) {
		MoveToEx(hdc, i * CellSize + leftCorner, topCorner, LPPOINT(NULL));
		LineTo(hdc, i * CellSize + leftCorner, topCorner + CellSize * grid->RowCount());
	}

	for (int i = 0; i < grid->RowCount() + 1; i++) {
		MoveToEx(hdc, leftCorner, topCorner + i * CellSize, LPPOINT(NULL));
		LineTo(hdc, leftCorner + grid->ColumnCount() * CellSize, topCorner + i * CellSize);
	}

	// Draw obstacles
	for (vector<IGrid::ICell*>::iterator it = g_vObstacles.begin(); it != g_vObstacles.end(); it++) {
		IGrid::ICell* cell = (*it);
		if (cell->GetCellType() & IGrid::ICell::eObstacle) {
			int row, column;
			cell->GetCoordinates(row, column);
			ColorizeBox(hdc, column, row, obstacleColor);
		}
	}

	// Draw open list
	vector<IGrid::ICell*> openList;
	grid->GetOpenList(openList);
	for (vector<IGrid::ICell*>::iterator it = openList.begin(); it != openList.end(); it++) {
		IGrid::ICell* cell = (*it);
		int row, column;
		cell->GetCoordinates(row, column);
		ColorizeBox(hdc, column, row, openColor);
		LabelizeBoxCost(hdc, cell, openColor, column, row);
	}

	// Draw close list
	vector<IGrid::ICell*> closeList;
	grid->GetCloseList(closeList);
	for (vector<IGrid::ICell*>::iterator it = closeList.begin(); it != closeList.end(); it++) {
		IGrid::ICell* cell = (*it);
		int row, column;
		cell->GetCoordinates(row, column);
		ColorizeBox(hdc, column, row, closeColor);
		LabelizeBoxCost(hdc, cell, closeColor, column, row);
	}

	// Draw path
	vector<IGrid::ICell*> vPath;
	grid->GetPath(vPath);
	for (vector<IGrid::ICell*>::iterator it = vPath.begin(); it != vPath.end(); it++) {
		IGrid::ICell* cell = (*it);
		int row, column;
		cell->GetCoordinates(row, column);
		ColorizeBox(hdc, column, row, pathColor);
		LabelizeBoxCost(hdc, cell, pathColor, column, row);
	}

	int row, column;
	// Marque Depart
	float mixFactor = 0.7f;
	IGrid::ICell* depart = grid->GetDepart();
	if (depart) {
		depart->GetCoordinates(row, column);
		int departColor2 = departColor;
		if (depart->GetCellType() & IGrid::ICell::eOpen)
			departColor2 = MixColor(departColor, openColor, mixFactor);
		else if (depart->GetCellType() & IGrid::ICell::eClose)
			departColor2 = MixColor(departColor, closeColor, mixFactor);
		ColorizeBox(hdc, column, row, departColor2);
		LabelizeBox(hdc, "D", departColor2, column, row);
		LabelizeBoxCost(hdc, depart, departColor2, column, row);
	}

	// Marque arrivee
	IGrid::ICell* destination = grid->GetDestination();
	if (destination) {
		destination->GetCoordinates(row, column);
		int destinationColor2 = destinationColor;
		if (destination->GetCellType() & IGrid::ICell::eOpen)
			destinationColor2 = MixColor(destinationColor, openColor, mixFactor);
		ColorizeBox(hdc, column, row, destinationColor2);
		LabelizeBox(hdc, "A", destinationColor2, column, row);
		LabelizeBoxCost(hdc, destination, destinationColor2, column, row);
	}
	

	// Draw arrows
	for (vector<IGrid::ICell*>::iterator it = openList.begin(); it != openList.end(); it++) {
		IGrid::ICell* cell = (*it);
		if (cell->GetParent()) {
			int r1, c1, r2, c2;
			cell->GetCoordinates(r1, c1);
			cell->GetParent()->GetCoordinates(r2, c2);
			DisplayArrow(hdc, r1, c1, r2, c2);
		}
	}
	for (vector<IGrid::ICell*>::iterator it = closeList.begin(); it != closeList.end(); it++) {
		IGrid::ICell* cell = (*it);
		if (cell->GetParent()) {
			int r1, c1, r2, c2;
			cell->GetCoordinates(r1, c1);
			cell->GetParent()->GetCoordinates(r2, c2);
			DisplayArrow(hdc, r1, c1, r2, c2);
		}
	}

}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int x, y, row, column;
	RECT rect;
    switch (message)
    {
	case WM_CREATE:
	{
		string sDirectoryName;
#ifdef _DEBUG
		sDirectoryName = "..\\..\\EasyEngine\\Debug\\";
#else
		sDirectoryName = "..\\..\\EasyEngine\\release\\";
#endif
		IPathFinder::Desc desc(NULL, "PathFinder");
		g_pPathFinder = (IPathFinder*)CPlugin::Create(desc, sDirectoryName + "IA.dll", "CreatePathFinder");
		grid = g_pPathFinder->CreateGrid(rowCount, columnCount);
		obstacleBrush = CreateSolidBrush(obstacleColor);
		break;
	}
	case WM_LBUTTONDOWN:
		x = (lParam & 0x0000ffff);
		y = lParam >> 16;

		GetCellFromCoordinates(x, y, row, column);
		if (row != -1 && column != -1) {
			switch (g_currentState) {
			case eInitDepDest:
				if (!grid->GetDepart())
					grid->SetDepart(column, row);
				else {
					grid->SetDestination(column, row);
					g_currentState = eInitObstacle;
				}
				break;
			case eInitObstacle:
			{
				int r, c;
				grid->GetDepart()->GetCoordinates(r, c);
				if ((c == column) && (r == row)) {
					g_currentState = eStarted;
					g_pPathFinder->FindPath(grid);
				}
				else {
					ToggleObstacle(&grid->GetCell(row, column));
				}
				break;
			}
			case eStarted:
				grid->Reset();
				g_vObstacles.clear();
				g_currentState = eInitDepDest;
				break;
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		
		ShowWindow(hWnd, 10);
		UpdateWindow(hWnd);
		break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			DisplayGrid(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
