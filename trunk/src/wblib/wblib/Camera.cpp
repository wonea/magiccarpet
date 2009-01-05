#include "Camera.h"

wbLib::Camera::Camera() {
  initialPos.x = 0.0f;
	initialPos.y = 0.0f;
	initialPos.z = 0.0f;  
	ResetCamera();
	strength[0] = 0.55f;
	strength[1] = 0.25f;
	strength[2] = 0.15f;
	strength[3] = 0.3f;
	strength[4] = 0.13f;
	strength[5] = 0.07f;
	strength[6] = 0.0f;
	strength[7] = 0.0f;
	strength[8] = 0.0f;
	strength[9] = 0.0f;
	
	/*// Focal Length along vd
	int mode = 1;
	switch (mode) {
   case 0:
   case 2:
   case 4:
      this->focallength = 10;
      break;
   case 1:
      this->focallength = 5;
      break;
   case 3:
      this->focallength = 15;
      break;
   }
   
  // Non stressful stereo setting
  this->eyesep = this->focallength / 30.0;
  if (mode == 4) {
    this->eyesep = 0;  
  }*/
  aperture = 0.0f;
  eyesep = 0.0f;
  focallength = 1.0f;
  screenheight = 0;
  screenwidth = 0;
  increase = true;
  step = 0;
  
  eyesephalfvector.x = 0.0f;
  eyesephalfvector.y = 0.0f;
  eyesephalfvector.z = 0.0f;
}

wbLib::Camera::Camera(float _x, float _y, float _z) {
	initialPos.x = _x;
	initialPos.y = _y;
	initialPos.z = _z;
	ResetCamera();
	strength[0] = 0.55f;
	strength[1] = 0.25f;
	strength[2] = 0.15f;
	strength[3] = 0.3f;
	strength[4] = 0.13f;
	strength[5] = 0.07f;
	strength[6] = 0.0f;
	strength[7] = 0.0f;
	strength[8] = 0.0f;
	strength[9] = 0.0f;
	
	// Focal Length along vd
	/*int mode = 1;
	switch (mode) {
   case 0:
   case 2:
   case 4:
      this->focallength = 10;
      break;
   case 1:
      this->focallength = 5;
      break;
   case 3:
      this->focallength = 15;
      break;
   }*/
   
  // Non stressful stereo setting
  //this->eyesep = this->focallength / 30.0;
  //if (mode == 4) {
  //  this->eyesep = 0;  
  //}
  aperture = 0.0f;
  eyesep = 0.0f;
  focallength = 1.0f;

  screenheight = 0;
  screenwidth = 0;
}

void wbLib::Camera::MouseUpdate(long x, long y) {
	this->xAngle=(float)x;
	this->yAngle=(float)y;
}

/*void wbLib::Camera::MouseRotateX(long l) {
	this->thetaX = -(float)l * this->xLookSpeed;
	this->UpdateCamera();
}

void wbLib::Camera::MouseRotateY(long l) {
	this->controlThetaY += (((float)l) * this->yLookSpeed);
	//if ((this->controlThetaY >= 0.2f) || (this->controlThetaY <= -0.2f) ) { return; }
	this->thetaY = (float)l * this->xLookSpeed;
	this->UpdateCamera();
}*/

/**
 * Called by ApplyCamera(), applies lookspeed factor
 * to mouse movement, calls UpdateCamera()
 */
/*void wbLib::Camera::MouseRotate(long x, long y) {
	//Result xx = Filter(x, y);	
// 	// Enforce camera bounds for looking up and down to
// 	// prevent strange view direction flipping
// 	if (this->controlThetaY >= CAM_MAX_Y_LOOK)
// 	{ 
// 		this->thetaX = -(float)xx.x * this->xLookSpeed;
// 		this->thetaY = 0.0f;
// 		this->controlThetaY = CAM_MAX_Y_LOOK;
// 		this->UpdateCamera(); 
// 		return; 
// 	} else if  (this->controlThetaY <= -CAM_MAX_Y_LOOK) {
// 		this->thetaX = -(float)xx.x * this->xLookSpeed;
// 		this->thetaY = 0.0f;
// 		this->controlThetaY = -CAM_MAX_Y_LOOK;
// 		this->UpdateCamera(); 
// 		return;
// 	}
	//xx = Filter(x, y);
	//this->thetaX = -(float)xx.x * this->xLookSpeed;
	//this->thetaY = (float)xx.y * this->yLookSpeed;
	//this->UpdateCamera();

	this->thetaX = -(float)x * this->xLookSpeed;
	this->thetaY = (float)y * this->yLookSpeed;
	this->UpdateCamera();
}*/

/**
 * param _x - delta of mouse movement in x direction
 * param _y - delta of mouse movement in y direction
 */
void wbLib::Camera::MouseRotate(float _x, float _y) {
  this->thetaX += _x * this->xLookSpeed;
	this->thetaY += _y * this->yLookSpeed;

  // SetPrespective computes a rotation matrix for a
  // certain amount of degrees 
	this->SetPrespective(this->thetaX, this->thetaY);

	//glTranslatef(-pos.x + eyesephalfvector.x, -pos.y, -pos.z + eyesephalfvector.z);
  glTranslatef(-pos.x, -pos.y, -pos.z);
}

/**
 * from nehe quaterion camera article
 */
void wbLib::Camera::SetPrespective(float _pitchDegrees, float _headingDegrees) {

	double matrix[16];
	wbLib::Quaternion q;
	wbLib::Quaternion qPitch;
	wbLib::Quaternion qHeading;  

	// Make the Quaternions that will represent our rotations
	qPitch.FromAxisAngle(1.0f, 0.0f, 0.0f, _headingDegrees);
	//m_qPitch.FromAxisAngle(1.0f, 0.0f, 0.0f, 0);
	qHeading.FromAxisAngle(0.0f, 1.0f, 0.0f, _pitchDegrees);
	//m_qHeading.FromAxisAngle(0.0f, 1.0f, 0.0f, 0);

	// Combine the pitch and heading rotations and store the results in q
	q = qPitch * qHeading;
	q.ToMatrix4X4f(matrix);

	// Let OpenGL set our new prespective on the world! We do not need a 
	// gluLookAt() call any more!
	glMultMatrixd(matrix);
	
	// Problem right now! The camera looks in the right direction, i.e.
	// viewing transformation is applied, but we have no view vector
	// and not right vector! If this was a spline traveling animation everything
	// was find, but as this is interactive free flight shooter camera, we
	// need view and right for movement in camera direction and strafing!
	//
	// view and right are computed now
	
	// Combine the heading and pitch rotations and make a matrix to get
	// the i and j vectors for our direction.
	//q = m_qHeading * m_qPitch;
	//q.TowbLib::Matrix4X4f(Matrix);
	//m_DirectionVector.i = Matrix[8];
	//m_DirectionVector.k = Matrix[10];
	view.x = matrix[8];
	view.z = (-1.0f)*matrix[10];

	// Create a matrix from the pitch Quaternion and get the j vector
	// for our direction.
	qPitch.ToMatrix4X4f(matrix);
	//m_DirectionVector.j = Matrix[9];
	view.y = matrix[9];
	
	perp = up.CrossProduct(Vector3f(view.x, view.y, view.z));

	// Scale the direction by our speed.
	//m_DirectionVector *= m_ForwardVelocity;

	// Increment our position by the vector
	//this->pos.x = this->view.x;
	//this->pos.y = this->view.y;
	//this->pos.z = this->view.z;

	//// Translate to our new position.
	//glTranslatef(-m_Position.x, -m_Position.y, m_Position.z);
}

wbLib::Result wbLib::Camera::Filter(long x, long y) {
	float xtemp = 0;
	float ytemp = 0;

	xbuffer[index] = x;
	ybuffer[index] = y;
	index++;
	index %= 10;

	/*for (int i = 0; i < 10; i++) {
		temp += buffer[i];
		temp /= 10.0f;
	}*/

	int i = index;
	
	int j = 0;
	do {
		xtemp += xbuffer[i]*(strength[j]);
		ytemp += ybuffer[i]*(strength[j]);
		i++;
	    i %= 10;
		j++;
	} while (i != index);

	xtemp /= 10.0f;
	ytemp /= 10.0f;
	//*x = temp;
	Result tempRes = {xtemp, ytemp};
	return tempRes;
}

/**
 * Moves the camera forward
 *
 * FreeFlight means, the camera moves along it's view direction
 *
void Camera::MoveForward() {
	Vector3f temp = this->view;
	temp.Normalize();
	temp.ScalarMult(this->moveSpeed);
	// By moving in the view direction, this camera
	// behaves like free flight
	this->pos += temp;
}*/

/**
 * Moves the camera back
 *
 * FreeFlight means, the camera moves along it's view direction
 *
void wbLib::Camera::MoveBackward() {
	Vector3f temp = this->view;
	temp.Normalize();
	temp.ScalarMult(this->moveSpeed);
	// By moving in the view direction, this camera
	// behaves like free flight
	this->pos -= temp;
}*/

/**
 * Sideways movement (= strafing)
 */
void wbLib::Camera::StrafeLeft() {
	Vector3f temp = this->perp;
	temp.ScalarMult(this->moveSpeed);
	pos += temp;
}

/**
 * Sideways movement (= strafing)
 */
void wbLib::Camera::StrafeRight() {
	Vector3f temp = this->perp;
	temp.ScalarMult(this->moveSpeed);
	pos -= temp;
}

/**
 * Performs viewing calculation
 *
 * Rotates view vector around world up vector 0 0 1.
 * Then computes perpendicular vector to current view
 * and world up and performs up and down looking around
 * this perpendicular vector, to get the new view vector
 */
void wbLib::Camera::UpdateCamera() {
	// rotate 0 0 -1 around up by angle thetaX 

	// make a quaternion which represents the quaternion
	// out of the axis to rotate abound (up) and the
	// angle to rotate by (thetaX)
	float f = ::sin(thetaX/2.0f);
	wbLib::Quaternion rotAroundUpByThetaX(up.x * f,
		up.y * f,
		up.z * f,
		::cos(thetaX/2.0f));

	//Quaternion viewQuat(this->view.x, this->view.y, this->view.z, 0.0f);

	// Rotate the view vector
	wbLib::Quaternion conjugate(rotAroundUpByThetaX);
	conjugate.Conjugate();
	wbLib::Quaternion result = ((rotAroundUpByThetaX.MultVector3f(this->view)).MultQuaternion(conjugate));

	// compute and normalize perpendicular vector to result and up
	this->perp = this->up.CrossProduct(Vector3f(result.x, result.y, result.z));
	this->perp.Normalize();

	// rotate result around perp by thetaY

	// make a quaternion which represents the quaternion
	// out of the axis to rotate abound (perp) and the
	// angle to rotate by (thetaY)
	float g = ::sin(this->thetaY/2.0f);
	wbLib::Quaternion rotAroundPerpByThetaY(this->perp.x * g,
		this->perp.y * g,
		this->perp.z * g,
		::cos(this->thetaY/2.0f));

	// copy constructor
	wbLib::Quaternion conjugateb(rotAroundPerpByThetaY);
	conjugateb.Conjugate();
	result = ((rotAroundPerpByThetaY.MultQuaternion(result)).MultQuaternion(conjugateb));

	// Quaternion to Vector
	this->view.x = result.x;
	this->view.y = result.y;
	this->view.z = result.z;
}

void wbLib::Camera::PrepareAnimate() {

  if (step == (animationList.size()-1)) {
    step = 0;
  }
  
  std::list<Result *>::iterator it;
  
  it = animationList.begin();
  
  for (unsigned int i = 0; i < step; i++) {
    it++;
  }

  float xAngleStart = (*it)->x;
  float yAngleStart = (*it)->y;
  
  it++;
  if (it == animationList.end()) {
    it = animationList.begin();
  }
  
  float xAngleEnd = (*it)->x;
  float yAngleEnd = (*it)->y;

  // Start RotationQuat for xangle
  float f = ::sin(xAngleStart/2.0f);
	startRotAroundUpByThetaX.x = this->up.x * f;
	startRotAroundUpByThetaX.y = this->up.y * f,
	startRotAroundUpByThetaX.z = this->up.z * f,
	startRotAroundUpByThetaX.w = ::cos(xAngleStart/2.0f);
	
	wbLib::Vector3f tempView (0.0f, 0.0f, -1.0f);
		
  // start RotationQuat for yangle
  
  // copy constructor
	wbLib::Quaternion conjugate(startRotAroundUpByThetaX);
	conjugate.Conjugate();
	wbLib::Quaternion result = ((startRotAroundUpByThetaX.MultVector3f(tempView)).MultQuaternion(conjugate));

	// compute and normalize perpendicular vector to result and up
	wbLib::Vector3f tempPerp = this->up.CrossProduct(Vector3f(result.x, result.y, result.z));
	tempPerp.Normalize();

	// rotate resulting vector around perp by thetaY

	// make a quaternion which represents the quaternion
	// out of the axis to rotate abound (perp) and the
	// angle to rotate by (thetaY)
	float g = ::sin(yAngleStart/2.0f);
	startRotAroundPerpByThetaY.x = tempPerp.x * g;
	startRotAroundPerpByThetaY.y = tempPerp.y * g,
	startRotAroundPerpByThetaY.z = tempPerp.z * g,
	startRotAroundPerpByThetaY.w = ::cos(yAngleStart/2.0f);

  // End RotationQuat for xangle
  f = ::sin(xAngleEnd/2.0f);
	endRotAroundUpByThetaX.x = this->up.x * f;
	endRotAroundUpByThetaX.y = this->up.y * f;
	endRotAroundUpByThetaX.z = this->up.z * f;
	endRotAroundUpByThetaX.w = ::cos(xAngleEnd/2.0f);
		
  // End RotationQuat for yangle
  
  // copy constructor
	conjugate.x = endRotAroundUpByThetaX.x;
	conjugate.y = endRotAroundUpByThetaX.y;
	conjugate.z = endRotAroundUpByThetaX.z;
	conjugate.w = endRotAroundUpByThetaX.w;
	conjugate.Conjugate();
	wbLib::Quaternion resultb = ((endRotAroundUpByThetaX.MultVector3f(tempView)).MultQuaternion(conjugate));

	// compute and normalize perpendicular vector to result and up
	tempPerp = this->up.CrossProduct(Vector3f(resultb.x, resultb.y, resultb.z));
	tempPerp.Normalize();

	// rotate resulting vector around perp by thetaY

	// make a quaternion which represents the quaternion
	// out of the axis to rotate abound (perp) and the
	// angle to rotate by (thetaY)
	g = ::sin(yAngleEnd/2.0f);
	endRotAroundPerpByThetaY.x = tempPerp.x * g;
	endRotAroundPerpByThetaY.y = tempPerp.y * g;
	endRotAroundPerpByThetaY.z = tempPerp.z * g;
	endRotAroundPerpByThetaY.w = ::cos(yAngleEnd/2.0f);
}

void wbLib::Camera::Animate() {
  
  wbLib::Quaternion interpolX;
  interpolX.SLERP(&startRotAroundUpByThetaX, &endRotAroundUpByThetaX, this->t);
  
  wbLib::Quaternion interpolY;
  interpolY.SLERP(&startRotAroundPerpByThetaY, &endRotAroundPerpByThetaY, this->t);
  
  wbLib::Vector3f tempView (0.0f, 0.0f, -1.0f);
  wbLib::Vector3f tempPerp (0.0f, 0.0f, -1.0f);
  
  wbLib::Quaternion conjugate(interpolX);
	conjugate.Conjugate();
	wbLib::Quaternion result = ((interpolX.MultVector3f(tempView)).MultQuaternion(conjugate));

	//// compute and normalize perpendicular vector to result and up
	//tempPerp = this->up.CrossProduct(Vector3f(result.x, result.y, result.z));
	//tempPerp.Normalize();

	// rotate result around perp by thetaY

	//// make a quaternion which represents the quaternion
	//// out of the axis to rotate abound (perp) and the
	//// angle to rotate by (thetaY)
	//float g = ::sin(0.0f/2.0f);
	//Quaternion rotAroundPerpByThetaY(this->perp.x * g,
	//	this->perp.y * g,
	//	this->perp.z * g,
	//	::cos(0.0f/2.0f));

	// copy constructor
	wbLib::Quaternion conjugateb(interpolY);
	conjugateb.Conjugate();
	result = ((interpolY.MultQuaternion(result)).MultQuaternion(conjugateb));

	// Quaternion to Vector
	tempView.x = result.x;
	tempView.y = result.y;
	tempView.z = result.z;	
	
	//std::cout << this->t << std::endl;
	
	gluLookAt(pos.x,
      pos.y,
      pos.z,
			pos.x + tempView.x,
			pos.y + tempView.y,
			pos.z + tempView.z,
			0.0f, 1.0f, 0.0f);
	
	/*if (this->t > 1.0f) {
	  increase = false;
	}
	
	if (this->t < 0.0f) {
	  increase = true;
	}
	
	if (increase) {
	  t += 0.01f;
	} else {
	  t -= 0.01f;
	}*/
	
	if (t > 1.0f) {
	  step++;
	  PrepareAnimate();
	  t = 0.0f;
	}
	
	t += 0.01f;
}

/**
 * Applies the camera's current state to the scene
 */
//void wbLib::Camera::ApplyCamera() {
//  // camera xAngle and camera yAngle are deltas of mouse movement!
//  // camera xAngle and camera yAngle are not automatically reset to 0!
//  // the deltas remain in xAngle and yAngle until a new delta occures and
//  // is written in camera xAngle and camera yAngle to replace the old delta
//	this->MouseRotate(this->xAngle, this->yAngle);
//	
//	// reset to 0 manually
//	this->xAngle = 0.0f;
//	this->yAngle = 0.0f;
//}

/**
 * Von http://wiki.delphigl.com/index.php/gluLookAt
 *
 *     | CenterX - EyeX |
 * F = | CenterY - EyeY |
 *     | CenterZ - EyeZ |
 *
 * Und UP gleich dem Up-Vektor (upX, upY, upZ) ist, dann wird wie folgt normalisiert :
 *
 * f = F / ||F||
 * 
 * und
 *
 * UP'  = UP / ||UP||
 *
 *
 * Wenn dann noch
 *
 * s' = f x UP'
 *
 * s = s' / ||s'||
 *
 * und
 * 
 * u = s x f
 *
 * dann wird die Matrix M wie folgt aussehen :
 *
 *     |  s[0]  s[1]  s[2]  0 |
 * M = |  u[0]  u[1]  u[2]  0 |
 *     | -f[0] -f[1] -f[2]  0 |
 *     |   0     0     0    1 |
 *
 * Diese Matrix transformiert einen Punkt aus dem normalen Koordinatensystem mit
 * den Achsen 1|0|0 0|1|0 und 0|0|1 und das Koordinatensystem, dessen Achsen durch
 * die Achsen der Kamera vorgegeben sind. Die Achsen der Kamera ergeben sich aus
 * dem Up-Vektor, dem Vektor, entlang dem die Kamera schaut (view) und dem 
 * right vektor, der rechtwinklig auf der Ebene steht, die durch up und view 
 * aufgespannt werden.
 * Wenn man einen Punkt des normalen Koordinatensystems mit der Matrix M
 * multipliziert, hat man seine Koordinaten im neuen Koordinatensystem. Wenn
 * man diese Koordinaten nimmt und einen Punkt mit diesen Koordinaten im normalen
 * Koordinatensystem malt, so hat sich dieser Punkt verschoben. Dies ist der
 * Effekt der Viewing transformation.
 * Diese Matrix M hat die Form:
 *     |  right[0]  right[1]  right[2]  0 |
 * M = |  up[0]     up[1]     up[2]     0 |
 *     | -look[0]   -look[1]  -look[2]  0 |
 *     |   0     0     0    1 |
 *
 * wobei die Methode allerdings leider nur in _eye den CameraPos Punkte,
 * in _center, den Punkt auf den die Kamera schaut und in 
 * _up den Aufvektor erhaelt.
 *
 * Um das normalisierte Koordinatensystem der Kamera zu berechnen, muss der view vector als
 * _center - _eye berechnet und normalisiert werden. 
 *
 * Der _up vector, der als Parameter uebernommen wird, steht noch nicht in jedem Fall im rechten Winkel
 * auf dem view vector, wie es fuer die Matrix benoetigt wird. Um einen solchen rechtwinkligen
 * up vektor zu erhalten, wird der _up vector normalisiert und es wird dann der right vector
 * per Kreuzprodukt aus view und up berechnet. Man nimmt also an, das view und up in einer
 * Ebene liegen und bildet die Normale dieser Ebene, die der right vector ist.
 *
 * Nun berechnet man einen neuen up vector, der den _up vector in der Matrix vertritt.
 * Dieser neue up vector wird durch Kreuzprodukt aus view und right berechnet!
 * Er ist bereits normalisiert und steht durch das Kreuzprodukt senkrecht auf _view und
 * right.
 *
 * Durch diese Schritte hat man aus den Parametern zu gluLookAt() eine orthonormales
 * Koordinatensystem der Kamera berechnet. Dieses Koordinatensystem, bzw. seine
 * Achsen, in eine Matrix eingetragen, ergeben die Viewtransformation matrix! 
 *
 * und gluLookAt ist dann gleich:
 *
 * glMulMatrixf(@M[0,0]);	
 * glTranslated(-EyeX, -EyeY, -EyeZ);
 */
void wbLib::Camera::PrepareViewingMatrix(GLdouble _eyeX, GLdouble _eyeY, GLdouble _eyeZ,
  GLdouble _centerX, GLdouble _centerY, GLdouble _centerZ, 
  GLdouble _upX, GLdouble _upY, GLdouble _upZ)
{
  double F[3];
  double f[3];
  double UPNormalized[3];
  double s[3];
  double SNormalized[3];
  double u[3];
  double mag;
  double matrix[16];
  
  F[0] = _centerX - _eyeX;
  F[1] = _centerY - _eyeY;
  F[2] = _centerZ - _eyeZ;
  
  mag = ::sqrt(F[0]*F[0] + F[1]*F[1] + F[2]*F[2]);
  f[0] = F[0]/mag;
  f[1] = F[1]/mag;
  f[2] = F[2]/mag;
  
  mag = ::sqrt(_upX*_upX + _upY*_upY + _upZ*_upZ);
  UPNormalized[0] = _upX / mag;
  UPNormalized[1] = _upY / mag;
  UPNormalized[2] = _upZ / mag;
  
  s[0] = (f[1]*UPNormalized[2]) - (f[2]*UPNormalized[1]);
	s[1] = (f[2]*UPNormalized[0]) - (f[0]*UPNormalized[2]);
	s[2] = (f[0]*UPNormalized[1]) - (f[1]*UPNormalized[0]);
	
	mag = ::sqrt(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]);
	SNormalized[0] = s[0] / mag;
	SNormalized[1] = s[1] / mag;
	SNormalized[2] = s[2] / mag;
	
	u[0] = (SNormalized[1]*f[2]) - (SNormalized[2]*f[1]);
	u[1] = (SNormalized[2]*f[0]) - (SNormalized[0]*f[2]);
	u[2] = (SNormalized[0]*f[1]) - (SNormalized[1]*f[0]);
	
	matrix[0] = SNormalized[0];
	matrix[4] = SNormalized[1];
	matrix[8] = SNormalized[2];
	matrix[12] = 0.0;
	
	matrix[1] = u[0];
	matrix[5] = u[1];
	matrix[9] = u[2];
	matrix[13] = 0.0;
	
	matrix[2] = -f[0];
	matrix[6] = -f[1];
	matrix[10] = -f[2];
	matrix[14] = 0.0;
	
	matrix[3] = 0.0;
	matrix[7] = 0.0;
	matrix[11] = 0.0;
	matrix[15] = 1.0;
	
	glMultMatrixd(matrix);
	
	glTranslated(-_eyeX, -_eyeY, -_eyeZ);
}

/**
 * Reset camera to the state it had, when it was created
 */
void wbLib::Camera::ResetCamera() {
	pos = initialPos;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	view.x = 0.0f;
	view.y = 0.0f;
	view.z = -1.0f;

	perp.x = -1.0f;
	perp.y = 0.0f;
	perp.z = 0.0f;

	controlThetaY = thetaX = thetaY = 0.0f;

	xLookSpeed = 0.3;
	yLookSpeed = 0.3;
	
	moveSpeed = 0.1;
	//this->moveSpeed = 6.4;

	index = 0;
	for (int i = 0; i < 10; i++) {
		xbuffer[i] = ybuffer[i] = 0;
		
	}

	xAngle = 0.0f;
	yAngle = 0.0f;
	step = 0;
}

void wbLib::Camera::AddView(float xAngle, float yAngle) {
  Result * r = new Result;
  r->x = xAngle;
  r->y = yAngle;
  animationList.push_back(r);
}

void wbLib::Camera::ClearUpAnimationList() {
  if (animationList.size() == 0) {
    return;
  }
  
  bool done = false;
	std::list<Result *>::iterator it;
	std::list<Result *>::iterator tempit;

	

	/*for (it = stoneList.begin(); it != stoneList.end(); it++) {
		(*it)->ClearPartList();
		Stone * stonePtr = (Stone*)(*it);
		delete stonePtr;
		stoneList.remove(*it);
	}*/
	
	it = animationList.begin();
	while (!done) {	
	  tempit = it;
	  // if it == partList.end() it doesn't point on any object.
	  // Then it is invalid and points "behind" the last object of the list
	  if (it == animationList.end()) {
	    done = true;
	    continue;
	  }
	  it++;
	  //(*tempit)->ClearPartList();
	  delete (Result *)(*tempit);
		animationList.remove(*tempit);
	}
	
	step = 0;
}
