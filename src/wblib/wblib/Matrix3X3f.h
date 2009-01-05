#ifndef _MATRIX3F
#define _MATRIX3F

#include <iostream>

namespace wbLib {

  class Matrix3f {
  public:
	  void SetXRot(bool _plus);
	  void SetYRot(bool _plus);
	  void SetZRot(bool _plus);
	  void Mult(float * _x, float * _y, float * _z);
	  float elements[3][3];
  };
}

#endif

