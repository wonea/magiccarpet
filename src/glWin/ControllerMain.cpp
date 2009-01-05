///////////////////////////////////////////////////////////////////////////////
// ControllerMain.cpp
// ==================
// Derived Controller class for main window
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2006-07-09
// UPDATED: 2006-07-09
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <windows.h>
#include <commctrl.h>                   // common controls
#include "ControllerMain.h"
#include "resource.h"
//#include <locale>
#include <string>

//#include "../../wblib/wblib/HeightMapLoader.h"

using namespace Win;

bool CALLBACK enumerateChildren(HWND childHandle, LPARAM lParam);

ControllerMain::ControllerMain(ModelGL* _model) {
  model = _model;
}

int ControllerMain::keyDown(int key, LPARAM lParam) {
  std::cout << "keyDown in MainController" << std::endl;
  return 0;
}

int ControllerMain::keyUp(int key, LPARAM lParam) {
  std::cout << "keyUp in MainController" << std::endl;
  return 0;
}

/**
* Handles selections in the main window's menubar
*/
int ControllerMain::command(int id, int cmd, LPARAM msg) {
  //wbLib::HeightMapLoader hml;
  OPENFILENAME ofn;
  TCHAR szFileName[MAX_PATH];
  DWORD error;
  //std::wstring wideString;
  //std::wstring myString(L"Liebe Grüße!");

  switch(id)
  {
  case ID_FILE_EXIT:
    ::PostMessage(handle, WM_CLOSE, 0, 0);
    break;

  case ID_FILE_LOADHEIGHTMAP:
    std::cout << __FILE__ << " " << __LINE__ <<  " load height map" << std::endl;

    ZeroMemory(&ofn, sizeof(ofn));
    ZeroMemory(szFileName, MAX_PATH);

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = handle;
    ofn.hInstance = NULL;
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    //ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.nMaxFile = MAX_PATH;
    ofn.nMaxFileTitle = MAX_PATH;
    ofn.lpstrTitle = NULL;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lCustData = 0l;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;
    ofn.lpstrFile = szFileName;
    ofn.lpstrFileTitle = szFileName;
    ofn.lpstrDefExt = L"txt";
    //ofn.lpstrDefExt = "txt";
    ofn.lpstrInitialDir = NULL;

    if (::GetOpenFileName(&ofn)) {
      //wideString = szFileName;
      //std::wcout << szFileName << std::endl;
      //std::wcout << wideString << std::endl;
      //std::wcout << myString << std::endl;
    } else {
      error = ::CommDlgExtendedError();
      switch (error) {
  case CDERR_DIALOGFAILURE:
    std::cout << __FILE__ << " " << __LINE__ <<  " CDERR_DIALOGFAILURE" << std::endl;
    break;

  case CDERR_FINDRESFAILURE:
    std::cout << "CDERR_FINDRESFAILURE" << std::endl; 
    break;

  case CDERR_NOHINSTANCE:
    std::cout << "CDERR_NOHINSTANCE" << std::endl;
    break;

  case CDERR_INITIALIZATION:
    std::cout << "CDERR_INITIALIZATION" << std::endl;
    break;

  case CDERR_NOHOOK:
    std::cout << "CDERR_NOHOOK" << std::endl;
    break;

  case CDERR_LOCKRESFAILURE:
    std::cout << "CDERR_LOCKRESFAILURE" << std::endl;
    break;

  case CDERR_NOTEMPLATE:
    std::cout << "CDERR_NOTEMPLATE" << std::endl;
    break;

  case CDERR_LOADRESFAILURE:
    std::cout << "CDERR_LOADRESFAILURE" << std::endl;
    break;

  case CDERR_STRUCTSIZE:
    std::cout << "CDERR_STRUCTSIZE" << std::endl;
    break;

  case CDERR_LOADSTRFAILURE:
    std::cout << "CDERR_LOADSTRFAILURE" << std::endl;
    break;

  case FNERR_BUFFERTOOSMALL:
    std::cout << "FNERR_BUFFERTOOSMALL" << std::endl;
    break;

  case CDERR_MEMALLOCFAILURE:
    std::cout << "CDERR_MEMALLOCFAILURE" << std::endl;
    break;

  case FNERR_INVALIDFILENAME:
    std::cout << "FNERR_INVALIDFILENAME" << std::endl;
    break;

  case CDERR_MEMLOCKFAILURE:
    std::cout << "CDERR_MEMLOCKFAILURE" << std::endl;
    break;

  case FNERR_SUBCLASSFAILURE:
    std::cout << "FNERR_SUBCLASSFAILURE" << std::endl;
    break;

  default:
    std::cout << __FILE__ << " " << __LINE__ <<  " unknown error" << std::endl;
    break;
      }
    }

    //wbLib::HeightMapLoader hml;
    //hml.load("test.txt");
    //hml.cleanUp();

    model->load(szFileName);


    //}
    break;
  }

  return 0;
}

int ControllerMain::close() {
  // close all child windows first
  ::EnumChildWindows(handle, (WNDENUMPROC)enumerateChildren, (LPARAM)WM_CLOSE);

  // close itself
  ::DestroyWindow(handle);

  return 0;
}

int ControllerMain::create() {
  return 0;
}

int ControllerMain::size(int w, int h, WPARAM wParam) {
  // pass it to status bar
  HWND statusHandle = ::GetDlgItem(handle, IDC_STATUSBAR);
  ::SendMessage(statusHandle, WM_SIZE, 0, 0);     // automatically resize width, so send 0s
  ::InvalidateRect(statusHandle, 0, FALSE);       // force to repaint
  return 0;
}

/**
 * enumerate all child windows
 */
bool CALLBACK enumerateChildren(HWND handle, LPARAM lParam) {
  if(lParam == WM_CLOSE) {
    ::SendMessage(handle, WM_CLOSE, 0, 0);  // close child windows
  }
  return true;
}
