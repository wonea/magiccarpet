#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <fcntl.h>     /* for _O_TEXT and _O_BINARY */
#include <errno.h>     /* for EINVAL */
#include <sys\stat.h>  /* for _S_IWRITE */
#include <share.h>     /* for _SH_DENYNO */
#include <io.h>        /* Also needed for console ... */
#include <locale>
#include <string>
#include <windows.h>
#include <commctrl.h>
#include "Window.h"
#include "DialogWindow.h"
#include "ControllerMain.h"
#include "ControllerGL.h"
#include "ControllerFormGL.h"
#include "ModelGL.h"
#include "ViewGL.h"
#include "ViewFormGL.h"
#include "resource.h"
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <iomanip>

int mainMessageLoop(HACCEL hAccelTable=0);

/**
* Opens a console window which displays every std::cout output of this
* application. This console cannot be used for input!
*/
void InitConsoleStreams() {
  // The following code opens a console and redirects the stream
  // std::out to that console.
  int hCrt;
  FILE *hf;
  AllocConsole();
  hCrt = _open_osfhandle(
    (long) GetStdHandle(STD_OUTPUT_HANDLE),
    _O_TEXT
    );
  hf = _fdopen( hCrt, "w" );
  *stdout = *hf;
  int i = setvbuf( stdout, NULL, _IONBF, 0 );
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdArgs, int cmdShow) {
  InitConsoleStreams();

  log4cplus::SharedAppenderPtr myAppender(new log4cplus::RollingFileAppender(L"log.log"));
  myAppender->setName(L"myAppenderName");
  std::auto_ptr<log4cplus::Layout> myLayout = std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(L"%d{%Y-%m-%d %a %H:%M:%S} %-5p %l : %m%n"));
  //std::auto_ptr myLayout = std::auto_ptr(new log4cplus::PatternLayout(L"%d{%Y-%m-%d %a %H:%M:%S} %-5p [%t]: %m%n"));
  myAppender->setLayout(myLayout);
  log4cplus::Logger logger = log4cplus::Logger::getInstance(L"loggerName");
  logger.addAppender(myAppender);
  logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);
  //logger.log(log4cplus::INFO_LOG_LEVEL, L"Info Level Message");
  //logger.log(log4cplus::TRACE_LOG_LEVEL, L"Trace Level Mesage");
    
  // register slider(trackbar) from comctl32.dll brfore creating windows
  INITCOMMONCONTROLSEX commonCtrls;
  commonCtrls.dwSize = sizeof(commonCtrls);
  // trackbar is in this class
  commonCtrls.dwICC = ICC_BAR_CLASSES;
  ::InitCommonControlsEx(&commonCtrls);

  // get app name from resource file
  wchar_t name[256];
  //char name[256];
  ::LoadString(hInst, IDS_APP_NAME, name, 256);

  // create model and view components for controller
  ModelGL modelGL;
  Win::ViewGL viewGL;
  Win::ViewFormGL viewFormGL;

  Win::ControllerMain mainCtrl(&modelGL);
  Win::Window mainWin(hInst, name, 0, &mainCtrl);

  // add menu to window class
  mainWin.setMenuName(MAKEINTRESOURCE(IDR_MAIN_MENU));
  mainWin.setWidth(800);
  mainWin.setHeight(600);
  mainWin.setPosition(0, 0);
  mainWin.setWindowStyleEx(WS_EX_WINDOWEDGE);

  // create a window and show
  mainWin.create();

  // create OpenGL rendering window as a child
  Win::ControllerGL glCtrl(&modelGL, &viewGL);

  Win::Window glWin(hInst, L"WindowGL", mainWin.getHandle(), &glCtrl);
  //Win::Window glWin(hInst, "WindowGL", mainWin.getHandle(), &glCtrl);

  glWin.setClassStyle(CS_OWNDC);
  glWin.setWindowStyle(0 | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
  glWin.setWidth(640);
  glWin.setHeight(480);

  glWin.setPosition(0, 0);

  glWin.create();

  // create a child dialog box contains controls
  Win::ControllerFormGL formCtrl(&modelGL, &viewFormGL);

  Win::DialogWindow glDialog(hInst, IDD_CONTROLS, mainWin.getHandle(), &formCtrl);

  glDialog.setWidth(800);
  glDialog.setHeight(120);

  glDialog.setPosition(0, 480);

  glDialog.create();

  // create status bar window using CreateWindowEx()
  // mainWin must pass WM_SIZE message to the status bar
  // So, mainWin accesses the status bar with GetDlgItem(handle, IDC_STATUSBAR).
  HWND statusHandle = ::CreateWindowEx(0, STATUSCLASSNAME, 0, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
    0, 0, 0, 0, mainWin.getHandle(), (HMENU)IDC_STATUSBAR, ::GetModuleHandle(0), 0);
 
  ::SendMessage(statusHandle, SB_SETTEXT, 0, (LPARAM)L"Ready");
  
  mainWin.show();

  int exitCode;

  HACCEL hAccelTable = 0;
  
  exitCode = mainMessageLoop(hAccelTable);

  return exitCode;
}



///////////////////////////////////////////////////////////////////////////////
// main message loop
///////////////////////////////////////////////////////////////////////////////
int mainMessageLoop(HACCEL hAccelTable)
{
  HWND activeHandle;
  MSG msg;

  while(::GetMessage(&msg, 0, 0, 0) > 0)  // loop until WM_QUIT(0) received
  {
    // determine the activated window is dialog box
    // skip if messages are for the dialog windows
    activeHandle = GetActiveWindow();
    if(::GetWindowLongPtr(activeHandle, GWL_EXSTYLE) & WS_EX_CONTROLPARENT) // WS_EX_CONTROLPARENT is automatically added by CreateDialogBox()
    {
      if(::IsDialogMessage(activeHandle, &msg))
        continue;   // message handled, back to while-loop
    }

    // now, handle window messages
    if(!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
    }
  }

  return (int)msg.wParam;                 // return nExitCode of PostQuitMessage()
}
