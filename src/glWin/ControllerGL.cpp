///////////////////////////////////////////////////////////////////////////////
// ControllerGL.cpp
// ================
// Derived Controller class for OpenGL window
// It is the controller of OpenGL rendering window. It initializes DC and RC,
// when WM_CREATE called, then, start new thread for OpenGL rendering loop.
//
// When this class is constructed, it gets the pointers to model and view
// components.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gamil.com)
// CREATED: 2006-07-09
// UPDATED: 2006-08-15
///////////////////////////////////////////////////////////////////////////////

#include <process.h>                                // for _beginthreadex()
#include "ControllerGL.h"
#include "Log.h"
using namespace Win;

ControllerGL::ControllerGL(ModelGL* model, ViewGL* view) : 
  model(model),
  view(view),
  threadHandle(0),
  threadId(0),
  loopFlag(false) 
{  
}

int ControllerGL::close() {
  loopFlag = false;
  // wait for rendering thread is terminated
  ::WaitForSingleObject(threadHandle, INFINITE);

  // close OpenGL Rendering context
  view->closeContext(handle);

  ::DestroyWindow(handle);
  return 0;
}

int ControllerGL::create() {
  // create a OpenGL rendering context
  if (!view->createContext(handle, 32, 24, 8)) {
    //Win::log(L"[ERROR] Failed to create OpenGL rendering context from ControllerGL::create().");
    return -1;
  }

  // create a thread for OpenGL rendering
  // The params of _beginthreadex() are security, stackSize, functionPtr, argPtr, initFlag, threadId.
  threadHandle = (HANDLE)_beginthreadex(0, 0, (unsigned (__stdcall *)(void *))threadFunction, this, 0, &threadId);
  if (threadHandle)
  {
    loopFlag = true;
    //Win::log(L"Created a rendering thread for OpenGL.");
  }
  //else
  //{
  //    Win::log(L"[ERROR] Failed to create rendering thread from ControllerGL::create().");
  //}

  return 0;
}

int ControllerGL::paint() {
  return 0;
}

int ControllerGL::command(int id, int cmd, LPARAM msg) {
  return 0;
}

void ControllerGL::threadFunction(void* param) {
  ((ControllerGL*)param)->runThread();
}

/**
 * Initialize OpenGL states and start rendering loop
 */
void ControllerGL::runThread() {
  
  // set the current RC in this thread
  ::wglMakeCurrent(view->getDC(), view->getRC());

  wbLib::FreeFlightCamera camera(50.0f, 0.0f, 50.0f);
  camera.xLookSpeed = 1.0f;
  camera.yLookSpeed = 1.0f;
  camera.moveSpeed = 1.5f;

  ::POINT point;
  ::GetCursorPos(&point);
  float oldX = (float)point.x;
  float oldY = (float)point.y;

  wbLib::ms3d::MS3DLoader ms3dLoader;

  try {
    //ms3dLoader.load("Data/model.ms3d");
    //ms3dLoader.load("Data/ant01.ms3d");
    //ms3dLoader.load("Data/dwarf1.ms3d");
    //ms3dLoader.load("Data/Cross.ms3d");
    //ms3dLoader.load("Data/turtle1.ms3d");
    ms3dLoader.load("Data/zombie02.ms3d");
  } catch (std::ios_base::failure f) {
    std::cout << f.what() << std::endl;
  }
  
  //wbLib::Landscape landscape;
  //int patchCount = 16;
  //int mapSize = 1024;
  //char * buffer;
  //buffer = NULL;

  //try {
  //  int bufferSize = 0;
  //  std::ifstream filestream;
  //  
	 // filestream.open(L"Height1024.raw", std::ios::in | std::ios::binary | std::ios::ate);
	 // if (!filestream.is_open()) {
  //    std::ios_base::failure f("Could not find file!");
  //    throw f;
	 // }
	 // bufferSize = filestream.tellg();
	 // filestream.seekg (0, std::ios::beg);
	 // buffer = new char[bufferSize];
	 // filestream.read(buffer, bufferSize);
	 // if (!filestream.good()) {
		//  delete[] buffer;
		//  buffer = NULL;
	 // }
	 // filestream.close();
  //} catch (std::ios_base::failure f) {    
  //  //logger.log(log4cplus::INFO_LOG_LEVEL, L"File not found");
  //}

  //landscape.init(mapSize, buffer, patchCount, &camera);

  model->init();

  RECT rect;
  ::GetClientRect(handle, &rect);
  model->setViewport(rect.right, rect.bottom);

  while (loopFlag) {
    // yield for other threads
    ::Sleep(1);    

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // VK key codes: http://msdn.microsoft.com/en-us/library/ms645540(VS.85).aspx
    if (::GetAsyncKeyState(0x57)) {
      camera.MoveForward();
    }
    if (::GetAsyncKeyState(0x53)) {
      camera.MoveBackward();
    }
    if (::GetAsyncKeyState(0x41)) {
      camera.StrafeLeft();
    }
    if (::GetAsyncKeyState(0x44)) {
      camera.StrafeRight();
    }
    
    ::GetCursorPos(&point);
    float deltaX = (float)point.x - (float)oldX;
    float deltaY = (float)point.y - (float)oldY;
    oldX = (float)point.x;
    oldY = (float)point.y;
    camera.MouseRotate(deltaX, deltaY);

    glutSolidSphere(1.0f, 5, 5);

    //ms3dLoader.drawVertices();
    //ms3dLoader.drawTriangles();
    //ms3dLoader.drawGroups();
    //ms3dLoader.drawJoints();
    
    ms3dLoader.advanceAnimation();
    ms3dLoader.drawGroupsUsingJoints();
    //ms3dLoader.drawJointsAnimated();

    //std::cout << "x: " << camera.pos.x << " z: " << camera.pos.z << std::endl;
    
    // you have to call reset prior to rendering every frame, as every frame
    // the patches need to get a new tesselation. So their base triangles
    // need to be set reset to null pointers, to throw away the complete
    // binary triangle tree from the last frame
    /*landscape.reset();
    landscape.tessellate();
    landscape.render();*/
    view->swapBuffers();
  }

  ::wglMakeCurrent(0, 0);
  ::CloseHandle(threadHandle);

  model->cleanUp();
  //landscape.cleanUp();
  ms3dLoader.cleanUp();
}

int ControllerGL::lButtonDown(WPARAM state, int x, int y) {
  //// update mouse position
  //model->setMousePosition(x, y);
  //if(state == MK_LBUTTON)
  //{
  //    model->setMouseLeft(true);
  //}
  
  return 0;
}

int ControllerGL::lButtonUp(WPARAM state, int x, int y) {
  //// update mouse position
  //model->setMousePosition(x, y);
  //model->setMouseLeft(false);
  return 0;
}

int ControllerGL::rButtonDown(WPARAM state, int x, int y) {
  //// update mouse position
  //model->setMousePosition(x, y);
  //if(state == MK_RBUTTON) {
  //    model->setMouseRight(true);
  //}
  return 0;
}

int ControllerGL::rButtonUp(WPARAM state, int x, int y) {
  //// update mouse position
  //model->setMousePosition(x, y);
  //model->setMouseRight(false);
  return 0;
}

int ControllerGL::keyDown(int key, LPARAM lParam) {
  std::cout << "Key down" << std::endl;
  return 0;
}

int ControllerGL::keyUp(int key, LPARAM lParam) {
  std::cout << "Key up" << std::endl;
  return 0;
}

int ControllerGL::mouseMove(WPARAM state, int x, int y) {
  /*if(state == MK_LBUTTON)
  {
  model->rotateCamera(x, y);
  }
  if(state == MK_RBUTTON)
  {
  model->zoomCamera(y);
  }*/
  //std::cout << "mouse move" << std::endl;

  //float deltaX = (float)x - (float)oldX;
  //float deltaY = (float)y - (float)oldY;

  //oldX = (float)x;
  //oldY = (float)y;

  //camera.MouseRotate(deltaX, deltaY);
  return 0;
}
