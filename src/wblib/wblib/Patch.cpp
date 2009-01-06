#include "ROAM.h"

void wbLib::Patch::init(int _x,
  int _y,
  char * _heightMap,
  int _mapSize,
  int _patchSize,
  wbLib::FreeFlightCamera * _camera
  ) 
{
	// Clear all the relationships
	baseLeft.RightNeighbor = 
  baseLeft.LeftNeighbor = 
  baseRight.RightNeighbor = 
  baseRight.LeftNeighbor =
	baseLeft.LeftChild = 
  baseLeft.RightChild = 
  baseRight.LeftChild = 
  baseRight.RightChild = 0;

	// Attach the two m_Base triangles together
	baseLeft.BaseNeighbor = &baseRight;
	baseRight.BaseNeighbor = &baseLeft;

	// Store Patch offsets for the world and heightmap.
	x = _x;
	y = _y;
  camera = _camera;

	// Store pointer to first byte of the height data for this patch.
  heightMapOffset = &_heightMap[y * _mapSize + x];

  patchSize = _patchSize;
  mapSize = _mapSize;

	// Initialize flags
	//varianceDirty = 1;
	//isVisible = false;
}

wbLib::TriTreeNode * wbLib::Patch::getBaseLeft() { 
  return &baseLeft; 
}

wbLib::TriTreeNode * wbLib::Patch::getBaseRight() { 
  return &baseRight;
}

//char wbLib::Patch::isDirty() { 
//  return varianceDirty; 
//}

//int wbLib::Patch::isVisibile() { 
//  return isVisible; 
//}

//void wbLib::setVisibility(int _eyeX, int _eyeY, int _leftX, int _leftY, int _rightX, int _rightY) {
//	// Get patch's center point
//	int patchCenterX = m_WorldX + PATCH_SIZE / 2;
//	int patchCenterY = m_WorldY + PATCH_SIZE / 2;
//	
//	// Set visibility flag (orientation of both triangles must be counter clockwise)
//	m_isVisible = (orientation( eyeX,  eyeY,  rightX, rightY, patchCenterX, patchCenterY ) < 0) &&
//				  (orientation( leftX, leftY, eyeX,   eyeY,   patchCenterX, patchCenterY ) < 0);
//}



/**
 * Computes the variance of a patch by splitting the square patch
 * into two triangles and recursivly computing the variance on those
 * two triangles by splitting the triangles into triangles. 
 *
 * The variance tree is filled by this method.
 *
 * The square patch is split into two triangles like this
 *
 *         (0|0)              (patchSize|0)
 *            ****************
 *            *             /*
 *            *            / *
 *            *           /  *
 *            *    A     /   *
 *            *         /    *
 *            *        /     *
 *            *       /      * 
 *            *      /       *
 *            *     /        *
 *            *    /         *
 *            *   /     B    *
 *            *  /           *
 *            * /            *
 *            *|             *
 *            ****************
 *      (0|patchSize)     (patchSize|patchSize)
 */

void wbLib::Patch::computeVariance() {
	currentVariance = varianceLeft;

	recursComputeVariance(
    // left
    0,
    patchSize, 
    heightMapOffset[patchSize * mapSize],
    // right
		patchSize,
    0,
    heightMapOffset[patchSize],
    // apex
		0,
    0,
    heightMapOffset[0],
		1);

	currentVariance = varianceRight;

	recursComputeVariance(patchSize,
    0,
    heightMapOffset[patchSize],
		0,
    patchSize,
    heightMapOffset[patchSize * mapSize],
    patchSize,
    patchSize,
    heightMapOffset[(patchSize * mapSize) + patchSize],
   	1);

	// Clear the dirty flag for this patch
	varianceDirty = 0;
}


/**
 * Discover the orientation of a triangle's points:
 *
 * Taken from "Programming Principles in Computer Graphics", L. Ammeraal (Wiley)
 */
inline int orientation(int _pX, int _pY, int _qX, int _qY, int _rX, int _rY) {
	int aX, aY, bX, bY;
	float d;

	aX = _qX - _pX;
	aY = _qY - _pY;

	bX = _rX - _pX;
	bY = _rY - _pY;

	d = (float)aX * (float)bY - (float)aY * (float)bX;
	return (d < 0) ? (-1) : (d > 0);
}

/**
 * Clears visibility and resets relationships
 */
void wbLib::Patch::reset() {
	// Assume patch is not visible.
	//isVisible = 0;
  //isVisible = 1;

	// Reset the important relationships
	baseLeft.LeftChild = baseLeft.RightChild = baseRight.LeftChild = baseLeft.LeftChild = 0;

	// Attach the two m_Base triangles together
	baseLeft.BaseNeighbor = &baseRight;
	baseRight.BaseNeighbor = &baseLeft;

	// Clear the other relationships.
	baseLeft.RightNeighbor = baseLeft.LeftNeighbor = baseRight.RightNeighbor = baseRight.LeftNeighbor = 0;
}


/**
 * Split a single Triangle and link it into the mesh.
 * Will correctly force-split diamonds.
 *
 * The split operation refines the binary triangle tree and uses
 * TriTreeNode structures from the pool.
 */
void wbLib::Patch::split(TriTreeNode * _tri) {
	// We are already split, no need to do it again.
  // Triangles, that have not been split yet, are having no
  // children and thus are having a null pointer as leftChild or rightChild
  // Null used in if() resembles false, non null is true. So if there is a child
  // and therefore the tri has been split, the if() executes the return statement
	if (_tri->LeftChild)
		return;

	// If this triangle is not in a proper diamond, force split our base neighbor
  if (_tri->BaseNeighbor && (_tri->BaseNeighbor->BaseNeighbor != _tri)) {
		split(_tri->BaseNeighbor);
  }

	// Create children and link into mesh
	_tri->LeftChild  = Landscape::allocateTri();
	_tri->RightChild = Landscape::allocateTri();

	// If creation failed, just exit.
  if (!_tri->LeftChild) {
		return;
  }

	// Fill in the information we can get from the parent (neighbor pointers)
	_tri->LeftChild->BaseNeighbor  = _tri->LeftNeighbor;
	_tri->LeftChild->LeftNeighbor  = _tri->RightChild;

	_tri->RightChild->BaseNeighbor  = _tri->RightNeighbor;
	_tri->RightChild->RightNeighbor = _tri->LeftChild;

	// Link our Left Neighbor to the new children
	if (_tri->LeftNeighbor != 0)
	{
		if (_tri->LeftNeighbor->BaseNeighbor == _tri)
			_tri->LeftNeighbor->BaseNeighbor = _tri->LeftChild;
		else if (_tri->LeftNeighbor->LeftNeighbor == _tri)
			_tri->LeftNeighbor->LeftNeighbor = _tri->LeftChild;
		else if (_tri->LeftNeighbor->RightNeighbor == _tri)
			_tri->LeftNeighbor->RightNeighbor = _tri->LeftChild;
		else
			;// Illegal Left Neighbor!
	}

	// Link our Right Neighbor to the new children
	if (_tri->RightNeighbor != 0)	{
		if (_tri->RightNeighbor->BaseNeighbor == _tri)
			_tri->RightNeighbor->BaseNeighbor = _tri->RightChild;
		else if (_tri->RightNeighbor->RightNeighbor == _tri)
			_tri->RightNeighbor->RightNeighbor = _tri->RightChild;
		else if (_tri->RightNeighbor->LeftNeighbor == _tri)
			_tri->RightNeighbor->LeftNeighbor = _tri->RightChild;
		else
			;// Illegal Right Neighbor!
	}

	// Link our Base Neighbor to the new children
	if (_tri->BaseNeighbor != 0) {
		if (_tri->BaseNeighbor->LeftChild )	{
			_tri->BaseNeighbor->LeftChild->RightNeighbor = _tri->RightChild;
			_tri->BaseNeighbor->RightChild->LeftNeighbor = _tri->LeftChild;
			_tri->LeftChild->RightNeighbor = _tri->BaseNeighbor->RightChild;
			_tri->RightChild->LeftNeighbor = _tri->BaseNeighbor->LeftChild;
		}
		else
			split(_tri->BaseNeighbor);  // Base Neighbor (in a diamond with us) was not split yet, so do that now.
	}
	else
	{
		// An edge triangle, trivial case.
		_tri->LeftChild->RightNeighbor = 0;
		_tri->RightChild->LeftNeighbor = 0;
	}
}

void wbLib::Patch::tessellate() {
	currentVariance = varianceLeft;

	recursTessellate(&baseLeft,
		x,
    y + patchSize,
		x + patchSize,
    y,
		x,
    y,
		1);
					
	currentVariance = varianceRight;

	recursTessellate(&baseRight,
		x + patchSize,
    y,
		x,
    y + patchSize,
  	x + patchSize,
    y + patchSize,
		1);
}

/**
 * Tessellate a Patch.
 * Will continue to split until the variance metric is met.
 *
 * The tesselation splits triangles and so builds the binary triangle tree (btt).
 * By refining the btt, it achieves smaller variances, because the triangles
 * are a better approximation to the landscape.
 *
 * All variances up to a certain depth are precomputed inside the variance tree (vt),
 * which does not change as long as the terrain is not deformed.
 *
 * If by refining tesselation, the btt grows deeper than the vt, no variance information
 * is present any more and by design, the algorithm just starts to tesselate until the 
 * best approximation is reached, or until the pool of triangles is used up. If a triangle
 * is still inside the vt, the tessalation can be aborted, when the triangles variance
 * is already smaller than the gFrameVariance.
 *
 * If the viewer position is not taken into account, this static approach leads to a landscape,
 * that may be unnecessarily detailed in places, the viewer is not even near. (vieweing is another
 * story). By taking distance into account, one can give far off areas a very small variance, which means,
 * the tesselation will stop soon, as this variance is already reached inside the vt.
 *
 * Areas near the viewer get big variances, which means, their tesselation is either to stop very late in the
 * vt, or not at all and it proceeds up to the highest level of detail. In effect this removes remote areas
 * which are drawn in high detail. If the parameters are choosen wisely, the only detailed spots
 * are located near the viewer.
 *
 * If you put #define VARIANCE_DEPTH (9) lower, say #define VARIANCE_DEPTH (5), vt gets less deep and the
 * recursion goes to highest level of detail sooner.
 *
 * If you put float gFrameVariance = 50; lower, the algorithm gives bad approximations no chance and creates
 * finer tesselations.
 */
void wbLib::Patch::recursTessellate(TriTreeNode * _tri,
							 int _leftX,  int _leftY,
							 int _rightX, int _rightY,
							 int _apexX,  int _apexY,
							 int _node)
{
	float triVariance;

	int centerX = (_leftX + _rightX) >> 1; // Compute X coordinate of center of Hypotenuse
	int centerY = (_leftY + _rightY) >> 1; // Compute Y coord...

  // If this recursion is still in a depth, where variance is precomputed inside
  // the variance tree, use this information to compute TriVariance. TriVariance
  // is able to stop the recursion sooner.
  //
  // If the recursion proceeded below the variance tree, recursion cannot be
  // stopped and progresses until the highest level of detailed has been 
  // tesselated out. The highest possible level of detail is the depth map information itself.
	if (_node < (1<<VARIANCE_DEPTH)) {

    // This distance computation assumes that the visual representation is
    // not scaled different than by the factor 1.0f in x and z direction!
		// extremely slow distance metric (sqrt is used).
		// replace this with a faster one
		float distance = 1.0f + sqrtf(SQR((float)centerX - camera->pos.x) +
			SQR((float)centerY - camera->pos.z));    

    // DANGER: If you have scaled the visual representation of the map,
    // you also have to scale the cameras position to match the visual
    // representation!
    //float distance = 1.0f + sqrtf(SQR((float)centerX - (camera->pos.x/scaleX)) +
		//	SQR((float)centerY - (camera->pos.z/scaleZ)));    
		
		// Egads!  A division too?  What's this world coming to!
		// This should also be replaced with a faster operation.
    // Take both distance and variance into consideration
    //
    // The farther away this triangle is, the smaller the computed variance
    // becomes. A small variance means a small need to further subdivide the 
    // triangle. So farther triangles stay more coarse
		triVariance = ((float)currentVariance[_node] * mapSize * 2)/distance;	
    //TriVariance = (float)m_CurrentVariance[node] * MAP_SIZE * 2;
    //triVariance = (float)currentVariance[node];
  }
  /*else // This else keeps the recursion inside the variance tree
  {
    return;
  }*/

  // IF we do not have variance info for this node, 
  // then we must have gotten here by splitting, 
  // so continue down to the lowest level.
  // 
  // Due to some stupid parameter setup it might happen, that the tesselation
  // creates a more depth binary triangle tree, than the variance tree provides 
  // information for. 
  //
  // If this happened, the recursion proceeds until the highest level of detail
  // has been reached. So in order to not reference array elements of the variance
  // tree, that do not exist, this check is made: (node >= (1<<VARIANCE_DEPTH)) before
  // the TriVariance Variable is used. TriVariance is only initialized if the node is 
  // in the variance tree.
  //
  // If the recursion acts on a node, which is not contained in the variance tree
  // the recursion prooceeds until the highest level of detail is reached.
  // 
  // If the recusion acts on a node in the variance tree, the recursion might be stopped
  // sooner, if the needed variance is already achieved.
	if ((_node >= (1<<VARIANCE_DEPTH)) ||	
		 (triVariance > FRAME_VARIANCE))	// OR if we are not below the variance tree, test for variance.
	{
		split(_tri); // Split this triangle.
		
    // If this triangle was split, try to split it's children as well.
		if (_tri->LeftChild &&	
      // Tessellate all the way down to one vertex per height field entry
      // >= 3 means if the is a triangle which has edge points that are 
      // three units apart at minimum, this triangle is split and thereby a 
      // new point is created in the middle. Smaller triangles are not split
      // any more
      ((::abs(_leftX - _rightX) >= 3) || (abs(_leftY - _rightY) >= 3)))	
		{
			recursTessellate(_tri->LeftChild,   _apexX,  _apexY, _leftX, _leftY, centerX, centerY,    _node<<1);
			recursTessellate(_tri->RightChild, _rightX, _rightY, _apexX, _apexY, centerX, centerY, 1+(_node<<1));
		}
	}
}

/**
 * Is called by Landscape::render(). Every square patch consists of its two
 * base triangles. The triangles are recursively refined to approximate the
 * landscape. The child triangles of each base triangle are stored inside
 * the two binary triangle tree (btt) of the patch.
 *
 * Every triangle is again made up of two
 * new triangles. The recursive rendering renders the btt by traversing down to
 * the leaf triangles and performing GL_TRIANGLE calls on their coordinates.
 */
void wbLib::Patch::render() {
	glPushMatrix();
	
	// translate the patch to the proper world coordinates
	glTranslatef((float)x, 0.0f, (float)y);

	glBegin(GL_TRIANGLES);
		
  // left base triangle of patch
	recursRender(&baseLeft,
    // left x,y
		0,
    patchSize,
    // right x,y
		patchSize,
    0,
    // apex x,y
		0,
    0);
	
  // right base triangle of patch
	recursRender(&baseRight,
    // left x,y
		patchSize,
    0,
    // right x,y
		0,
    patchSize,
    // apex x,y
		patchSize,
    patchSize);
	
	glEnd();
	
	glPopMatrix();
}

void wbLib::Patch::renderTexture() {
	glPushMatrix();
	
	// translate the patch to the proper world coordinates
	glTranslatef((float)x, 0.0f, (float)y);

	glBegin(GL_TRIANGLES);
		
  // left base triangle of patch
	recursRenderTexture(&baseLeft,
    // left x,y
		0,
    patchSize,
    // left texcoord s,t
    0.0f,
    0.0f,
    // right x,y
		patchSize,
    0,
    // right textcoord s,t
    1.0f,
    1.0f,
    // apex x,y
		0,
    0,
    // apex textcoord s,t
    0.0f,
    1.0f);
	
  // right base triangle of patch
	recursRenderTexture(&baseRight,
    // left x,y
		patchSize,
    0,
    // left texcoord s,t
    1.0f,
    1.0f,
    // right x,y
		0,
    patchSize,
    // right texcoord s,t
    0.0f,
    0.0f,
    // apex x,y
		patchSize,
    patchSize,
    // apex texcoord s,t
    1.0f,
    0.0f);
	
	glEnd();
	
	glPopMatrix();
}

void wbLib::Patch::recursRender(TriTreeNode * tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY) {
  // All non-leaf nodes have both children, so just check for one
	if (tri->LeftChild)	{
		int centerX = (leftX + rightX)>>1;	// Compute X coordinate of center of Hypotenuse
		int centerY = (leftY + rightY)>>1;	// Compute Y coord...

		recursRender(tri->LeftChild,  apexX,   apexY, leftX, leftY, centerX, centerY);
		recursRender(tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY);
	}	else {
    // A leaf node!  Output a triangle to be rendered.
		// Actual number of rendered triangles...
		//gNumTrisRendered++;

		float leftZ  = heightMapOffset[(leftY  * mapSize) + leftX ];
		float rightZ = heightMapOffset[(rightY * mapSize) + rightX];
		float apexZ  = heightMapOffset[(apexY  * mapSize) + apexX ];

		//// Perform lighting calculations if requested.
		//if (gDrawMode == DRAW_USE_LIGHTING)
		//{
		//	float v[3][3];
		//	float out[3];
		//	
		//	// Create a vertex normal for this triangle.
		//	// NOTE: This is an extremely slow operation for illustration purposes only.
		//	//       You should use a texture map with the lighting pre-applied to the texture.
		//	v[0][0] = (float) leftX;
		//	v[0][1] = (float) leftZ;
		//	v[0][2] = (float) leftY;
		//	
		//	v[1][0] = (float) rightX;
		//	v[1][1] = (float) rightZ ;
		//	v[1][2] = (float) rightY;
		//	
		//	v[2][0] = (float) apexX;
		//	v[2][1] = (float) apexZ ;
		//	v[2][2] = (float) apexY;
		//	
		//	calcNormal( v, out );
		//	glNormal3fv( out );
		//}

		//// Perform polygon coloring based on a height sample
		//float fColor = (60.0f + leftZ) / 256.0f;
  //  if (fColor > 1.0f) {
  //    fColor = 1.0f;
  //  }
		//glColor3f(fColor, fColor, fColor);

		// Output the LEFT VERTEX for the triangle
		glVertex3f((GLfloat) leftX,
			(GLfloat) leftZ,
			(GLfloat) leftY);

		//if ( gDrawMode == DRAW_USE_TEXTURE ||	// Gaurad shading based on height samples instead of light normal
		//	 gDrawMode == DRAW_USE_FILL_ONLY )
		//{
		//	float fColor = (60.0f + rightZ) / 256.0f;
		//	if ( fColor > 1.0f )  fColor = 1.0f;
		//	glColor3f( fColor, fColor, fColor );
		//}

		// Output the RIGHT VERTEX for the triangle
		glVertex3f((GLfloat) rightX,
			(GLfloat) rightZ,
			(GLfloat) rightY);


		//if ( gDrawMode == DRAW_USE_TEXTURE ||	// Gaurad shading based on height samples instead of light normal
		//	 gDrawMode == DRAW_USE_FILL_ONLY )
		//{
		//	float fColor = (60.0f + apexZ) / 256.0f;
		//	if ( fColor > 1.0f )  fColor = 1.0f;
		//	glColor3f( fColor, fColor, fColor );
		//}

		// Output the APEX VERTEX for the triangle
		glVertex3f((GLfloat) apexX,
			(GLfloat) apexZ,
			(GLfloat) apexY);
	}
}

void wbLib::Patch::recursRenderTexture(wbLib::TriTreeNode * _tri,
      int _leftX, 
      int _leftY,
      float _leftS,
      float _leftT,
      int _rightX, 
      int _rightY, 
      float _rightS,
      float _rightT,
      int _apexX, 
      int _apexY,
      float _apexS,
      float _apexT) 
{
  // All non-leaf nodes have both children, so just check for one
	if (_tri->LeftChild)	{
		int centerX = (_leftX + _rightX)>>1;	// Compute X coordinate of center of Hypotenuse
		int centerY = (_leftY + _rightY)>>1;	// Compute Y coord...

    float centerS = _leftS + ((_rightS - _leftS) / 2.0f);
    float centerT = _leftT + ((_rightT - _leftT) / 2.0f);

		recursRenderTexture(_tri->LeftChild, _apexX, _apexY, _apexS, _apexT, _leftX, _leftY, _leftS, _leftT, centerX, centerY, centerS, centerT);
		recursRenderTexture(_tri->RightChild, _rightX, _rightY, _rightS, _rightT, _apexX, _apexY, _apexS, _apexT, centerX, centerY, centerS, centerT);
	}	else {
    // A leaf node!  Output a triangle to be rendered.
		// Actual number of rendered triangles...
		//gNumTrisRendered++;

		float leftZ  = heightMapOffset[(_leftY  * mapSize) + _leftX ];
		float rightZ = heightMapOffset[(_rightY * mapSize) + _rightX];
		float apexZ  = heightMapOffset[(_apexY  * mapSize) + _apexX ];

		// Output the LEFT VERTEX for the triangle
    glTexCoord2f(_leftS, _leftT);
		glVertex3f((GLfloat) _leftX,
			(GLfloat) leftZ,
			(GLfloat) _leftY);

		// Output the RIGHT VERTEX for the triangle
    glTexCoord2f(_rightS, _rightT);
		glVertex3f((GLfloat) _rightX,
			(GLfloat) rightZ,
			(GLfloat) _rightY);

		// Output the APEX VERTEX for the triangle
    glTexCoord2f(_apexS, _apexT);
		glVertex3f((GLfloat) _apexX,
			(GLfloat) apexZ,
			(GLfloat) _apexY);
	}
}

// ---------------------------------------------------------------------
// Computes Variance over the entire tree. Does not examine node relationships.
//
// This means, even if the tesselation does not create triangles up to the height 
// of the information the variance tree provides, the values in the variance tree
// are computed anyways.
//
// If the terrain does not change shape, the variance tree can be reused.
//
// The tesselation builds the binary triangle tree and reads variances from the 
// variance tree. The binary triangle tree is affected by level of detail considerations.
// Near the camera high level of detail is used and the binary triangle tree is build
// very deep, which provides for high detail. So near the camera
//
unsigned char wbLib::Patch::recursComputeVariance( int leftX,  int leftY,  unsigned char leftZ,
										    int rightX, int rightY, unsigned char rightZ,
											int apexX,  int apexY,  unsigned char apexZ,
											int node)
{
	//        /|\
	//      /  |  \
	//    /    |    \
	//  /      |      \
	//  ~~~~~~~*~~~~~~~  <-- Compute the X and Y coordinates of '*'
	//
	int centerX = (leftX + rightX) >>1;		// Compute X coordinate of center of Hypotenuse
	int centerY = (leftY + rightY) >>1;		// Compute Y coord...
	unsigned char myVariance;

	// Get the height value at the middle of the Hypotenuse
	unsigned char centerZ  = heightMapOffset[(centerY * mapSize) + centerX];

	// Variance of this triangle is the actual height at it's hypotenuse midpoint minus the interpolated height.
	// Use values passed on the stack instead of re-accessing the Height Field.
	myVariance = abs((int)centerZ - (((int)leftZ + (int)rightZ)>>1));

	// Since we're after speed and not perfect representations,
	//    only calculate variance down to an 8x8 block
	if ( (abs(leftX - rightX) >= 8) ||
		 (abs(leftY - rightY) >= 8) )
	{
		// Final Variance for this node is the max of it's own variance and that of it's children.
		myVariance = MAX( myVariance, recursComputeVariance( apexX,   apexY,  apexZ, leftX, leftY, leftZ, centerX, centerY, centerZ,    node<<1 ) );
		myVariance = MAX( myVariance, recursComputeVariance( rightX, rightY, rightZ, apexX, apexY, apexZ, centerX, centerY, centerZ, 1+(node<<1)) );
	}

	// Store the final variance for this node.  Note Variance is never zero.
	if (node < (1<<VARIANCE_DEPTH))
		currentVariance[node] = 1 + myVariance;

	return myVariance;
}

/**
 * Draws the triangles inside its original patches binary triangle tree.
 * Therefore pointers to the original bttS are used.
 */
//void wbLib::MirrorPatch::render() {
//	glPushMatrix();
//	
//	// translate the patch to the proper world coordinates
//	glTranslatef((float)x, 0, (float)y);
//
//	glBegin(GL_TRIANGLES);
//		
//  // left base triangle of patch
//	recursRender(ptrBaseLeft,
//    // left x,y
//		0,
//    patchSize,
//    // right x,y
//		patchSize,
//    0,
//    // apex x,y
//		0,
//    0);
//	
//  // right base triangle of patch
//	recursRender(ptrBaseRight,
//    // left x,y
//		patchSize,
//    0,
//    // right x,y
//		0,
//    patchSize,
//    // apex x,y
//		patchSize,
//    patchSize);
//	
//	glEnd();
//	
//	glPopMatrix();
//}