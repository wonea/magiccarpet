#ifndef VECTOR3F_H
#define VECTOR3F_H

#include <math.h>
#include <iostream>
#include <exception>

#include "UserDefinedExceptions.h"

/*
Vector3f v;
  std::cout << (float)v.x << " | " << (float)v.y << " | " << (float)v.z << std::endl;
  
  float f[3] = {1.0f, 1.0f, 1.0f};  
  std::cout << (float)v.x << " | " << (float)v.y << " | " << (float)v.z << std::endl;
  
  v[0] = 4.0f;
  v[1] = 4.0f;
  v[2] = 4.0f;
  std::cout << (float)v.x << " | " << (float)v.y << " | " << (float)v.z << std::endl;
  
  v.Set(0.4f, 1.9f, 0.0f);
  std::cout << (float)v.x << " | " << (float)v.y << " | " << (float)v.z << std::endl;
*/

namespace wbLib {
  class Vector3f {
  public:
    //Vector3f();
	  Vector3f(float x = 0, float y = 0, float z = 0);
	  Vector3f CrossProduct(Vector3f other);
	  float DotProduct(Vector3f other);
	  Vector3f ScalarMult(float f);
	  void Normalize();
	  float Magnitude();
	  Vector3f& operator+=(const Vector3f &rhs);
	  Vector3f& operator-=(const Vector3f &rhs);
	  Vector3f& operator=(const Vector3f& rhs);
	  const Vector3f operator - (const Vector3f& v) const;
	  const Vector3f operator + (const Vector3f& v) const;
	  //const Vector3f operator * (float val) const;
	  friend Vector3f operator * (const Vector3f& a, const float scale);
	  friend Vector3f operator * (const float scale, const Vector3f& a);
	  Vector3f& operator *= (float val);
  	
	  void Set(float _x, float _y, float _z) {
	    this->x = _x;
	    this->y = _y;
	    this->z = _z;
	  }
  	
	  // cast to pointer to float for glVertex3fv etc
	  operator float* () const {return (float*) this;}
	  operator const float* () const {return (const float*) this;}
  	
	  // allows for: 
	  // Vector3f v;
	  // float f[3] = {1.0f, 1.0f, 1.0f};
    // v = f;
	  void operator=(float* f) {
      //std::cout << "Vector3f::operator=(float*)" << std::endl;
      this->x = f[0];
      this->y = f[1];
      this->z = f[2];
    }
    
    // allows for: 
    // v[0] = 4.0f;
    float& operator[](int i) {
      switch (i) {
        case 0:
          return (this->x);
          break;
        case 1:
          return (this->y);
          break;
        case 2:
          return (this->z);
          break;
          
        default:
          throw BadIndex();
          break;
      }
    }
    
    //float operator[](int i) const {
    //  return this->x;
    //}
    
    // allows for:
    // Vector3f v(1.1f, 0.4f, 2.3f);
    // std::cout << v << std::endl;
    friend std::ostream &operator<< (std::ostream &os, const Vector3f &v) {  
	    os << (float)v.x << "|" <<
	    (float)v.y << "|" <<
	    (float)v.z << std::endl;
  	  
	    return os;
	  }
    
  public:
	  float x;
	  float y;
	  float z;
  };
}

#endif

