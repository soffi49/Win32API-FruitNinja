// FruitNinja.cpp : Defines the entry point for the application.
//

#pragma comment(lib, "Msimg32.lib")
#include "framework.h"
#include "FruitNinja.h"
#include <vector>

using namespace std;

#define MAX_LOADSTRING 100


/*
Resources used:

1.https://sourceforge.net/p/win32loopl/code/ci/default/tree/LooplessSizeMove.c?fbclid=IwAR1HR2KdMafDRNC6m-NtK3TOrGA5FJIBVfgLu89yrITJcS2P5AlbW-g41u4#l344
2.https://stackoverflow.com/questions/10975180/using-alphablend-to-draw-slightly-transparent-rectangle-fails?fbclid=IwAR33VqVW_OcUONW-DK1lbo_zseGXmJgakva78UipgUHcQ0UqiYiqNoor63U

And in general:
-https://docs.microsoft.com/pl-pl/windows/win32/

*/

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HCURSOR cursor = NULL;
HDC hdc;
HDC offDC;
static HBITMAP offOldBitmap = NULL; 
static HBITMAP offBitmap = NULL;
LPCTSTR path = _T(".\\fruitninja.ini");
HMENU hMenu;
double elapsed = 0;             //time since the game startes
int start;                      //time when game starts
int points = 0;                 //number of points
static vector<POINT> line;      //saves points during mouse movement to create polychain
static vector<balls> fruits;    
POINT mousept;                  //single mouse point
double mousedx, mousedy;        //"speed" of the mouse 

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void resizeWindow(HWND hWnd, int x, int y); 
void makeBackground(HWND hWnd);
void makeFruit(balls fruit);
void makeProgress(HWND hWnd);
void makeCounter(HWND hWnd);
void endGame(HWND hWnd,RECT rc);
void makeMouse(HWND hWnd);
void selection(HMENU hMenu,int opt);
void isIntersecting(HWND hWnd, POINT point, vector<balls>* newpoints);
void newGame(HWND hWnd);

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
    LoadStringW(hInstance, IDC_FRUITNINJA, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FRUITNINJA));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FRUITNINJA);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));


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

   RECT rc;
   TCHAR boardT[2];
   int temp = GetPrivateProfileString(_T("BOARD"),_T("SIZE"),_T(""),boardT,2,path);
   int board = _ttoi(boardT);

   if (GetLastError() == 0x2)
   {
       board = 1;
       WritePrivateProfileString(_T("BOARD"), _T("SIZE"), _T("1"), path);
   }

   switch (board)
   {
   case 1:
       rc.right = 8*SQUARE_SIZE;
       rc.bottom =  6*SQUARE_SIZE + PROGRESS_BAR_THICK;
       break;
   case 2:
       rc.right = 12*SQUARE_SIZE;
       rc.bottom = 10*SQUARE_SIZE + PROGRESS_BAR_THICK;
       break;
   case 3:
       rc.right = 16 * SQUARE_SIZE;
       rc.bottom = 12 * SQUARE_SIZE + PROGRESS_BAR_THICK;
       break;
   default:
       break;
   }

   rc.left = 0;
   rc.top = 0;

   AdjustWindowRect(&rc, WS_CAPTION | WS_SYSMENU |WS_OVERLAPPED , true);

   int width = rc.right-rc.left;
   int height = rc.bottom-rc.top;

   GetClientRect(GetDesktopWindow(), &rc);

   int posx = (rc.right / 2) - (width / 2);
   int posy = (rc.bottom / 2) - (height / 2);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_SYSMENU|WS_OVERLAPPED|WS_CAPTION|WS_CLIPCHILDREN,
      posx, posy, width, height, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   hMenu=GetMenu(hWnd);
   selection(hMenu, board);

   SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    hMenu = GetMenu(hWnd);
 
    switch (message)
    {
    case WM_SIZE:
    {
        int cw = LOWORD(lParam);
        int ch = HIWORD(lParam);

        hdc = GetDC(hWnd);                                          //getting device context
        if (offOldBitmap != NULL)                                   
        {
            SelectObject(offDC, offOldBitmap);                      //select bitmap to dc
        }
        if (offBitmap != NULL)
        {
            DeleteObject(offBitmap);                               
        }
        offBitmap = CreateCompatibleBitmap(hdc, cw, ch);           //creates bitmap for hdc
        offOldBitmap = (HBITMAP)SelectObject(offDC, offBitmap);    //selects offbitmap as current
        ReleaseDC(hWnd, hdc);                                      //free a dc
    }
    break;

    case WM_SYSCOMMAND: //bibliography 1.
    {
        switch (wParam & 0xfff0)
        {
        case SC_MOVE: break;
        case SC_SIZE: break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }

    case WM_CREATE:
    {
        hdc = GetDC(hWnd);                                 
        offDC = CreateCompatibleDC(hdc);                        //creating compatible dc to hdc
        ReleaseDC(hWnd, hdc);                                   //releasing hdc
        cursor = LoadCursor(hInst, (LPCWSTR)IDC_CURSOR2);
        start = GetTickCount64();
        SetTimer(hWnd, MOVEMENT, MOVEMENT_T, NULL);
        SetTimer(hWnd, FREQUENCY, FREQUENCY_T, NULL);
        SetTimer(hWnd, END, END_T, NULL);
        SetTimer(hWnd, PROGRESS, PROGRESS_T, NULL);   
    }

    case WM_MOUSEMOVE:
    {
        KillTimer(hWnd, MOUSE);
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        UpdateWindow(hWnd);

        if (elapsed < 30)
        {
            mousept.x = LOWORD(lParam);
            mousept.y = HIWORD(lParam);
            line.push_back(mousept);

            if (line.size() > 2)
            {
                mousedx = (double)line[line.size() - 1].x - line[line.size() - 2].x;
                mousedy = (double)line[line.size() - 1].y - line[line.size() - 2].y;
            }
        }

        SetTimer(hWnd, MOUSE, MOUSE_T, NULL);
    }
    break;


    case WM_TIMER:
    {
        if (wParam == MOUSE)
        {
            SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, (255*50)/100, LWA_ALPHA);
            UpdateWindow(hWnd);
        }

        else if (wParam == MOVEMENT)
        {
            vector<balls> newpoints;

                isIntersecting(hWnd, mousept, &newpoints);

                if (!newpoints.empty())
                {
                    fruits.clear();
                    fruits = newpoints;
                }

            double height = (double)rc.bottom - rc.top;
            double gravity = height / 20000;

            if (fruits.empty() == false)
            {
                for (auto fruit = fruits.begin(); fruit != fruits.end(); ) 
                {
                    (*fruit).x += (*fruit).dx;
                    (*fruit).y += (*fruit).dy;
                    (*fruit).dy = (*fruit).dy+gravity;

                    if ((*fruit).y > rc.bottom) fruit=fruits.erase(fruit);
                    else ++fruit;
                    
                }
                InvalidateRect(hWnd, NULL, TRUE);
            }

        }

        else if (wParam == FREQUENCY)
        {
            double height = (double)rc.bottom - rc.top;
            double width = (double)rc.right - rc.left;

            double dx = ((double)(rand() % 5) - 2)/5;
            double dy = (double)-height / 100;
            double x = rand() % ((int)width-2*FRUIT_SIZE);
            double y = (double)rc.bottom;

            balls temp(FRUIT_SIZE, x, y, dx, dy);
            fruits.push_back(temp);
        }

        else if (wParam == PROGRESS)
        {
            if (elapsed <= 1 / 1000) line.clear();

            elapsed= ((double)(GetTickCount64() - start)) / 1000;
            InvalidateRect(hWnd,&rc, TRUE);
        }
        else if (wParam == END)
        {
            KillTimer(hWnd, MOVEMENT);
            KillTimer(hWnd, FREQUENCY);
            KillTimer(hWnd, PROGRESS);
            InvalidateRect(hWnd,NULL,TRUE);
        }

    }break;

    case WM_ERASEBKGND: //from tutorial -to stop flickering
        return 1;

    case WM_SETCURSOR:
         SetCursor(cursor);
         return TRUE;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ID_BOARD_SMALL:
            {
                selection(hMenu, 1);
                newGame(hWnd);
                resizeWindow(hWnd, 6, 8);
                WritePrivateProfileString(_T("BOARD"), _T("SIZE"), _T("1"), path);
            }
            break;
            case ID_BOARD_MEDIUM:
            {
                selection(hMenu, 2);
                newGame(hWnd);
                resizeWindow(hWnd, 10, 12);
                WritePrivateProfileString(_T("BOARD"), _T("SIZE"), _T("2"), path);
            }
            break;
            case ID_BOARD_BIG:
            {
                selection(hMenu, 3);
                newGame(hWnd);
                resizeWindow(hWnd, 12, 16);
                WritePrivateProfileString(_T("BOARD"), _T("SIZE"), _T("3"), path);
            }
            break;
            case ID_GAME_NEWGAME:
            {
                newGame(hWnd);
            }
            break;

            case IDM_EXIT:
                KillTimer(hWnd, MOVEMENT);
                KillTimer(hWnd, FREQUENCY);
                KillTimer(hWnd, PROGRESS);
                KillTimer(hWnd, END);
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
        hdc = BeginPaint(hWnd, &ps);
        makeBackground(hWnd);

        for (auto& fruit : fruits)
            makeFruit(fruit);

        makeProgress(hWnd);
        makeCounter(hWnd);

        makeMouse(hWnd);

        if (elapsed >= 30)
            endGame(hWnd, rc);

        BitBlt(hdc, 0, 0, rc.right, rc.bottom, offDC, 0, 0, SRCCOPY); //transfering color data from offDC to hdc
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

void resizeWindow(HWND hWnd,int y,int x)
{
    RECT rc;

    rc.left = 0;
    rc.right = SQUARE_SIZE*x;
    rc.top = 0;
    rc.bottom = SQUARE_SIZE*y+PROGRESS_BAR_THICK;

    AdjustWindowRect(&rc, WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED, true);

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    GetClientRect(GetDesktopWindow(), &rc);

    int posx = (rc.right / 2) - (width / 2);
    int posy = (rc.bottom / 2) - (height / 2);

    SetWindowPos(hWnd,0, posx, posy, width, height, SWP_NOZORDER);
    InvalidateRect(hWnd, NULL, TRUE);

}

void makeBackground(HWND hWnd)
{
    HBRUSH brushBlack = CreateSolidBrush(RGB(0, 0, 0));
    HBRUSH brushWhite = CreateSolidBrush(RGB(255, 255, 255));
    RECT rc;
    GetClientRect(hWnd, &rc);
    FillRect(offDC, &rc, brushWhite);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top-PROGRESS_BAR_THICK;

    for (int i = 0; i < height; i += SQUARE_SIZE)
    {
        for (int j = 0; j <width; j += (2*SQUARE_SIZE))
        {
            if ((i / SQUARE_SIZE) % 2 == 0)
            {
                SetRect(&rc, j, i, j + SQUARE_SIZE, i + SQUARE_SIZE);
                FillRect(offDC, &rc, brushBlack);
            }
            else
            {
                SetRect(&rc, j + SQUARE_SIZE, i, j + 2*SQUARE_SIZE, i + SQUARE_SIZE);
                FillRect(offDC, &rc, brushBlack);
            }
        }
    }

    DeleteObject(brushBlack);
    DeleteObject(brushWhite);
}

void makeCounter(HWND hWnd)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    TCHAR s[50];
    _stprintf_s(s, 50, _T("%d"), points);

    SetTextColor(offDC, RGB(0, 220, 0));
    SetBkMode(offDC, TRANSPARENT);

    HFONT font = CreateFont(-MulDiv(24, GetDeviceCaps(offDC, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,_T(" Verdana "));
    HFONT oldFont = (HFONT)SelectObject(offDC, font);
    DrawText(offDC, s, (int)_tcslen(s), &rc, DT_RIGHT | DT_TOP|DT_SINGLELINE);
    SelectObject(offDC, oldFont);
    DeleteObject(font);
}

void makeMouse(HWND hWnd)
{
    RECT rc;
    GetClientRect(hWnd,&rc);

    HPEN pen = CreatePen(PS_SOLID, 4, RGB(255,255,51));
    HPEN oldPen = (HPEN)SelectObject(offDC, pen);

    POINT position;
    GetCursorPos(&position);
    ScreenToClient(hWnd, &position);

    if (!line.empty())
    {
        if (position.x >= rc.right || position.x <= rc.left ||
            position.y <= rc.top || position.y >= rc.bottom)
        {
            line.clear();
        }
        else
        {
            POINT* points = &line[0];
            Polyline(offDC, points, line.size());
        }
    }

    if(line.size()>10) line.clear();
    DeleteObject(pen);
}

void makeFruit(balls fruit)
{
    HPEN pen = CreatePen(PS_SOLID, 1, fruit.color);
    HBRUSH brush = CreateSolidBrush(fruit.color);
    HPEN oldPen = (HPEN)SelectObject(offDC, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(offDC, brush);

    Ellipse(offDC, fruit.x, fruit.y, fruit.x + (double)fruit.size * 2, fruit.y + (double)fruit.size * 2);

    SelectObject(offDC, oldBrush);
    DeleteObject(brush);
    DeleteObject(pen);
}

void makeProgress(HWND hWnd)
{
    HBRUSH brushGreen = CreateSolidBrush(RGB(0, 200, 0));
    RECT rc;
    GetClientRect(hWnd, &rc);

    double width = (double)((double)rc.right - rc.left);
    width = width / 30;

    SetRect(&rc,0, rc.bottom-PROGRESS_BAR_THICK, elapsed* width,rc.bottom);
    FillRect(offDC, &rc, brushGreen);
    DeleteObject(brushGreen);
}

void endGame(HWND hWnd, RECT rc) //bibliography 2.
{
    HDC tempHdc = CreateCompatibleDC(offDC);
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 127, 0 };

    HBITMAP hbitmap;
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = rc.right - rc.left;
    bmi.bmiHeader.biHeight = rc.bottom - rc.top;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = (rc.right - rc.left) * (rc.bottom - rc.top) * 4;
    hbitmap = (HBITMAP)CreateDIBSection(tempHdc, &bmi, DIB_RGB_COLORS, 0, NULL, 0x0);
    HBITMAP oldBitmap=(HBITMAP)SelectObject(tempHdc, hbitmap);

    SetDCPenColor(tempHdc, RGB(0, 0, 255));
    SetDCBrushColor(tempHdc, RGB(0, 0, 255));
    FillRect(tempHdc, &rc, CreateSolidBrush(RGB(0, 220, 0)));
    AlphaBlend(offDC, rc.left, rc.top, rc.right, rc.bottom, tempHdc, rc.left, rc.top, rc.right, rc.bottom, blend);

    SelectObject(tempHdc, oldBitmap);
    DeleteObject(hbitmap);

    TCHAR pt[10];
    _stprintf_s(pt, 10, _T("%d"), points);

    RECT textrect;
    GetClientRect(hWnd, &rc);

    SetTextColor(offDC, RGB(255, 255, 255));
    SetBkMode(offDC, TRANSPARENT);
    HFONT font = CreateFont(-MulDiv(24, GetDeviceCaps(offDC, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T(" Verdana "));
    HFONT oldFont = (HFONT)SelectObject(offDC, font);
    int nextline = 52;

    DrawText(offDC, _T(" Score:"), _tcslen(_T("Score:\n")), &rc, DT_CENTER | DT_VCENTER|DT_SINGLELINE);
    rc.top+= nextline; 
    DrawText(offDC, pt, _tcslen(pt), &rc, DT_CENTER | DT_VCENTER |DT_SINGLELINE );

    SelectObject(offDC, oldFont);
    DeleteObject(font);

}

void newGame(HWND hWnd)
{
    points = 0;
    elapsed = 0;
    InvalidateRect(hWnd, NULL, TRUE);
    fruits.clear();
    line.clear();
    start = GetTickCount64();
    SetTimer(hWnd, MOVEMENT, MOVEMENT_T, NULL);
    SetTimer(hWnd, FREQUENCY, FREQUENCY_T, NULL);
    SetTimer(hWnd, END, END_T, NULL);
    SetTimer(hWnd, PROGRESS, PROGRESS_T, NULL);
}

void selection(HMENU hMenu, int opt)
{
    switch (opt)
    {
    case 1:
        CheckMenuItem(hMenu, ID_BOARD_SMALL, MF_CHECKED);
        CheckMenuItem(hMenu, ID_BOARD_MEDIUM, MF_UNCHECKED);
        CheckMenuItem(hMenu, ID_BOARD_BIG, MF_UNCHECKED);
        break;
    case 2:
        CheckMenuItem(hMenu, ID_BOARD_SMALL, MF_UNCHECKED);
        CheckMenuItem(hMenu, ID_BOARD_MEDIUM, MF_CHECKED);
        CheckMenuItem(hMenu, ID_BOARD_BIG, MF_UNCHECKED);
        break;
    case 3:
        CheckMenuItem(hMenu, ID_BOARD_SMALL, MF_UNCHECKED);
        CheckMenuItem(hMenu, ID_BOARD_MEDIUM, MF_UNCHECKED);
        CheckMenuItem(hMenu, ID_BOARD_BIG, MF_CHECKED);
        break;
    default:
        break;
    }
}

void isIntersecting(HWND hWnd,POINT point,vector<balls>* newpoints)
{
    double x = point.x;
    double y = point.y;

    if (fruits.empty() == false)
    {
        for (auto fruit = fruits.begin(); fruit != fruits.end(); ++fruit)
        {
            double fruitxs = (*fruit).x + (*fruit).size;
            double fruitys = (*fruit).y + (*fruit).size;

            if (pow((x - fruitxs), 2) + pow((y - fruitys), 2) <= pow((*fruit).size, 2))
            {
                points++;
                COLORREF color = (*fruit).color;
                double fruit_shift = 10;
                int newsize = (*fruit).size / 2;
                double newdx = (double)(*fruit).dx / 2 + mousedx / ((*fruit).size);
                double newdy = (double)(*fruit).dy / 2 + mousedy / ((*fruit).size);

                double newx[4] = { x - fruit_shift,x + fruit_shift,x - 2*fruit_shift,x + 2*fruit_shift };
                double newy[4] = { y - fruit_shift,y + fruit_shift,y - 2*fruit_shift,y + 2*fruit_shift };

                for (int i = 0; i < 4; i++)
                {
                    balls temp(newsize, newx[i], newy[i], newdx,newdy);
                    temp.color = color;
                    (*newpoints).push_back(temp);
                }
            }
            else
                (*newpoints).push_back((*fruit));
        }
    }
}