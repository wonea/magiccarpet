//////////////////////////////////////////////////////////////////////////////////////////
//	Vector4f.h
//	Class declaration for a 4d vector
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//	Modified:	8th November 2002	-	Changed Constructor layout
//									-	Some speed Improvements
//									-	Corrected Lerp
//				7th January 2003	-	Added QuadraticInterpolate
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#ifndef _VECTOR4F
#define _VECTOR4F

#include "Vector3f.h"

namespace wbLib {

  class Vector4f
  {
  public:
	  //constructors
	  wbLib::Vector4f(void)	:	x(0.0f), y(0.0f), z(0.0f), w(0.0f)
	  {}

	  Vector4f(float newX, float newY, float newZ, float newW)	
		  :	x(newX), y(newY), z(newZ), w(newW)
	  {}

	  Vector4f(const float * rhs)	:	x(*rhs), y(*(rhs+1)), z(*(rhs+2)), w(*(rhs+3))
	  {}

	  Vector4f(const Vector4f & rhs):	x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
	  {}

	  /*//convert v3d to v4d
	  Vector4f(const VECTOR3D & rhs):	x(rhs.x), y(rhs.y), z(rhs.z), w(1.0f)
	  {}*/
  	
	  //convert Vector3f to v4d
	  wbLib::Vector4f(const Vector3f & rhs):	x(rhs.x), y(rhs.y), z(rhs.z), w(1.0f)
	  {}

	  ~Vector4f() {}	//empty

	  void Set(float newX, float newY, float newZ, float newW)
	  {	x=newX;	y=newY;	z=newZ; w=newW;	}
  	
	  //accessors kept for compatability
	  void SetX(float newX) {x = newX;}
	  void SetY(float newY) {y = newY;}
	  void SetZ(float newZ) {z = newZ;}
	  void SetW(float newW) {w = newW;}
  	
	  float GetX() const {return x;}	//public accessor functions
	  float GetY() const {return y;}	//inline, const
	  float GetZ() const {return z;}
	  float GetW() const {return w;}

	  void LoadZero(void)
	  {	x=0.0f; y=0.0f; z=0.0f; w=0.0f;	}

	  void LoadOne(void)
	  {	x=1.0f; y=1.0f; z=1.0f; w=1.0f;	}

	  //vector algebra
	  float DotProduct(const wbLib::Vector4f & rhs)
	  {	return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w;	}

	  //rotations
	  void RotateX(double _angle);
	  //Vector4f GetRotatedX(double angle) const;
	  void RotateY(double _angle);
	  //Vector4f GetRotatedY(double angle) const;
	  void RotateZ(double _angle);
	  //Vector4f GetRotatedZ(double angle) const;
	  //void RotateAxis(double angle, const VECTOR3D & axis);
	  //Vector4f GetRotatedAxis(double angle, const VECTOR3D & axis) const;
  	
	  wbLib::Vector4f lerp(const wbLib::Vector4f & _v2, float _factor) const
	  {	return (*this)*(1.0f-_factor)+_v2*_factor;	}

	  wbLib::Vector4f QuadraticInterpolate(const wbLib::Vector4f & _v2, const wbLib::Vector4f & _v3, float _factor) const
	  {	return (*this)*(1.0f-_factor)*(1.0f-_factor) + 2*_v2*_factor*(1.0f-_factor) + _v3*_factor*_factor;}

	  //binary operators
	  wbLib::Vector4f operator+(const wbLib::Vector4f & rhs) const
	  {	return wbLib::Vector4f(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w);	}

	  wbLib::Vector4f operator-(const wbLib::Vector4f & rhs) const
	  {	return Vector4f(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w);	}

	  wbLib::Vector4f operator*(const float rhs) const
	  {	return Vector4f(x*rhs, y*rhs, z*rhs, w*rhs);	}

	  wbLib::Vector4f operator/(const float rhs) const
	  {	return rhs==0.0f	?	wbLib::Vector4f(0.0f, 0.0f, 0.0f, 0.0f)
							  :	wbLib::Vector4f(x/rhs, y/rhs, z/rhs, w/rhs);	}

	  //multiply by a float, eg 3*v
	  friend wbLib::Vector4f operator*(float scaleFactor, const wbLib::Vector4f & rhs);

	  bool operator==(const wbLib::Vector4f & rhs) const;
	  bool operator!=(const wbLib::Vector4f & rhs) const
	  {	return !((*this)==rhs);	}

	  //self-add etc
	  void operator+=(const wbLib::Vector4f & rhs)
	  {	x+=rhs.x; y+=rhs.y; z+=rhs.z; w+=rhs.w;	}

	  void operator-=(const wbLib::Vector4f & rhs)
	  {	x-=rhs.x; y-=rhs.y; z-=rhs.z; w-=rhs.w;	}

	  void operator*=(const float rhs)
	  {	x*=rhs; y*=rhs; z*=rhs; w*=rhs;	}

	  void operator/=(const float rhs)
	  {	if(rhs==0.0f)
			  return;
		  else
		  {	x/=rhs; y/=rhs; z/=rhs; w/=rhs;	}
	  }

	  //unary operators
	  wbLib::Vector4f operator-(void) const {return wbLib::Vector4f(-x, -y, -z, -w);}
	  wbLib::Vector4f operator+(void) const {return (*this);}

	  //cast to pointer to float for glVertex4fv etc
	  operator float* () const {return (float*) this;}
	  operator const float* () const {return (const float*) this;}

	  //operator VECTOR3D();							//convert v4d to v3d

	  //member variables
	  float x;
	  float y;
	  float z;
	  float w;
  };
}

#endif

