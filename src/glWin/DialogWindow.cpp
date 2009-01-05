///////////////////////////////////////////////////////////////////////////////
// DialogWindow.cpp
// =================
// A class of Dialog Box for MS Windows
// It creates a dialog box with CreateDialogParam() API call.
//
//  AUTHOR: Song Ho Ahn
// CREATED: 2005-03-16
// UPDATED: 2006-06-20
///////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <iostream>
#include <cstring>
#include "DialogWindow.h"
#include "procedure.h"
using namespace Win;




///////////////////////////////////////////////////////////////////////////////
// constructor with params
///////////////////////////////////////////////////////////////////////////////
DialogWindow::DialogWindow(HINSTANCE hInst, WORD id, HWND hParent, Controller* ctrl) : handle(0), instance(hInst), id(id),
parentHandle(hParent), controller(ctrl),
x(0), y(0)
{
  width = 100;
  height = 100;
}



///////////////////////////////////////////////////////////////////////////////
// destructor
///////////////////////////////////////////////////////////////////////////////
DialogWindow::~DialogWindow()
{
}




HWND DialogWindow::create()
{
  handle = ::CreateDialogParam(instance, 
    MAKEINTRESOURCE(id), 
    parentHandle, 
    Win::dialogProcedure, 
    (LPARAM)controller);

  //this->show(SW_SHOWDEFAULT);                     // make it visible

  ::SetWindowPos(handle, 
    0, // ?
    x, // x
    y, // y
    width, // width
    height, // height
    SWP_NOZORDER);

  return handle;
}



///////////////////////////////////////////////////////////////////////////////
// show the window on the screen
///////////////////////////////////////////////////////////////////////////////
void DialogWindow::show(int cmdShow)
{
  ::ShowWindow(handle, cmdShow);
  ::UpdateWindow(handle);
}



