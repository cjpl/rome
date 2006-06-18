// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMESplashScreen                                                     //
//                                                                      //
//  Displays a splash screen.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )

#include <Windows4Root.h>
#include "ROMEiostream.h"
#include "ROMEString.h"

#define ID_SPLASHSCREEN      25
#define MY_BUFSIZE         1024 // buffer size for console window titles

///////////////////////////////////////////////////////////////////////////////
// Global Variables:
HINSTANCE hInst;                        // current instance
HWND hMainWnd = NULL;
HWND hSplashWnd = NULL;               // Splash screen
HBITMAP hBmp = NULL;
BITMAP bm;
BOOL bSplash = TRUE;
DWORD DelayVal = 0;
BOOL bClassRegistered = FALSE;

///////////////////////////////////////////////////////////////////////////////
// Foward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    SplashWndProc(HWND, UINT, WPARAM, LPARAM);


////////////////////////////////////////////////////////////////////////////////
// Splashscreen functions
////////////////////////////////////////////////////////////////////////////////
//
//
void HideSplashScreen()
{
    // Destroy the window
    if (IsWindow(hSplashWnd)) {
        DestroyWindow(hSplashWnd);
        DeleteObject(hBmp);
        hSplashWnd = NULL;
        UnregisterClass("RomeSplashScreen", hInst);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//
BOOL CreateSplashScreen(HWND hParent)
{
    int xScreen;
    int yScreen;
    // Crenter the splashscreen
    xScreen = GetSystemMetrics(SM_CXFULLSCREEN);
    yScreen = GetSystemMetrics(SM_CYFULLSCREEN);

    hSplashWnd = CreateWindowEx(
            WS_EX_DLGMODALFRAME,//WS_EX_TOOLWINDOW,
            "RomeSplashScreen",
            NULL,
            WS_POPUP | WS_VISIBLE ,
            (xScreen - 360)/2,
            (yScreen - 240)/2,
            360, 240,
            hParent,
            NULL,
            hInst,
            NULL);
    
    return (hSplashWnd != NULL);
}


BOOL PreTranslateMessage(MSG* pMsg)
{
    if (!IsWindow(hSplashWnd))
        return FALSE;

    // If we get a keyboard or mouse message, hide the splash screen.
    if (pMsg->message == WM_KEYDOWN ||
        pMsg->message == WM_SYSKEYDOWN ||
        pMsg->message == WM_LBUTTONDOWN ||
        pMsg->message == WM_RBUTTONDOWN ||
        pMsg->message == WM_MBUTTONDOWN ||
        pMsg->message == WM_NCLBUTTONDOWN ||
        pMsg->message == WM_NCRBUTTONDOWN ||
        pMsg->message == WM_NCMBUTTONDOWN)
    {
        HideSplashScreen();
        return TRUE;    // message handled here
    }

    return FALSE;   // message not handled
}

char* programName;
char* version;
ROMEString *authorNames;
int numberOfAuthors;

void CreateSplash(DWORD time,char* prog,char* vers,ROMEString* author,int numAuthor)
{
    programName = prog;
    version = vers;
    authorNames = author;
    numberOfAuthors = numAuthor;
    MSG msg;
    if (MyRegisterClass(hInst) != 0)
        bClassRegistered = TRUE;
    if(time > 0) {
        bSplash = TRUE;
        DelayVal = time * 1000;
    }
    else return;
    // Create the splash screen
    if (bSplash)
        CreateSplashScreen(NULL);

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0)) {
        PreTranslateMessage(&msg);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    HideSplashScreen();
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = (WNDPROC)SplashWndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance,IDI_ERROR);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = CreateSolidBrush(0x0bddfe6);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = "RomeSplashScreen";
    wcex.hIconSm        = NULL;
    return RegisterClassEx(&wcex);
}



////////////////////////////////////////////////////////////////////////////
// Message handler for splash screen.
//
LRESULT CALLBACK SplashWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ROMEString buf;
    PAINTSTRUCT ps;
    HDC hDC;
    HANDLE hfbm;
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    HGLOBAL hmem1,hmem2;
    LPBITMAPINFO lpbmi;
    LPVOID lpvBits;
    DWORD dwRead;
    HWND hwndFound;         // this is what is returned to the caller
    ROMEString pszNewWindowTitle; // contains fabricated WindowTitle
    ROMEString bmpDir("\\icons\\rome.bmp");
    ROMEString FullBmpDir;
    char *RomeSysDir;
    int xScreen;
    int yScreen;

    switch (message)
    {
        case WM_CREATE:
            SetTimer(hWnd, ID_SPLASHSCREEN, DelayVal, NULL);
            RomeSysDir = getenv("ROMESYS");
            FullBmpDir.SetFormatted("%s%s",RomeSysDir,bmpDir.Data());
            // Retrieve a handle identifying the file. 
            hfbm = CreateFile(FullBmpDir.Data(), GENERIC_READ, 
                    FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES) NULL, 
                    OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, 
                    (HANDLE) NULL); 
            if(hfbm) {
                // Retrieve the BITMAPFILEHEADER structure. 
                ReadFile(hfbm, &bmfh, sizeof(BITMAPFILEHEADER), &dwRead, 
                        (LPOVERLAPPED)NULL); 
                // Retrieve the BITMAPFILEHEADER structure. 
                ReadFile(hfbm, &bmih, sizeof(BITMAPINFOHEADER), 
                        &dwRead, (LPOVERLAPPED)NULL); 
                // Allocate memory for the BITMAPINFO structure. 
                hmem1 = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) + 
                        ((1<<bmih.biBitCount) * sizeof(RGBQUAD))); 
                lpbmi = (LPBITMAPINFO)GlobalLock(hmem1); 
                // Load BITMAPINFOHEADER into the BITMAPINFO structure. 
                lpbmi->bmiHeader.biSize = bmih.biSize; 
                lpbmi->bmiHeader.biWidth = bmih.biWidth; 
                lpbmi->bmiHeader.biHeight = bmih.biHeight; 
                lpbmi->bmiHeader.biPlanes = bmih.biPlanes; 
                lpbmi->bmiHeader.biBitCount = bmih.biBitCount; 
                lpbmi->bmiHeader.biCompression = bmih.biCompression; 
                lpbmi->bmiHeader.biSizeImage = bmih.biSizeImage; 
                lpbmi->bmiHeader.biXPelsPerMeter = bmih.biXPelsPerMeter; 
                lpbmi->bmiHeader.biYPelsPerMeter = bmih.biYPelsPerMeter; 
                lpbmi->bmiHeader.biClrUsed = bmih.biClrUsed; 
                lpbmi->bmiHeader.biClrImportant = bmih.biClrImportant; 
                // Retrieve the color table. 
                // 1 << bmih.biBitCount == 2 ^ bmih.biBitCount 
                ReadFile(hfbm, lpbmi->bmiColors, 
                        ((1<<bmih.biBitCount) * sizeof(RGBQUAD)), 
                        &dwRead, (LPOVERLAPPED) NULL); 
                // Allocate memory for the required number of bytes. 
                hmem2 = GlobalAlloc(GHND, (bmfh.bfSize - bmfh.bfOffBits)); 
                lpvBits = GlobalLock(hmem2); 
                // Retrieve the bitmap data. 
                ReadFile(hfbm, lpvBits, (bmfh.bfSize - bmfh.bfOffBits), 
                        &dwRead, (LPOVERLAPPED) NULL); 
                hDC = GetDC(hWnd);
                // Create a bitmap from the data stored in the .BMP file. 
                hBmp = CreateDIBitmap(hDC, &bmih, CBM_INIT, lpvBits, lpbmi, DIB_RGB_COLORS); 
                // Unlock the global memory objects and close the .BMP file. 
                GlobalUnlock(hmem1); 
                GlobalUnlock(hmem2); 
                CloseHandle(hfbm); 
                GetObjectA(hBmp, sizeof(bm), &bm);
                // Center the splashscreen
                xScreen = GetSystemMetrics(SM_CXFULLSCREEN);
                yScreen = GetSystemMetrics(SM_CYFULLSCREEN);
                if (numberOfAuthors>5) {
                    long hh = bm.bmHeight-250+numberOfAuthors*20/2;
                    SetWindowPos(hWnd, HWND_TOPMOST, (xScreen - bm.bmWidth)/2, 
                        (yScreen - hh)/2, bm.bmWidth, hh, 0 );
                }
                else {
                    long hh = bm.bmHeight-250+numberOfAuthors*20;
                    SetWindowPos(hWnd, HWND_TOPMOST, (xScreen - bm.bmWidth)/2, 
                        (yScreen - hh)/2, bm.bmWidth, hh, 0 );
                }
            }
            break;

        case WM_TIMER:
            if (wParam == ID_SPLASHSCREEN) {
                KillTimer (hWnd, ID_SPLASHSCREEN);
                HideSplashScreen();
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);

        case WM_PAINT:
            hDC = BeginPaint(hWnd, &ps);
            RECT rt;
            GetClientRect(hWnd, &rt);
            HDC hImageDC;
            hImageDC = CreateCompatibleDC(hDC);
            if (hImageDC == NULL)
                return FALSE;
            // Paint the image.
            HBITMAP hOldBitmap;
            hOldBitmap = (HBITMAP)SelectObject(hImageDC, hBmp);
            BitBlt(hDC, -2, -2, bm.bmWidth-4, bm.bmHeight-4, hImageDC, 0, 0, SRCCOPY);
            SelectObject(hImageDC, hOldBitmap);

            int i;
            HFONT fontProg,fontText,fontROME,fontAuthor;
            fontProg = CreateFont(30,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
                     OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"BernhardMod BT");
            fontText = CreateFont(24,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
                     OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"BernhardMod BT");
            fontROME = CreateFont(60,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
                     OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"BernhardMod BT");
            fontAuthor = CreateFont(16,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
                     OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"BernhardMod BT");
            SetTextAlign(hDC,TA_CENTER);
            SetBkColor(hDC,0x0bddfe6);

            buf = programName;
            SelectObject(hDC,fontProg);
            TextOut(hDC,bm.bmWidth/2,20,buf.Data(),buf.Length());

            buf = "generated by the";
            SelectObject(hDC,fontText);
            TextOut(hDC,bm.bmWidth/2,60,buf.Data(),buf.Length());

            SetTextColor(hDC,0x00000b2);
            buf = "ROME";
            SelectObject(hDC,fontROME);
            TextOut(hDC,bm.bmWidth/2,90,buf.Data(),buf.Length());
            SetTextColor(hDC,0x0000000);

            buf = "Environment";
            SelectObject(hDC,fontText);
            TextOut(hDC,bm.bmWidth/2,150,buf.Data(),buf.Length());

            buf = version;
            SelectObject(hDC,fontText);
            TextOut(hDC,bm.bmWidth/2,355,buf.Data(),buf.Length());

            buf  = "Author of ROME : Matthias Schneebeli";
            SelectObject(hDC,fontAuthor);
            TextOut(hDC,bm.bmWidth/2,400,buf.Data(),buf.Length());

            if (numberOfAuthors>0) {
               if (numberOfAuthors==1) 
                  buf.SetFormatted("Author of the %s :",programName);
               else 
                  buf.SetFormatted("Authors of the %s :",programName);
               SelectObject(hDC,fontAuthor);
               TextOut(hDC,bm.bmWidth/2,430,buf.Data(),buf.Length());

               if (numberOfAuthors>5) {
                  for (i=0;i<numberOfAuthors;i++) {
                      buf = authorNames[i];
                      if (i+1<numberOfAuthors) 
                         buf.AppendFormatted(", %s",authorNames[(i+1)].Data());
                      SelectObject(hDC,fontAuthor);
                      TextOut(hDC,bm.bmWidth/2,450+20*(i/2),buf.Data(),buf.Length());
                      i++;
                  }
               }
               else {
                  for (i=0;i<numberOfAuthors;i++) {
                      buf = authorNames[i];
                      SelectObject(hDC,fontAuthor);
                      TextOut(hDC,bm.bmWidth/2,450+20*i,buf.Data(),buf.Length());
                  }
               }
            }

            
            
            EndPaint(hWnd, &ps);
            // format a "unique" NewWindowTitle
            pszNewWindowTitle.SetFormatted("%d/%d", GetTickCount(), GetCurrentProcessId());
            // change current window title
            SetConsoleTitle(pszNewWindowTitle.Data());
            // ensure window title has been updated
            Sleep(40);
            // look for NewWindowTitle
            hwndFound=FindWindow(NULL, pszNewWindowTitle.Data());
            // restore original window title
            ShowWindow(hwndFound, SW_RESTORE);
            SetForegroundWindow(hwndFound);
            SetConsoleTitle("ROME");
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
#endif
