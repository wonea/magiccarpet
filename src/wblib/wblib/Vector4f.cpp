//////////////////////////////////////////////////////////////////////////////////////////
//	Vector4f.cpp
//	Function definitions for 4d vector class
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//	Modified:	15th August 2002	-	prevent divide by zero in operator VECTOR3D()
//	Modified:	8th November 2002	-	Changed Constructor layout
//									-	Some speed Improvements
//									-	Corrected Lerp
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#include "Maths.h"

/*void Vector4f::RotateX(double angle)
{
	(*this)=GetRotatedX(angle);
}

Vector4f Vector4f::GetRotatedX(double angle) const
{
	VECTOR3D v3d(x, y, z);

	v3d.RotateX(angle);

	return Vector4f(v3d.x, v3d.y, v3d.z, w);
}

void Vector4f::RotateY(double angle)
{
	(*this)=GetRotatedY(angle);
}

Vector4f Vector4f::GetRotatedY(double angle) const
{
	VECTOR3D v3d(x, y, z);

	v3d.RotateY(angle);

	return Vector4f(v3d.x, v3d.y, v3d.z, w);
}

void Vector4f::RotateZ(double angle)
{
	(*this)=GetRotatedZ(angle);
}

Vector4f Vector4f::GetRotatedZ(double angle) const
{
	VECTOR3D v3d(x, y, z);

	v3d.RotateZ(angle);

	return Vector4f(v3d.x, v3d.y, v3d.z, w);
}

void Vector4f::RotateAxis(double angle, const VECTOR3D & axis)
{
	(*this)=GetRotatedAxis(angle, axis);
}

Vector4f Vector4f::GetRotatedAxis(double angle, const VECTOR3D & axis) const
{
	VECTOR3D v3d(x, y, z);

	v3d.RotateAxis(angle, axis);

	return Vector4f(v3d.x, v3d.y, v3d.z, w);
}*/


wbLib::Vector4f operator*(float scaleFactor, const wbLib::Vector4f & rhs)
{
	return rhs*scaleFactor;
}

bool wbLib::Vector4f::operator==(const wbLib::Vector4f & rhs) const
{
	if(x==rhs.x && y==rhs.y && z==rhs.z && w==rhs.w)
		return true;

	return false;
}

/*Vector4f::operator VECTOR3D()
{
	if(w==0.0f || w==1.0f)
		return VECTOR3D(x, y, z);
	else
		return VECTOR3D(x/w, y/w, z/w);
}*/
