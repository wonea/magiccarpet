#include <exception>
#include "ROAM.h"

// statics
int         wbLib::Landscape::nextTriNode;
wbLib::TriTreeNode wbLib::Landscape::triPool[POOL_SIZE];

wbLib::Landscape::Landscape() {
  heightMap = 0;
  patches = 0;
  // DANGER: If you adjust these scaling factors for x and z dimensions,
  // you HAVE TO adjust the variance distance computation inside the
  // patch tessellation method! The variance computation currently assumes,
  // that the cameras x and z position is in the range of 0 to 1024 which
  // is the map dimension! If you scale the visual representation of the
  // map, you have to adjust the camera position accordingly otherwise the
  // patch the camera is positioned in, might not be the patch which currently
  // gets more detailed tesselation
  scaleX = 1.0f;
  scaleY = 0.1f;
  scaleZ = 1.0f;
}

/**
 * Loads the height map into a buffer and prepares patches which 
 * control square subareas of the height map. Performs initial
 * variance computation and tesselation. After this call, the
 * landscape is ready for rendering but patches are still not
 * linked together which is done by reset()
 *
 * Param _patchCount - amount of patches per side (Total amount of
 *   patches = _patchCount * _patchCount)
 */
void wbLib::Landscape::init(int _mapSize, char * _heightMap, 
  int _patchCount, wbLib::FreeFlightCamera * _camera) throw (std::bad_exception) {
	Patch * patch = 0;

  if (0 == _heightMap) {
    std::bad_exception e("_heightMap is a null reference");
    throw e;
  }

  mapSize = _mapSize;
  heightMap = _heightMap;
  patchCount = _patchCount;
  camera = _camera;

  patchSize = mapSize / patchCount;

  patches = new Patch[patchCount * patchCount];

  patch = patches;

	// initialize all terrain patches
  for (int i = 0; i < patchCount; i++) {
		for (int j = 0; j < patchCount; j++) {
			
      //patch->reset();
			patch->init(j * patchSize, i * patchSize, heightMap,
        mapSize, patchSize, camera);
			patch->computeVariance();
      patch->tessellate();
      patch++;
		}
  }

  /*n.init(this, patchCount, 0, -mapSize);
  no.init(this, patchCount, mapSize, -mapSize);
  o.init(this, patchCount, mapSize, 0);
  so.init(this, patchCount, mapSize, mapSize);
  s.init(this, patchCount, 0, mapSize);
  sw.init(this, patchCount, -mapSize, mapSize);
  w.init(this, patchCount, -mapSize, 0);
  nw.init(this, patchCount, -mapSize, -mapSize);*/
}

void wbLib::Landscape::cleanUp() {
  if (0 != heightMap) {
    //delete [] heightMap;
    delete heightMap;
    heightMap = 0;
  }
  if (0 != patches) {
    delete [] patches;
    patches = 0;
  }
  /*n.cleanUp();
  no.cleanUp();
  o.cleanUp();
  so.cleanUp();
  s.cleanUp();
  sw.cleanUp();
  w.cleanUp();
  nw.cleanUp();*/
}

/**
 * reset all patches, recompute variance if needed, link
 * patches inside landscape and link landscape to landscape.
 * The linking of patches is needed as patches base triangles
 * need to know their neighbours for correct triangle splitting
 */
void wbLib::Landscape::reset() {
	//
	// Perform simple visibility culling on entire patches.
	//   - Define a triangle set back from the camera by one patch size, following
	//     the angle of the frustum.
	//   - A patch is visible if it's center point is included in the angle: Left,Eye,Right
	//   - This visibility test is only accurate if the camera cannot look up or down significantly.
	//
	//const float PI_DIV_180 = M_PI / 180.0f;
	//const float FOV_DIV_2 = gFovX/2;

	/*int eyeX = (int)(gViewPosition[0] - PATCH_SIZE * sinf( gClipAngle * PI_DIV_180 ));
	int eyeY = (int)(gViewPosition[2] + PATCH_SIZE * cosf( gClipAngle * PI_DIV_180 ));

	int leftX  = (int)(eyeX + 100.0f * sinf( (gClipAngle-FOV_DIV_2) * PI_DIV_180 ));
	int leftY  = (int)(eyeY - 100.0f * cosf( (gClipAngle-FOV_DIV_2) * PI_DIV_180 ));

	int rightX = (int)(eyeX + 100.0f * sinf( (gClipAngle+FOV_DIV_2) * PI_DIV_180 ));
	int rightY = (int)(eyeY - 100.0f * cosf( (gClipAngle+FOV_DIV_2) * PI_DIV_180 ));*/

	int X, Y;
	wbLib::Patch * patch;

	// Set the index, which specifies the next element of the triPool
  // which is returned a pointer to, when a triangle is requested,
  // the next time
	setNextTriNode(0);

	// Reset rendered triangle count.
	//gNumTrisRendered = 0;

	// Go through the patches performing resets, compute variances, and linking.
  for (Y = 0; Y < patchCount; Y++) {
    for (X = 0; X < patchCount; X++) {

			patch = &(patches[Y * patchCount + X]);
			
			// Reset the patch
			patch->reset();
			//patch->SetVisibility( eyeX, eyeY, leftX, leftY, rightX, rightY );
			
			// check to see if this patch has been deformed since last frame.
			// If so, recompute the variance tree for it.
			//if (patch->isDirty())
			//	patch->computeVariance();

			//if (patch->isVisibile())
			//{
				// Link all the patches together.
				if (X > 0)
					patch->getBaseLeft()->LeftNeighbor = patches[Y * patchCount + (X-1)].getBaseRight();
				else
					patch->getBaseLeft()->LeftNeighbor = 0;		// Link to bordering Landscape here..

				if (X < (patchCount-1))
					patch->getBaseRight()->LeftNeighbor = patches[Y * patchCount + (X+1)].getBaseLeft();
				else
					patch->getBaseRight()->LeftNeighbor = 0;		// Link to bordering Landscape here..

				if (Y > 0)
					patch->getBaseLeft()->RightNeighbor = patches[(Y-1) * patchCount + X].getBaseRight();
				else
					patch->getBaseLeft()->RightNeighbor = 0;		// Link to bordering Landscape here..

				if (Y < (patchCount-1))
					patch->getBaseRight()->RightNeighbor = patches[(Y+1) * patchCount + X].getBaseLeft();
				else
					patch->getBaseRight()->RightNeighbor = 0;	// Link to bordering Landscape here..
			//}
		}
  }
}

/**
 * Create an approximate mesh of the landscape.
 */
void wbLib::Landscape::tessellate() {	
	int count;
	wbLib::Patch * patch = patches;

	for (count=0; count < (patchCount * patchCount); count++, patch++) {
		//if (patch->isVisibile())
			patch->tessellate();
	}
}

/** 
 * Scale terrain. Call render for each() patch. Adjust the frame variance.
 */
void wbLib::Landscape::render() {
	int nCount;
	wbLib::Patch * patch = &(patches[0]);

	// Scale the terrain by the terrain scale specified at compile time.
	glScalef(scaleX, scaleY, scaleZ);

	for (nCount = 0; nCount < (patchCount * patchCount); nCount++, patch++)	{
		//if (patch->isVisibile())
			patch->render();
	}
  /*n.render();
  no.render();
  o.render();
  so.render();
  s.render();
  sw.render();
  w.render();
  nw.render();*/

	// Check to see if we got close to the desired number of triangles.
	// Adjust the frame variance to a better value.
	//if ( GetNextTriNode() != gDesiredTris )
	//	gFrameVariance += ((float)GetNextTriNode() - (float)gDesiredTris) / (float)gDesiredTris;

	// Bounds checking.
	//if (gFrameVariance < 0)
	//	gFrameVariance = 0;
}

/**
 * Returns a pointer to an element of the triPool array. The element
 * is specified by the index nextTriNode, which is adjusted by reset()
 * setNextTriNode(), and this methode.
 */
wbLib::TriTreeNode * wbLib::Landscape::allocateTri() {
	wbLib::TriTreeNode * pTri;

	// IF we've run out of TriTreeNodes, just return NULL (this is handled gracefully)
  if (nextTriNode >= POOL_SIZE) {
		return 0;
  }

	pTri = &(triPool[nextTriNode++]);

  // Init structure
	pTri->LeftChild = pTri->RightChild = 0;

	return pTri;
}

int wbLib::Landscape::getNextTriNode() { 
  return nextTriNode; 
}

void wbLib::Landscape::setNextTriNode(int _nextNode) { 
  nextTriNode = _nextNode; 
}

//void wbLib::MirrorLandscape::init(wbLib::Landscape * _landscape, 
//                                        int _patchCount, int _offsetX, int _offsetY) 
//{
//  patchCount = _patchCount;
//  offsetX = _offsetX;
//  offsetY = _offsetY;
//
//  scaleX = _landscape->scaleX;
//  scaleY = _landscape->scaleY;
//  scaleZ = _landscape->scaleZ;
//
//  wbLib::MirrorPatch * mirrorPatch = 0;
//
//  patches = new wbLib::MirrorPatch[patchCount * patchCount];
//
//  mirrorPatch = patches;
//
//  for (int i = 0; i < (patchCount * patchCount); i++) {   
//
//    mirrorPatch->init(_landscape->patches[i].x,
//    _landscape->patches[i].y,
//    _landscape->heightMap,
//    //mirrorPatch->heightMapOffset = _landscape->patches[i].heightMapOffset;
//    _landscape->patches[i].mapSize,
//    _landscape->patches[i].patchSize);
//
//    mirrorPatch->ptrBaseLeft = &(_landscape->patches[i].baseLeft);
//    mirrorPatch->ptrBaseRight = &(_landscape->patches[i].baseRight);
//
//    mirrorPatch->x = _landscape->patches[i].x + _offsetX;
//    mirrorPatch->y = _landscape->patches[i].y + _offsetY;
//
//    mirrorPatch++;
//  }
//}
//
//void wbLib::MirrorLandscape::render() {
//  //glScalef(scaleX, scaleY, scaleZ);
//
//  wbLib::MirrorPatch * patch = patches;
//
//	for (int count = 0; count < (patchCount * patchCount); count++, patch++)	{
//			patch->render();
//	}
//}
//
//void wbLib::MirrorLandscape::cleanUp() {
//  if (0 != patches) {
//    delete [] patches;
//    patches = 0;
//  }
//}