#include "Vector3f.h"

/*Vector3f::Vector3f() {
  this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
}*/

wbLib::Vector3f::Vector3f(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

wbLib::Vector3f::Vector3f(float * _coords) {
  this->x = _coords[0];
	this->y = _coords[1];
	this->z = _coords[2];
}

wbLib::Vector3f wbLib::Vector3f::CrossProduct(Vector3f other) {
	return Vector3f(
		(this->y*other.z) - (this->z*other.y),
		(this->z*other.x) - (this->x*other.z),
		(this->x*other.y) - (this->y*other.x));
}

float wbLib::Vector3f::DotProduct(Vector3f other) {
  return (this->x*other.x + this->y*other.y + this->z*other.z);
}

wbLib::Vector3f wbLib::Vector3f::ScalarMult(float f) {
	return Vector3f(this->x *= f, this->y *= f,	this->z *= f);	
}

float wbLib::Vector3f::Magnitude() {
	return ::sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
}

void wbLib::Vector3f::Normalize() {
	float mag = this->Magnitude();
	this->x /= mag;
	this->y /= mag;
	this->z /= mag;
}

/* if the points are given in ccw order from the viewers viewpoint, 
   then the resulting normal will be oriented towards the viewer 
void calculateNormal(float *v1, float *v2, float *v3, float *normal) {
  
  float a[3], b[3];
  int length;

  a[0] = v1[0] - v2[0];
  a[1] = v1[1] - v2[1];
  a[2] = v1[2] - v2[2];

  b[0] = v3[0] - v2[0];
  b[1] = v3[1] - v2[1];
  b[2] = v3[2] - v2[2];

  normal[0] = a[1] * b[2] - a[2] * b[1];
  normal[1] = a[2] * b[0] - a[0] * b[2];
  normal[2] = a[0] * b[1] - a[1] * b[0];

  length = (float)sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);

  normal[0] /= length;
  normal[1] /= length;
  normal[2] /= length;
}*/

wbLib::Vector3f& wbLib::Vector3f::operator+=(const Vector3f& rhs) {
  this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

wbLib::Vector3f& wbLib::Vector3f::operator-=(const Vector3f& rhs) {
  this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

wbLib::Vector3f& wbLib::Vector3f::operator=(const Vector3f& rhs) {
  this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	return *this;
}

const wbLib::Vector3f wbLib::Vector3f::operator - (const Vector3f& v) const {

  // use copy operator
  wbLib::Vector3f t = wbLib::Vector3f(*this);
  
  // change the copied vector and return it
  return (t -= v);
}

const wbLib::Vector3f wbLib::Vector3f::operator + (const Vector3f& _v) const {
   wbLib::Vector3f t = wbLib::Vector3f(*this);
   return (t += _v);
}

//const Vector3f Vector3f::operator * (float val) const
//{
//   Vector3f t = Vector3f(*this);
//   return (t *= val);
//}

wbLib::Vector3f operator * (const wbLib::Vector3f& _a, const float _scale) {
  wbLib::Vector3f v;
  v.x = _a.x * _scale;
  v.y = _a.y * _scale;
  v.z = _a.z * _scale;
  return v;
}

 wbLib::Vector3f operator * (const float _scale, const wbLib::Vector3f& _a) {
    wbLib::Vector3f v;
    v.x = _a.x * _scale;
    v.y = _a.y * _scale;
    v.z = _a.z * _scale;
    return v;
 }
   

wbLib::Vector3f& wbLib::Vector3f::operator *= (float _val) {
   x *= _val;
   y *= _val;
   z *= _val;
   return *this;
}
