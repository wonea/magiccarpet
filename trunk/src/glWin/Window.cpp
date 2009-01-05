///////////////////////////////////////////////////////////////////////////////
// Window.cpp
// ==========
// A class of Window for MS Windows
// It registers window class(WNDCLASSEX) with RegisterClassEx() and creates a 
// window with CreateWindowEx() API call.
//
//  AUTHOR: Song Ho Ahn
// CREATED: 2005-03-16
// UPDATED: 2006-09-20
///////////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4996)
#include <sstream>
#include <iostream>
#include <cstring>
#include "Window.h"
#include "procedure.h"
using std::wstringstream;
using std::wcout;
using std::endl;

using namespace Win;




///////////////////////////////////////////////////////////////////////////////
// constructor with params
// NOTE: Windows does not clip a child window from the parent client's area.
// To prevent the parent window from drawing over its child window area, the
// parent window must have WS_CLIPCHILDREN flag.
///////////////////////////////////////////////////////////////////////////////
Window::Window(HINSTANCE hInst, const wchar_t* name, HWND hParent, Controller* ctrl) : handle(0), instance(hInst), controller(ctrl), /*winStyle(WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN)*/ winStyle(WS_OVERLAPPED | WS_SYSMENU),
//Window::Window(HINSTANCE hInst, const char* name, HWND hParent, Controller* ctrl) : handle(0), instance(hInst), controller(ctrl), /*winStyle(WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN)*/ winStyle(WS_OVERLAPPED | WS_SYSMENU),
winStyleEx(WS_EX_CLIENTEDGE), x(CW_USEDEFAULT), y(CW_USEDEFAULT),
width(CW_USEDEFAULT), height(CW_USEDEFAULT),
parentHandle(hParent), menuHandle(0)
{
  // copy string
  wcsncpy(this->title, name, MAX_STRING-1);
  //strncpy(this->title, name, MAX_STRING-1);
  wcsncpy(this->className, name, MAX_STRING-1);
  //strncpy(this->className, name, MAX_STRING-1);

  // populate window class struct
  winClass.cbSize        = sizeof(WNDCLASSEX);
  winClass.style = 0;
  //winClass.style         = 0 | CS_HREDRAW | CS_VREDRAW;                             // class styles: CS_OWNDC, CS_PARENTDC, CS_CLASSDC, CS_GLOBALCLASS, ...
  //winClass.style = WS_OVERLAPPED | WS_SYSMENU;
  winClass.lpfnWndProc   = Win::windowProcedure;          // pointer to window procedure
  winClass.cbClsExtra    = 0;
  winClass.cbWndExtra    = 0;
  winClass.hInstance     = instance;                      // owner of this class
  winClass.hIcon         = LoadIcon(0, IDI_APPLICATION);  // default icon
  winClass.hCursor       = LoadCursor(0, IDC_ARROW);      // default arrow cursor
  winClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // default white brush
  winClass.lpszMenuName  = 0;
  winClass.lpszClassName = className;
  winClass.hIconSm       = LoadIcon(0, IDI_APPLICATION);  // default small icon

  width = 100;
  height = 100;
}



///////////////////////////////////////////////////////////////////////////////
// destructor
///////////////////////////////////////////////////////////////////////////////
Window::~Window()
{
  ::UnregisterClass(className, instance);
}



///////////////////////////////////////////////////////////////////////////////
// create a window
///////////////////////////////////////////////////////////////////////////////
HWND Window::create()
{
  // register a window class
  if(!::RegisterClassEx(&winClass)) return 0;

  handle = ::CreateWindowEx(winStyleEx,           // window border with a sunken edge
    className,            // name of a registered window class
    title,                // caption of window
    winStyle,             // window style
    x,                    // x position
    y,                    // y position
    width,                // witdh
    height,               // height
    parentHandle,         // handle to parent window
    menuHandle,           // handle to menu
    instance,             // application instance
    (LPVOID)controller);  // window creation data

  //this->show(SW_SHOWDEFAULT);                     // make it visible

  sizeClientArea();

  return handle;
}



///////////////////////////////////////////////////////////////////////////////
// show the window on the screen
///////////////////////////////////////////////////////////////////////////////
void Window::show(int cmdShow)
{
  ::ShowWindow(handle, cmdShow);
  ::UpdateWindow(handle);
}



///////////////////////////////////////////////////////////////////////////////
// print itself
///////////////////////////////////////////////////////////////////////////////
void Window::printSelf() const
{
  wstringstream wss;                          // wide char output string stream buffer

  // build output string
  wss << L"=== Win::Window object ===\n"
    << L"Name: " << title << L"\n"
    << L"Position: (" << x << L", " << y << L")\n"
    << L"Width: " << width << L"\n"
    << L"Height: " << height << L"\n"
    << L"Handle: " << handle << L"\n"
    << L"Parent Handle: " << parentHandle << L"\n"
    << L"Menu Handle: " << menuHandle << L"\n"
    << L"Instance: " << instance << L"\n"
    << L"Controller: " << std::hex << controller << L"\n"
    << endl;

  wcout << wss.str();                         // print the string to the console
}



///////////////////////////////////////////////////////////////////////////////
// load an icon using resource ID and convert it to HICON
///////////////////////////////////////////////////////////////////////////////
HICON Window::loadIcon(int id)
{
  return (HICON)::LoadImage(instance, MAKEINTRESOURCE(id), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
}



///////////////////////////////////////////////////////////////////////////////
// load an icon using resource ID and convert it to HICON
///////////////////////////////////////////////////////////////////////////////
HICON Window::loadCursor(int id)
{
  return (HCURSOR)::LoadImage(instance, MAKEINTRESOURCE(id), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE);
}

/**
 * Sets client area width and adjusts window size, if window has already
 * been created
 */
void Window::setWidth(int _w) {
  width = _w;

  // adjust window to desired size
  sizeClientArea();
}

void Window::setHeight(int _h) { 
  height = _h; 

   // adjust window to desired size
  sizeClientArea();
}

/**
 * Uses currently set width and height to scale the window, so that its
 * client area are of width and height. Takes the menu bar into account
 * although officially the menu bar is part of the client area
 */
void Window::sizeClientArea() {

  RECT rect;
  RECT menuBarRect;
  HMENU menuHandle = NULL;

  if (NULL == handle) {
    return;
  }

  DWORD style = ::GetWindowLong(handle, GWL_STYLE);
  DWORD styleEx = ::GetWindowLong(handle, GWL_EXSTYLE);

  // menu bar is part of the client area but this
  // method does not count the menu bar to the client area
  SetRect(&menuBarRect, 0, 0, 0, 0);
  menuHandle = ::GetMenu(handle);
  if (NULL != menuHandle) {
    GetMenuItemRect(handle, menuHandle, 0, &menuBarRect);
  }

  rect.left = 0;
  rect.right = width;
  rect.top = 0;
  // The height of the menubar is computed by (bottom - top)
  rect.bottom = height + (menuBarRect.bottom - menuBarRect.top);

  // If the window has a caption (thick blue bar at the top of the window),
  // then we have to adjust its size, so that the client area has the desired
  // dimensions
  if (style & WS_CAPTION) {

    // computes the needed window size according to a desired client area size
    // and stores that size into the first parameter, which is an in out parameter.
    // This call does not resize a window
    ::AdjustWindowRectEx(&rect, style, TRUE, styleEx);

    // use the computed size and resize the window
    ::SetWindowPos(handle, 
      0, 
      x, 
      y, 
      (rect.right - rect.left), 
      (rect.bottom - rect.top), 
      SWP_NOZORDER);
  }
}

