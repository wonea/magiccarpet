#include "Matrix3X3f.h"

// Precomputes values for a 90 degree rotation matrix around the z axis
// if parameter _plus is true, the matrix rotates +90 degrees, -90 otherwise
void wbLib::Matrix3f::SetXRot(bool _plus) {
	if (_plus) {
		elements[0][0] = 1.0f;
		elements[0][1] = 0.0f;
		elements[0][2] = 0.0f;

		elements[1][0] = 0.0f;
		elements[1][1] = 0.0f;
		elements[1][2] = 1.0f;

		elements[2][0] = 0.0f;
		elements[2][1] = -1.0f;
		elements[2][2] = 0.0f;
	} else {
		elements[0][0] = 1.0f;
		elements[0][1] = 0.0f;
		elements[0][2] = 0.0f;

		elements[1][0] = 0.0f;
		elements[1][1] = 0.0f;
		elements[1][2] = -1.0f;

		elements[2][0] = 0.0f;
		elements[2][1] = 1.0f;
		elements[2][2] = 0.0f;
	}
}

// Precomputes values for a 90 degree rotation matrix around the z axis
// if parameter _plus is true, the matrix rotates +90 degrees, -90 otherwise
void wbLib::Matrix3f::SetYRot(bool _plus) {
	if (_plus) {
		elements[0][0] = 0.0f;
		elements[0][1] = 0.0f;
		elements[0][2] = -1.0f;

		elements[1][0] = 0.0f;
		elements[1][1] = 1.0f;
		elements[1][2] = 0.0f;

		elements[2][0] = 1.0f;
		elements[2][1] = 0.0f;
		elements[2][2] = 0.0f;
	} else {
		elements[0][0] = 0.0f;
		elements[0][1] = 0.0f;
		elements[0][2] = 1.0f;

		elements[1][0] = 0.0f;
		elements[1][1] = 1.0f;
		elements[1][2] = 0.0f;

		elements[2][0] = -1.0f;
		elements[2][1] = 0.0f;
		elements[2][2] = 0.0f;
	}
}

// Precomputes values for a 90 degree rotation matrix around the z axis
// if parameter _plus is true, the matrix rotates +90 degrees, -90 otherwise
void wbLib::Matrix3f::SetZRot(bool _plus) {
	if (_plus) {
		elements[0][0] = 0.0f;
		elements[0][1] = 1.0f;
		elements[0][2] = 0.0f;

		elements[1][0] = -1.0f;
		elements[1][1] = 0.0f;
		elements[1][2] = 0.0f;

		elements[2][0] = 0.0f;
		elements[2][1] = 0.0f;
		elements[2][2] = 1.0f;
	} else {
		elements[0][0] = 0.0f;
		elements[0][1] = -1.0f;
		elements[0][2] = 0.0f;

		elements[1][0] = 1.0f;
		elements[1][1] = 0.0f;
		elements[1][2] = 0.0f;

		elements[2][0] = 0.0f;
		elements[2][1] = 0.0f;
		elements[2][2] = 1.0f;
	}
}

void wbLib::Matrix3f::Mult(float * _x, float * _y, float * _z) {
	//std::cout << (*_x) << " " << (*_y) << " " << (*_z) << std::endl;

	float tempX = (*_x)*elements[0][0] + (*_y)*elements[1][0] + (*_z)*elements[2][0];
	float tempY = (*_x)*elements[0][1] + (*_y)*elements[1][1] + (*_z)*elements[2][1];
	float tempZ = (*_x)*elements[0][2] + (*_y)*elements[1][2] + (*_z)*elements[2][2];

	(*_x) = tempX;
	(*_y) = tempY;
	(*_z) = tempZ;

	//std::cout << (*_x) << " " << (*_y) << " " << (*_z) << std::endl;
}

