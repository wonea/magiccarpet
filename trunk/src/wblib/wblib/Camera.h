#ifndef _CAMERA_H
#define _CAMERA_H

#include <math.h>
#include <list>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Maths.h"

// To prevent strange flipping of view direction, the camera has to be
// bound to a certain maximal angle for looking up and down
#define CAM_MAX_Y_LOOK 1.4f

namespace wbLib {
  class Result {
  public:
    float x;
    float y;
  };

  /**
   * USAGE: 
   * 1. Create variable and initialize with initial position
   * Camera g_Camera(0.0f, 0.0f, 0.0f);
   *
   * 2. To update the camera's view direction, call MouseRotate(long x, long y);
   * g_Camera.MouseRotate(mouse_state.lX, mouse_state.lY);
   * The parameters to MouseRotate() are the deltas of mouse movement.
   *
   * 3. To update camera's position call
   * g_Camera.MoveForward();
   * g_Camera.MoveBackward();
   * g_Camera.StrafeLeft();
   * g_Camera.StrafeRight();
   *
   * 4. If the camera is too fast, adjust the public variables xLookSpeed and
   * yLookSpeed
   */
  class Camera {
  public:
    Camera();
	  Camera(float x, float y, float z);
	  void MouseRotate(float _x, float _y);
	  void MouseUpdate(long x, long y);
	  virtual void MoveForward() = 0;
	  virtual void MoveBackward() = 0;
	  void StrafeLeft();
	  void StrafeRight();
	  void ResetCamera();
  	
	  void AddView(float xAngle, float yAngle);
  	
	  void PrepareAnimate();
	  void Animate();
	  void ClearUpAnimationList();

  protected:
	  void UpdateCamera();
	  void SetPrespective(float m_PitchDegrees, float m_HeadingDegrees);
	  Result Filter(long x, long y);
	  void PrepareViewingMatrix(GLdouble _eyeX, GLdouble _eyeY, GLdouble _eyeZ,
      GLdouble _centerX, GLdouble _centerY, GLdouble _centerZ, 
      GLdouble _upX, GLdouble _upY, GLdouble _upZ);

  public:
	  Vector3f initialPos;
	  Vector3f pos;
	  Vector3f view; // view direction
	  Vector3f up;
	  Vector3f perp;
	  Vector3f eyesephalfvector;

	  float thetaX;
	  float thetaY;
	  float controlThetaY;
  	
	  float xAngle;
	  float yAngle;	
  	
	  double focallength;
    double aperture;
    double eyesep;
    double dNear;
    double dFar;

    int screenheight;
    int screenwidth;
     
    std::list<Result *> animationList;
    
    float xLookSpeed;
	  float yLookSpeed;
	  float moveSpeed;	

  protected:
	  int index;
	  float xbuffer[10];
	  float ybuffer[10];
	  float strength[10];

	  unsigned int step;
  	
	  Quaternion startRotAroundUpByThetaX;
	  Quaternion startRotAroundPerpByThetaY;
  	
	  Quaternion endRotAroundUpByThetaX;
	  Quaternion endRotAroundPerpByThetaY;
  	
	  float t;
	  bool increase;	
  };
}

#endif
