#include "Quaternion.h"

wbLib::Quaternion::Quaternion(const Quaternion& quaternion) {
	this->x = quaternion.x;
	this->y = quaternion.y;
	this->z = quaternion.z;
	this->w = quaternion.w;
}

wbLib::Quaternion::Quaternion(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

float wbLib::Quaternion::Length() {
	return ::sqrt(this->x * this->x + this->y * this->y +
              this->z * this->z + this->w * this->w);
}

void wbLib::Quaternion::Normalize() {
  float L = Length();

  this->x /= L;
  this->y /= L;
  this->z /= L;
  this->w /= L;
}

void wbLib::Quaternion::Conjugate() {
  this->x = -this->x;
  this->y = -this->y;
  this->z = -this->z;
}

wbLib::Quaternion wbLib::Quaternion::MultVector3f(Vector3f vector) {
	// To reformulate a Vector3f as quaternion, w=0 is
	// added as forth component, so we just drop all
	// operations that involve the vectors new w, as they
	// end up being zero anyways
	wbLib::Quaternion C;

	C.x = this->w*vector.x /*+ A.x*B.w*/ + this->y*vector.z - this->z*vector.y;
	C.y = this->w*vector.y - this->x*vector.z /*+ A.y*B.w*/ + this->z*vector.x;
	C.z = this->w*vector.z + this->x*vector.y - this->y*vector.x /*+ A.z*B.w*/;
	C.w = /*A.w*B.w*/ (-(this->x*vector.x)) - this->y*vector.y - this->z*vector.z;

	return C;
}

wbLib::Quaternion wbLib::Quaternion::MultQuaternion(wbLib::Quaternion quaternion) {
	wbLib::Quaternion C;
	C.x = this->w*quaternion.x + this->x*quaternion.w + this->y*quaternion.z - this->z*quaternion.y;
	C.y = this->w*quaternion.y - this->x*quaternion.z + this->y*quaternion.w + this->z*quaternion.x;
	C.z = this->w*quaternion.z + this->x*quaternion.y - this->y*quaternion.x + this->z*quaternion.w;
	C.w = this->w*quaternion.w - this->x*quaternion.x - this->y*quaternion.y - this->z*quaternion.z;
	return C;
}

void wbLib::Quaternion::Dump() {
	std::cout << "[ " << (this->x) << " | " << this->x << " | " << this->z << " | " << this->w << "]" << std::endl;
}

void wbLib::Quaternion::FromMatrix4X4f(double * m) {
  float  tr, s, q[4];
  int    i, j, k;
  int nxt[3] = {1, 2, 0};
  //tr = m[0][0] + m[1][1] + m[2][2];
  tr = m[0 + 0*4] + m[1 + 1*4] + m[2 + 2*4];
  // check the diagonal
  if (tr > 0.0) {
    s = sqrt (tr + 1.0);
    this->w = s / 2.0;
    s = 0.5 / s;
    //this->x = (m[1][2] - m[2][1]) * s;
    //this->y = (m[2][0] - m[0][2]) * s;
    //this->z = (m[0][1] - m[1][0]) * s;
    
    this->x = (m[9] - m[6]) * s;
    this->y = (m[2] - m[8]) * s;
    this->z = (m[4] - m[1]) * s;
  } else {		
    // diagonal is negative
    i = 0;
    //if (m[1][1] > m[0][0]) i = 1;
    //if (m[2][2] > m[i][i]) i = 2;
    if (m[5] > m[0]) i = 1;
    if (m[10] > m[i + i*4]) i = 2;
    j = nxt[i];
    k = nxt[j];

    s = sqrt ((m[i + i*4] - (m[j + j*4] + m[k + k*4])) + 1.0);

    q[i] = s * 0.5;

    if (s != 0.0) s = 0.5 / s;

    /*q[3] = (m[j][k] - m[k][j]) * s;
    q[j] = (m[i][j] + m[j][i]) * s;
    q[k] = (m[i][k] + m[k][i]) * s;*/
    
    q[3] = (m[j + k*4] - m[k + j*4]) * s;
    q[j] = (m[i + j*4] + m[j + i*4]) * s;
    q[k] = (m[i + k*4] + m[k + i*4]) * s;

    this->x = q[0];
    this->y = q[1];
    this->z = q[2];
    this->w = q[3];
  }
}

/*void wbLib::Quaternion::ToMatrix4X4f(double * mat) {
  float xx      = this->x * this->x;
  float xy      = this->x * this->y;
  float xz      = this->x * this->z;
  float xw      = this->x * w;
  float yy      = this->y * this->y;
  float yz      = this->y * z;
  float yw      = this->y * w;
  float zz      = z * z;
  float zw      = z * w;
  mat[0]  = 1 - 2 * ( yy + zz );
  mat[1]  =     2 * ( xy - zw );
  mat[2]  =     2 * ( xz + yw );
  mat[4]  =     2 * ( xy + zw );
  mat[5]  = 1 - 2 * ( xx + zz );
  mat[6]  =     2 * ( yz - xw );
  mat[8]  =     2 * ( xz - yw );
  mat[9]  =     2 * ( yz + xw );
  mat[10] = 1 - 2 * ( xx + yy );
  mat[3]  = mat[7] = mat[11] = mat[12] = mat[13] = mat[14] = 0;
  mat[15] = 1;
}*/

void wbLib::Quaternion::ToMatrix4X4f(double * pMatrix) {
// Make sure the matrix has allocated memory to store the rotation data
	if(!pMatrix) return;

	// First row
	pMatrix[ 0] = 1.0f - 2.0f * ( y * y + z * z ); 
	pMatrix[ 1] = 2.0f * (x * y + z * w);
	pMatrix[ 2] = 2.0f * (x * z - y * w);
	pMatrix[ 3] = 0.0f;  

	// Second row
	pMatrix[ 4] = 2.0f * ( x * y - z * w );  
	pMatrix[ 5] = 1.0f - 2.0f * ( x * x + z * z ); 
	pMatrix[ 6] = 2.0f * (z * y + x * w );  
	pMatrix[ 7] = 0.0f;  

	// Third row
	pMatrix[ 8] = 2.0f * ( x * z + y * w );
	pMatrix[ 9] = 2.0f * ( y * z - x * w );
	pMatrix[10] = 1.0f - 2.0f * ( x * x + y * y );  
	pMatrix[11] = 0.0f;  

	// Fourth row
	pMatrix[12] = 0;  
	pMatrix[13] = 0;  
	pMatrix[14] = 0;  
	pMatrix[15] = 1.0f;
}

/*void wbLib::Quaternion::ToMatrix4X4(double * m) {
  

    float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;


    // calculate coefficients
    x2 = this->x + this->x; 
    y2 = this->y + this->y;
    z2 = this->z + this->z;
    
    xx = this->x * x2; 
    xy = this->x * y2; 
    xz = this->x * z2;
    yy = this->y * y2; 
    yz = this->y * z2; 
    zz = this->z * z2;
    wx = this->w * x2; 
    wy = this->w * y2; 
    wz = this->w * z2;


    //m[0][0] = 1.0 - (yy + zz); 
    //m[1][0] = xy - wz;
    //m[2][0] = xz + wy; 
    //m[3][0] = 0.0;

    //m[0][1] = xy + wz; 
    //m[1][1] = 1.0 - (xx + zz);
    //m[2][1] = yz - wx; 
    //m[3][1] = 0.0;

    //m[0][2] = xz - wy; 
    //m[1][2] = yz + wx;
    //m[2][2] = 1.0 - (xx + yy); 
    //m[3][2] = 0.0;

    //m[0][3] = 0; 
    //m[1][3] = 0;
    //m[2][3] = 0; 
    //m[3][3] = 1;
    
    m[0+0*4] = 1.0 - (yy + zz); 
    m[1+0*4] = xy - wz;
    m[2+0*4] = xz + wy; 
    m[3+0*4] = 0.0;

    m[0+1*4] = xy + wz; 
    m[1+1*4] = 1.0 - (xx + zz);
    m[2+1*4] = yz - wx; 
    m[3+1*4] = 0.0;

    m[0+2*4] = xz - wy; 
    m[1+2*4] = yz + wx;
    m[2+2*4] = 1.0 - (xx + yy); 
    m[3+2*4] = 0.0;

    m[0+3*4] = 0; 
    m[1+3*4] = 0;
    m[2+3*4] = 0; 
    m[3+3*4] = 1;
    
    
    
    
    
 // m[0] = 1.0;
	//m[4] = 0.0;
	//m[8] = 0.0;
	//m[12] = 0.0;
	//
	//m[1] = 0.0;
	//m[5] = 1.0;
	//m[9] = 0.0;
	//m[13] = 0.0;
	//
	//m[2] = 0.0;
	//m[6] = 0.0;
	//m[10] = 1.0;
	//m[14] = 0.0;
	//
	//m[3] = 0.0;
	//m[7] = 0.0;
	//m[11] = 0.0;
	//m[15] = 1.0;

}*/

/*
void Quaternion::FromEuler(float _angleX, float _angleY, float _angleZ) {
  
  double matrix[16];
  float alpha = _angleX * M_PI / 180.0f;
  float beta = (_angleY * M_PI / 180.0f) * -1.0;
  float gamma = _angleZ * M_PI / 180.0f;
  
  
 // matrix[0] = cos(beta);
	//matrix[4] = sin(beta) * sin(alpha);
	//matrix[8] = sin(beta) * cos(alpha);
	//matrix[12] = 0.0;
	//
	//matrix[1] = 0;
	//matrix[5] = cos(alpha);
	//matrix[9] = -sin(alpha);
	//matrix[13] = 0.0;
	//
	//matrix[2] = -sin(beta);
	//matrix[6] = sin(alpha)*cos(beta);
	//matrix[10] = cos(beta) * cos(alpha);
	//matrix[14] = 0.0;
	//
	//matrix[3] = 0.0;
	//matrix[7] = 0.0;
	//matrix[11] = 0.0;
	//matrix[15] = 1.0;
  
  matrix[0] = cos(gamma)*cos(beta);
  matrix[1] = sin(gamma)*cos(beta);
  matrix[2] = -sin(beta);
  matrix[3] = 0.0;
  
  matrix[4] = cos(gamma)*sin(beta)*sin(alpha) - sin(gamma)*cos(alpha);
  matrix[5] = sin(gamma)*sin(beta)*sin(alpha) + cos(gamma)*cos(alpha);
  matrix[6] = sin(alpha)*cos(beta);
  matrix[7] = 0.0;
  
  matrix[8] = cos(gamma)*sin(beta)*cos(alpha) + sin(gamma)*sin(alpha);
  matrix[9] = sin(gamma)*sin(beta)*cos(alpha) - cos(gamma)*sin(alpha);
  matrix[10] = cos(alpha)*cos(beta);
  matrix[11] = 0.0;
  
  matrix[12] = 0.0;
  matrix[13] = 0.0;
  matrix[14] = 0.0;
  matrix[15] = 1.0;
  
  this->FromMatrix4X4(matrix);
}*/

void wbLib::Quaternion::FromEuler(float roll, float pitch, float yaw)
{
	float cr, cp, cy, sr, sp, sy, cpcy, spsy;


// calculate trig identities
cr = cos(roll/2);

	cp = cos(pitch/2);
	cy = cos(yaw/2);


	sr = sin(roll/2);
	sp = sin(pitch/2);
	sy = sin(yaw/2);
	
	cpcy = cp * cy;
	spsy = sp * sy;


	this->w = cr * cpcy + sr * spsy;
	this->x = sr * cpcy - cr * spsy;
	this->y = cr * sp * cy + sr * cp * sy;
	this->z = cr * cp * sy - sr * sp * cy;
}

void wbLib::Quaternion::SLERP(Quaternion * from, Quaternion * to, float t) {
  float to1[4];
  double omega, cosom, sinom, scale0, scale1;


  // calc cosine
  cosom = from->x * to->x + from->y * to->y + from->z * to->z + from->w * to->w;


  // adjust signs (if necessary)
  if ( cosom < 0.0 )
  { 
    cosom = -cosom; 
    to1[0] = - to->x;
    to1[1] = - to->y;
    to1[2] = - to->z;
    to1[3] = - to->w;
  } 
  else 
  {
    to1[0] = to->x;
    to1[1] = to->y;
    to1[2] = to->z;
    to1[3] = to->w;
  }


  // calculate coefficients


  if ( (1.0 - cosom) > 0.05 ) {
    // standard case (slerp)
    omega = acos(cosom);
    sinom = sin(omega);
    scale0 = sin((1.0 - t) * omega) / sinom;
    scale1 = sin(t * omega) / sinom;


  } else {        
    // "from" and "to" quaternions are very close 
    //  ... so we can do a linear interpolation
    scale0 = 1.0 - t;
    scale1 = t;
  }
  // calculate final values
  this->x = scale0 * from->x + scale1 * to1[0];
  this->y = scale0 * from->y + scale1 * to1[1];
  this->z = scale0 * from->z + scale1 * to1[2];
  this->w = scale0 * from->w + scale1 * to1[3];
}

void wbLib::Quaternion::FromAxisAngle(float _x, float _y, float _z, float degrees)
{
	// First we want to convert the degrees to radians
	// since the angle is assumed to be in radians
	float angle = float((degrees / 180.0f) * M_PI);

	// Here we calculate the sin( theta / 2) once for optimization
	float result = (float)sin( angle / 2.0f );

	// Calcualte the w value by cos( theta / 2 )
	this->w = (float)cos( angle / 2.0f );

	// Calculate the x, y and z of the quaternion
	this->x = float(_x * result);
	this->y = float(_y * result);
	this->z = float(_z * result);
}

wbLib::Quaternion wbLib::Quaternion::operator *(Quaternion q)
{
	wbLib::Quaternion r;

	r.w = w*q.w - x*q.x - y*q.y - z*q.z;
	r.x = w*q.x + x*q.w + y*q.z - z*q.y;
	r.y = w*q.y + y*q.w + z*q.x - x*q.z;
	r.z = w*q.z + z*q.w + x*q.y - y*q.x;

	return(r);
}

