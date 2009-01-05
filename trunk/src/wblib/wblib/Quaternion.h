#ifndef _QUATERNION
#define _QUATERNION

#include <math.h>
#include <iostream>
#include "Maths.h"

namespace wbLib {
  class Quaternion {
  public:
	  Quaternion(const Quaternion& quaternion);
	  Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f);
	  float Length();
	  void Normalize();
	  void Conjugate();
	  void Dump();
	  /**
	   * Reformulates the vector parameter as quaternion and multiplies this
	   * quaternion to the reformulated vector and returns the resulting
	   * quaternion.
	   */
	  Quaternion MultVector3f(Vector3f vector);
	  Quaternion MultQuaternion(Quaternion quaternion);
  	
	  void FromMatrix4X4f(double * _matrix);
	  void ToMatrix4X4f(double * m);
	  //void FromEuler(float _angleX, float _angleY, float _angleZ);
	  void FromEuler(float roll, float pitch, float yaw);
	  void FromAxisAngle(float x, float y, float z, float degrees);
	  Quaternion operator *(Quaternion q);
  	
	  void SLERP(Quaternion * q1, Quaternion * q2, float t);

  public:
	  float x;
	  float y;
	  float z;
	  float w;

  };
}

#endif

