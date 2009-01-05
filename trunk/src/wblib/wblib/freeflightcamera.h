#ifndef FREEFLIGHTCAMERA_H
#define FREEFLIGHTCAMERA_H

#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <list>

#include "Camera.h"
#include "Maths.h"

// To prevent strange flipping of view direction, the camera has to be
// bound to a certain maximal angle for looking up and down
#define CAM_MAX_Y_LOOK 1.4f

namespace wbLib {
  /**
   * USAGE: 
   * 1. Create variable and initialize with initial position
   * Camera g_Camera(0.0f, 0.0f, 0.0f);
   *
   * 2. To update the camera's view direction, call MouseRotate(long x, long y);
   * g_Camera.MouseRotate(mouse_state.lX, mouse_state.lY);
   *
   * 3. To update camera's position call
   * g_Camera.MoveForward();
   * g_Camera.MoveBackward();
   * g_Camera.StrafeLeft();
   * g_Camera.StrafeRight();
   *
   * 4. To adjust the OpenGL Scene to the Camera's position and angles, call
   * void ApplyCamera(); inside the rendering before all geometry is rendered
   * g_Camera.ApplyCamera();
   */
  class FreeFlightCamera : public wbLib::Camera {
  public:
    FreeFlightCamera(float _x, float _y, float _z);
	  void MoveForward();
	  void MoveBackward();
  };
}

#endif
