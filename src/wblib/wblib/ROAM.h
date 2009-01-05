#ifndef _ROAM
#define _ROAM

#include <math.h>
#if _WIN32
  #include <windows.h>
#endif
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "FreeFlightCamera.h"


// scale of the terrain ie: 1 unit of the height map == how many world units (meters)?
// 1.0f == 1 meter resolution
// 0.5f == 1/2 meter resolution
// 0.25f == 1/4 meter resolution
// etc..
#define MULT_SCALE (0.5f)

// how many TriTreeNodes should be allocated?
#define POOL_SIZE (25000)

// Some more definitions
#define PATCH_SIZE (MAP_SIZE/NUM_PATCHES_PER_SIDE)
#define TEXTURE_SIZE (128)

// Drawing Modes
#define DRAW_USE_TEXTURE   (0)
#define DRAW_USE_LIGHTING  (1)
#define DRAW_USE_FILL_ONLY (2)
#define DRAW_USE_WIREFRAME (3)

// Rotation Indexes
#define ROTATE_PITCH (0)
#define ROTATE_YAW   (1)
#define ROTATE_ROLL	 (2)

#define SQR(x) ((x) * (x))
//#define MAX(a,b) ((a < b) ? (b) : (a))
//#define DEG2RAD(a) (((a) * M_PI) / 180.0f)
//#define M_PI (3.14159265358979323846f)

// External variables and functions:
//extern GLuint gTextureID;
//extern int gDrawMode;
//extern GLfloat gViewPosition[];
//extern GLfloat gCameraRotation[];
//extern GLfloat gClipAngle;
//extern float gFrameVariance;
//extern int gDesiredTris;
//extern int gNumTrisRendered;
//extern float gFovX;

//extern void calcNormal(float v[3][3], float out[3]);
//extern void ReduceToUnit(float vector[3]);


// Depth of variance tree: should be near SQRT(PATCH_SIZE) + 1
#define VARIANCE_DEPTH (9)
#define FRAME_VARIANCE 100
#define MAX(a,b) ((a < b) ? (b) : (a))

namespace wbLib {

  class Landscape;

  struct TriTreeNode {
	  TriTreeNode *LeftChild;
	  TriTreeNode *RightChild;
	  TriTreeNode *BaseNeighbor;
	  TriTreeNode *LeftNeighbor;
	  TriTreeNode *RightNeighbor;
  };

  class Patch {

  public:
	  char * heightMapOffset;
	  int x;
    int y;
    int patchSize;
    int mapSize;

    wbLib::FreeFlightCamera * camera;

    // Variance trees. The amount of elements is a power of two.
    // The exponent is VARIANCE_DEPTH. (2^VARIANCE_DEPTH)
	  unsigned char varianceLeft[ 1<<(VARIANCE_DEPTH)];
	  unsigned char varianceRight[1<<(VARIANCE_DEPTH)];

	  unsigned char * currentVariance;							// Which varience we are currently using. [Only valid during the Tessellate and ComputeVariance passes]
	  unsigned char varianceDirty;								// Does the Varience Tree need to be recalculated for this Patch?
	  bool isVisible;									// Is this patch visible in the current frame?

	  TriTreeNode baseLeft;										// Left base triangle tree node
	  TriTreeNode baseRight;									// Right base triangle tree node

	  TriTreeNode * getBaseLeft();
	  TriTreeNode * getBaseRight();
	  //char isDirty();
	  //bool isVisibile();
	  //void setVisibility(int _eyeX, int _eyeY, int _leftX, int _leftY, int _rightX, int _rightY);
    inline int orientation(int _pX, int _pY, int _qX, int _qY, int _rX, int _rY);

	  // The static half of the Patch Class
	  virtual void init(int _x, int _y, char * _heightMap, int _mapSize, int _patchSize,
      wbLib::FreeFlightCamera * _camera);
	  virtual void reset();
	  virtual void tessellate();
	  virtual void render();
	  virtual void computeVariance();

	  // The recursive half of the Patch Class
	  virtual void split(TriTreeNode * _tri);

	  virtual void recursTessellate(TriTreeNode * _tri,
      int leftX,
      int leftY,
      int rightX,
      int rightY,
      int apexX,
      int apexY,
      int node);

	  virtual void recursRender(TriTreeNode * _tri,
      int _leftX, 
      int _leftY, 
      int _rightX, 
      int _rightY, 
      int _apexX, 
      int _apexY);

	  virtual unsigned char	recursComputeVariance(int _leftX,
      int _leftY,
      unsigned char _leftZ,
      int _rightX, 
      int _rightY, 
      unsigned char rightZ,
		  int _apexX, 
      int _apexY,
      unsigned char _apexZ,
		  int _node);
  };

  /*class MirrorPatch : public wbLib::Patch {
  public:
    void render();
    wbLib::TriTreeNode * ptrBaseLeft;
	  wbLib::TriTreeNode * ptrBaseRight;
  };

  class MirrorLandscape {
  public:
    void init(Landscape * _landscape, int _patchCount, int _offsetX, int _offsetY);
    
    void render();
    void cleanUp();

    int patchCount;
    int offsetX;
    int offsetY;

    MirrorPatch * patches;
    float scaleX;
    float scaleY;
    float scaleZ;
  };*/

  /**
   * The Landscape class loads the terrain and splits it in peaces
   * which are assigned to square non-overlapping patches which
   * are controlled by the landscape class
   */
  class Landscape {

  public:
    Landscape();

    float scaleX;
    float scaleY;
    float scaleZ;
  	
    int mapSize;
    int patchCount;
    char * heightMap;  
    Patch * patches;
    int patchSize;

    wbLib::FreeFlightCamera * camera;

	  static int nextTriNode;	// Index to next free TriTreeNode
	  static TriTreeNode triPool[POOL_SIZE]; // Pool of TriTree nodes for splitting
	  static int getNextTriNode();
	  static void setNextTriNode(int _nextNode);

	  static TriTreeNode * allocateTri();

	  virtual void init(int _mapSize, char * _heightMap, int _patchCount,
      wbLib::FreeFlightCamera * _camera);
	  virtual void reset();
	  virtual void tessellate();
	  virtual void render();

    void cleanUp();

    /*wbLib::MirrorLandscape n;
    wbLib::MirrorLandscape no;
    wbLib::MirrorLandscape o;
    wbLib::MirrorLandscape so;
    wbLib::MirrorLandscape s;
    wbLib::MirrorLandscape sw;
    wbLib::MirrorLandscape w;
    wbLib::MirrorLandscape nw;*/
  };

  

  
}

#endif