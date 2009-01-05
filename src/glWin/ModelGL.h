#ifndef _MODEL_GL_H
#define _MODEL_GL_H

#include <iostream>
#include <fstream>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <iomanip>
#include "../wblib/wblib/ROAM.h"
#include "../wblib/wblib/FreeFlightCamera.h"

class ModelGL {
public:
  ModelGL();
  ~ModelGL();

  void init();
  //void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
  //void setCamera(wbLib::Camera * _camera);
  void setViewport(int width, int height);
  void draw();

  void rotateCamera(int x, int y);
  void zoomCamera(int dist);

  void load(const TCHAR * _fileName) throw (std::ios_base::failure);
  void cleanUp();

protected:

private:

  int windowWidth;
  int windowHeight;    
  float cameraAngleX;
  float cameraAngleY;
  float cameraDistance;

  log4cplus::Logger logger;
  
  
};

#endif
