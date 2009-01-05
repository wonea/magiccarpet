///////////////////////////////////////////////////////////////////////////////
// ModelGL.cpp
// ===========
// Model component of OpenGL
// 
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2006-07-10
// UPDATED: 2006-07-10
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#if _WIN32
  #include <windows.h>    // include windows.h to avoid thousands of compile errors even though this class is not depending on Windows
#endif
#include <GL/glew.h>
#include <GL/glut.h>
#include "ModelGL.h"
#include "Bmp.h"

ModelGL::ModelGL() : logger(log4cplus::Logger::getInstance(L"loggerName")) {
}

ModelGL::~ModelGL() {
}

void ModelGL::load(const TCHAR * _fileName) throw (std::ios_base::failure) {

  int bufferSize = 0;
  std::ifstream filestream;
  char * buffer;

  std::cout << __FILE__ << " " << __LINE__ << " " << _fileName << std::endl;

  // stop rendering, free currently loaded map, reset roam algorithm

  // load new map 
  buffer = NULL;
  
	filestream.open(_fileName, std::ios::in | std::ios::binary | std::ios::ate);
	if (!filestream.is_open()) {
    std::ios_base::failure f("Could not find file!");
    throw f;
	}
	bufferSize = filestream.tellg();
	filestream.seekg (0, std::ios::beg);
	buffer = new char[bufferSize];
	filestream.read(buffer, bufferSize);
	if (!filestream.good()) {
		delete[] buffer;
		buffer = NULL;
	}
	filestream.close();

  // only square maps, only square patches
  int patchCount = 16;
  int mapSize = 1024;
  
  //landscape.init(mapSize, buffer, patchCount);

  // start rendering
}

void ModelGL::cleanUp() {
  //landscape.cleanUp();
}

void ModelGL::init() {
  //glShadeModel(GL_SMOOTH);                        // shading mathod: GL_SMOOTH or GL_FLAT
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);          // 4-byte pixel alignment

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_LIGHTING);
  //glEnable(GL_TEXTURE_2D);
  //glEnable(GL_CULL_FACE);
  //glEnable(GL_BLEND);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  //glEnable(GL_COLOR_MATERIAL);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);
  glDepthFunc(GL_LEQUAL);
  //setCamera(0, 0, 8, 0, 0, 0);
}

//void ModelGL::setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ) {
//  glMatrixMode(GL_MODELVIEW);
//  glLoadIdentity();
//  gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
//}
//void ModelGL::setCamera(wbLib::Camera * _camera) {
//  camera = _camera;
//}

void ModelGL::setViewport(int w, int h) {
  // assign the width/height of viewport
  windowWidth = w;
  windowHeight = h;    

  // set viewport to be the entire window
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  // set perspective viewing frustum
  float aspectRatio = (float)w / h;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0f, (float)(w)/h, 0.1f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip

  // switch to modelview matrix in order to set scene
  glMatrixMode(GL_MODELVIEW);
}

void ModelGL::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  ::POINT point;
  ::GetCursorPos(&point);

  //std::cout << "x: " << point.x << " y: " << point.y << std::endl;

  /*float deltaX = (float)point.x - (float)oldX;
  float deltaY = (float)point.y - (float)oldY;
  oldX = (float)point.x;
  oldY = (float)point.y;*/

  //std::cout << "deltaX: " << deltaX << " deltaY: " << deltaY << std::endl;

  //if ((deltaX != 0.0f) || (deltaY != 0.0f)) {
  //  std::cout << "move" << std::endl;
  //  oldX = point.x;
  //  oldY = point.y;

  //  //camera->MouseRotate(deltaX, deltaY);    
  //  camera->xAngle = 1.0f;//deltaX*5.0f;
  //  camera->yAngle = 1.0f;//deltaY*5.0f;
  //}
  //camera->xAngle = (deltaX * 5.0f);
  //camera->yAngle = ( * 5.0f);
  //camera.MouseRotate(deltaX, deltaY);
  //camera->xAngle = 0.0f;
  //camera->yAngle = 0.0f;

  // eye center up
  //gluLookAt(0.0f, 0.0f, 10.0f, 
  //  0.0f, 0.0f, 0.0f,
  //  0.0f, 1.0f, 0.0f);

  //glutSolidSphere(1.0f, 5, 5);

  //glTranslatef(0, 0, cameraDistance);
  //glRotatef(cameraAngleX, 1, 0, 0);   // pitch
  //glRotatef(cameraAngleY, 0, 1, 0);   // heading

  // you have to call reset prior to rendering every frame, as every frame
  // the patches need to get a new tesselation. So their base triangles
  // need to be set reset to null pointers, to throw away the complete
  // binary triangle tree from the last frame
  //landscape.reset();
  //landscape.render();
}
