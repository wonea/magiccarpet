#include "FreeFlightCamera.h"

wbLib::FreeFlightCamera::FreeFlightCamera(float _x, float _y, float _z) : Camera(_x, _y, _z) {
}

/**
 * Moves the camera forward
 *
 * FreeFlight means, the camera moves along it's view direction
 */
void wbLib::FreeFlightCamera::MoveForward() {
	wbLib::Vector3f temp = this->view;
	temp.Normalize();
	temp.ScalarMult(this->moveSpeed);
	// By moving in the view direction, this camera
	// behaves like free flight
	this->pos += temp;
}

/**
 * Moves the camera back
 *
 * FreeFlight means, the camera moves along it�s view direction
 */
void wbLib::FreeFlightCamera::MoveBackward() {
	wbLib::Vector3f temp = this->view;
	temp.Normalize();
	temp.ScalarMult(this->moveSpeed);
	// By moving in the view direction, this camera
	// behaves like free flight
	this->pos -= temp;
}
