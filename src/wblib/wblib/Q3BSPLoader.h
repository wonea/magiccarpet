/* 
 * http://www.thatsorange.com/portfolio/projects/
 * http://w3studi.informatik.uni-stuttgart.de/~bischowg/OpenGL/OGLq3bsp.html
 * http://www.devmaster.net/articles/quake3collision/
 * http://graphics.stanford.edu/~kekoa/q3/
 * http://graphics.cs.brown.edu/games/quake/quake3.html
 * http://www.flipcode.com/archives/Simple_Quake3_BSP_Loader.shtml
 *
 * Features implemented:
 * - patch faces
 * - lightmaps
 * - texturing
 * - PVS data
 * - already visible list
 * - handles maps having no pvs information and no lightmaps
 *
 * Features outstanding:
 * - texture coords for patch faces
 * - light volumes
 * - shaders
 * - map objects
 * - frustum culling
 * - collision detection
 *
 * LightMap parsing
 * - you need the multitexturing extension. Insert a stack at startup.
 * - compute the amount of lightmap textures stored in the bsp by
 *   sizeof lightmap lump divided by size of one TLightMap struct.
 * - Provide some kind of array or std::vector to hold the OpenGL
 *   texture object IDs for each created lightmap texture.
 * - Parse the lightmap lump and create a 128x128 texture for each read
 *   TLightMap struct. The lightmap textures are always 128x128 RGB 
 *   (3 Byte per pixel). The 128x128x3 bytes to generate a texture from are
 *   stored in a TLightMap struct (it holds nothing else besides that).
 *   Store the texture object ids in your vector or array in the same 
 *   sequence, you read them from the bsp. This sequence number is used
 *   as reference point later.
 * - Before creating the texture from the bytes read from the lightmap lump,
 *   you can adjust the brightness (gamma) of the image data to lighten
 *   dark levels up
 *  
 * LightMap rendering
 * - Apply lightmaps to rendered faces. Each face carries a lightmap ID. This
 *   ID indexes into the vector or array you stored your lightmaps in. You 
 *   should bind the faces texture on texture unit 0 and the faces lightmap
 *   texture onto texture unit 1
 * 
 * Usage:
 * - Create a variable and specify the bsp file's name and path to the 
 *   constructor
 * - Render the map using one of the DrawXXX() methods
 *   When using Draw(app->camera.pos.x, app->camera.pos.y, app->camera.pos.z);
 *   which needs the camera's position for visibility estimations, you can
 *   insert the cameras OpenGL coordinates (unswizzled) without a division
 *   by 64.0f. The loader takes care of that for you!
 * - CleanUp the map using the CleanUp() method
 *
 * Collision detection:
 * Q3BSP is based on a BSP tree. A bsp tree is made up of nodes. There is 
 * exactly one root node. Each node is associated with a plane which
 * cuts right through it and a childnode in front of that plane and a child node
 * behind that plane. The child nodes are given by positive indices into the
 * node lump. If a node with negative indices for its children is encountered,
 * these indices do not point into the nodes lump, but can be transformed into
 * positive indices into the leaves lump. Leaves are not nodes!
 *
 * Every leave has brushes associated to it. A brush has sides associated with.
 *
 *
 * Statically:
 * 1. Load file into buffer
 * 2. Check magic number and version
 * 3. 
 *
 * For each frame:
 */
#ifndef Q3BSP_H
#define Q3BSP_H

#include <GL/glew.h>
#include <GL/glut.h>

#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <vector>

#include "JPEGFile.h"
#include "TGAFile.h"

#define CONTENTS_SOLID 1
#define Q3EPSILON 0.03125f
#define SCALE_FACTOR 64.0f
//#define SCALE_FACTOR 1.0f

#define DIRENTRYCOUNT 17

// Game-related object descriptions.
#define ENTITIES 0 
// Surface descriptions (assume these have been converted to OpenGL textures).
#define TEXTURES 1 	
// Planes used by map geometry.
#define PLANES_LUMP 2
// BSP tree nodes.
#define NODES 3	
// 	BSP tree leaves.
#define LEAVES 4
// Lists of face indices, one list per leaf.
#define LEAFFACES 5	
// Lists of brush indices, one list per leaf.
#define LEAFBRUSHES 6
// Descriptions of rigid world geometry in map (we only use model[0]).
#define MODELS 7	
// Convex polyhedra used to describe solid space.
#define BRUSHES 8 
//	Brush surfaces.
#define BRUSHSIDES 9 
// Vertices used to describe faces.
#define VERTEXES 10	
// Lists of offsets, one list per mesh.
#define MESHVERTS 11
//	List of special map effects.
#define EFFECTS 12 
// Surface geometry. Clusters are made up of faces. Faces aren't single
// triangles, but they are a set of triangles. Faces store an index into the
// vertex lump (= 10) and therefore store the vertices they consist of.
// These vertices have to be drawn as triangle fans.
#define FACES 13 
// Packed lightmap data (assume these have been converted to an OpenGL texture)
#define LIGHTMAPS 14
// Local illumination data.
#define LIGHTVOLS 15 
// Cluster-cluster visibility data.
#define VISDATA 16

// Faces from lump 13 each have a type
// Simple triangle data
#define FACE_TYPE_POLYGON 1
// b spline, needs tesselation before rendering
#define FACE_TYPE_PATCH 2
// Simple triangle data
#define FACE_TYPE_MESH 3
// a surface that always has to face the viewer
#define FACE_TYPE_BILLBOARD 4

#ifdef _WIN32
#pragma(1)
#endif

/** 
 * The header stores an array of these structures, which contains
 * offsets to the real lump data. This structure should better
 * be called TLumpDescriptor, as it is not the real lump
 */
struct TLump {
	int	mOffset; // Offset to start of lump, relative to beginning of file.  
	int	mLength; // Length of lump. Always a multiple of 4.  
};

/**
 * Identifies file format and version number, stores lumps i.e. indices to
 * the file content
 */
struct THeader {
	char mMagicNumber[4]; // Magic number. Always "IBSP".  
	int mVersion; // Version number 0x2e for the BSP files distributed with Quake 3.  
	TLump mLumps[DIRENTRYCOUNT]; // Lump directory, seventeen entries.
};

/**
 * Face of the Q3 map.
 * The faces lump stores information used to render the surfaces of the map.
 */
struct TFace {
    int mTexture; // Texture index.
    int mEffect; // Index into lump 12 (Effects), or -1.
    int mType; // Face type. 1=polygon, 2=patch, 3=mesh, 4=billboard
    int mVertex; // Index of first vertex.
    int m_n_vertexes; // Number of vertices.
    int mMeshvert; // Index of first meshvert.
    int m_n_meshverts; // Number of meshverts.
    int lm_index; // Lightmap index.
    int lm_start[2]; // Corner of this face's lightmap image in lightmap.
    int lm_size[2]; // Size of this face's lightmap image in lightmap.
    float lm_origin[3]; // World space origin of lightmap.
    float lm_vecs[2][3]; // World space lightmap s and t unit vectors.
    float mNormal[3]; // Surface normal.
    int mSize[2]; // Patch dimensions.
};

/**
 * Vertex of the Q3 map.
 * The vertexes lump stores lists of vertices used to describe faces.
 */
//struct TVertex {
//    float mPosition[3]; // Vertex position.
//    float mTexcoord[2][2]; // Vertex texture coordinates. 0=surface, 1=lightmap.
//    float mNormal[3]; // Vertex normal.
//    unsigned char mColor[4]; // Vertex color. RGBA.
//};
struct TVertex {
    float mPosition[3]; // Vertex position.
    float mTexcoord[2];
    float mLightMap[2]; 
    float mNormal[3]; // Vertex normal.
    unsigned char mColor[4]; // Vertex color. RGBA.
};

/**
 * Entity of the Q3 map.
 * The entities lump stores game-related map information, 
 * including information about the map name, weapons, health, armor, 
 * triggers, spawn points, lights, and .md3 models to be placed in the map. 
 */
struct TEntity
{
	int					mSize;				// Size of the description.
	char*				mBuffer;			// Entity descriptions, stored as a string. 
};

/**
 * Texture of the Q3 map.
 * The textures lump stores information about surfaces and volumes, which are in turn associated with faces, brushes, and brushsides. 
 */
struct TTexture
{
	char				mName[64];			// Texture name.  
	int					mFlags;				// Surface flags.  
	int					mContents;			// Content flags.  
};

/**
 * Plane of the Q3 map.
 * The planes lump stores a generic set of planes that are in turn referenced by nodes and brushsides. 
 */
struct TPlane
{
	float				mNormal[3];			// Plane normal.  
	float				mDistance;			// Distance from origin to plane along normal.  
};

/**
 * Node of the Q3 map bsp tree.
 * The nodes lump stores all of the nodes in the map's BSP tree.
 */
struct TNode
{
	int					mPlane;				// Plane index.  
	int					mChildren[2];	// Children indices. Negative numbers are leaf indices: -(leaf+1).  
	int					mMins[3];			// Integer bounding box min coord.  
	int					mMaxs[3];			// Integer bounding box max coord.  
};

/**
 * Leaf of the Q3 map.
 * The leafs lump stores the leaves of the map's BSP tree. 
 */
struct TLeaf
{
	int					mCluster;			// Visdata cluster index.  
	int					mArea;				// Areaportal area.  
	int					mMins[3];			// Integer bounding box min coord.  
	int					mMaxs[3];			// Integer bounding box max coord.  
	int					mLeafFace;			// First leafface for leaf.  
	int					mNbLeafFaces;		// Number of leaffaces for leaf.  
	int					mLeafBrush;			// First leafbrush for leaf.  
	int					mNbLeafBrushes;		// Number of leafbrushes for leaf.
};

/**
 * LeafFace of the Q3 map.
 * The leaffaces lump stores lists of face indices, with one list per leaf. 
 */
struct TLeafFace
{
	int					mFaceIndex;			// Face index.  
};

/**
 * Leaf Brush of the Q3 map.
 * The leafbrushes lump stores lists of brush indices, with one list per leaf. 
 */
struct TLeafBrush
{
	int					mBrushIndex;		// Brush index.  
};

/**
 * Model of the Q3 map.
 * The models lump describes rigid groups of world geometry. 
 */
struct TModel
{
	float				mMins[3];			// Bounding box min coord.  
	float				mMaxs[3];			// Bounding box max coord.  
	int					mFace;				// First face for model.  
	int					mNbFaces;			// Number of faces for model.  
	int					mBrush;				// First brush for model.  
	int					mNBrushes;		// Number of brushes for model.  

};

/**
 * Brush of the Q3 map.
 * The brushes lump stores a set of brushes, which are in turn used for collision detection.
 */
struct TBrush
{
	int					mBrushSide;			  // First brushside for brush.  
	int					mNbBrushSides;		// Number of brushsides for brush.  
	int					mTextureIndex;		// Texture index.  
};

/**
 * BrushSide of the Q3 map.
 * The brushsides lump stores descriptions of brush bounding surfaces.
 */
struct TBrushSide
{
	int					mPlaneIndex;		  // Plane index.  
	int					mTextureIndex;		// Texture index.  
};

/**
 * MeshVert of the Q3 map.
 * The meshverts lump stores lists of vertex offsets, used to describe generalized triangle meshes.
 */
struct TMeshVert
{
	int					mMeshVert;			// Vertex index offset, relative to first vertex of corresponding face.
};

/**
 * Effect of the Q3 map.
 * The effects lump stores references to volumetric shaders (typically fog) which affect the rendering of a particular group of faces.
 */
struct TEffect
{
	char				mName[64];			// Effect shader.  
	int					mBrush;				// Brush that generated this effect.  
	int					mUnknown;			// Always 5, except in q3dm8, which has one effect with -1.  
};

/**
 * Lightmap of the Q3 map.
 * The lightmaps lump stores the light map textures used make surface lighting look more realistic.
 */
struct TLightMap
{
	unsigned char		mMapData[128][128][3];// Lightmap color data. RGB. 
};

/**
 * Light volume of the Q3 map.
 * The lightvols lump stores a uniform grid of lighting information used to illuminate non-map objects.
 */
struct TLightVol
{
	unsigned char		mAmbient[3];		// Ambient color component. RGB.  
	unsigned char		mDirectional[3];	// Directional color component. RGB.  
	unsigned char		mDir[2];			// Direction to light. 0=phi, 1=theta.  
};

/**
 * The Visibility data of the Q3 map.
 * The visdata lump stores bit vectors that provide cluster-to-cluster 
 * visibility information.
 */
struct TVisData
{
	int					mNbClusters;		// The number of clusters
  int					mBytesPerCluster;	// Bytes (8 bits) in the cluster's bitset
  unsigned char *	mBuffer;			// Array of bytes holding the cluster vis.
};

#ifdef _WIN32
#pragma()
#endif

enum EntityParserMode {PREKEY, KEY, PREVALUE, VALUE};

struct KeyValuePair {
  char key[100];
  char value[100];
};

class EntityDescriptor {
public:
  std::list<KeyValuePair *> keyValueList;
  void CleanUp();
  /**
   * Insert a key, get a value or null
   */ 
  const char * Get(const char * _key);
};

/** 
 * Parses the entity string.
 *
 * {
 *   "classname" "worldspawn"
 * }
 *
 * Each entity is contained inside a pair of squiggly brackets. Each entity
 * is a set of key value pairs. Key and value are surrounded by " characters.
 * Per entity there is exactly one classname_key
 */
class EntityParser {
  public:
    EntityParser(const char * _entitiyString);
    void Process(const char * _key, const char * _value);
    void EntityStart();
    void EntityEnd(); 
    void CleanUp();
    EntityDescriptor * GetDescriptorContaining(const char * _key, const char * _value);
    std::list<EntityDescriptor *> entityDescriptorList;
    EntityDescriptor * currentDescriptor;
};

/**
 * Used to tesselate patches inside the Grid of control points of a patch face
 * by two tier interpolation using quadratic bezier curves which describe
 * a biquadratic bezier surface
 */
class Bezier {
private:
    int level;
    std::vector<TVertex> vertex;
    std::vector<unsigned int> indexes;
    std::vector<unsigned int> trianglesPerRow;
    std::vector<unsigned int *> rowIndexes;

public:
    TVertex controls[9];
    void tessellate(int level);
    void render();
    void renderDebug();
};

class Q3BSP {
public:
	Q3BSP(const std::string fileName);

	// Renders all the faces (= lump 13) without using
	// PVS logic
	void DrawWithoutPVS();
	
	void DrawVertices();
	
	// with PVS logic
	void Draw(float camPos[3]);
	void Draw(float camera_x, float camera_y, float camera_z);
	
	// finds the leaf, the camera is currently in
	int GetCurrentLeafIndex(float camPos[3]);
	
	void ResetAlreadyVisible();
	
	bool IsPotentiallyVisible(int _camCluster, int _testCluster);
	
	int IsAlreadyVisible(int faceIndex);
	
	void MakeAlreadyVisible(int faceIndex);
	
	void RenderMeshFace(TFace f);

  void RenderPolygonFace(TFace f);
  
  void RenderPolygonFaceDebug(TFace f);

  void RenderPatchFaceDebug(int _faceIndex);
	
	void RenderLeaf(int _leafIndex);
	
	void RenderCluster(int _clusterIndex);

	void CleanUp();
	
	int GetFaceCount() { return faceCount; }
	
	int GetClusterCount() { return clusterCount; }
	
	// Sets the member variable outputEnd to that point on the line segment 
	// defined between _inputStart and _inputEnd, which is the first collision
	// point by the line segment and the solid part of the q3 level. By first
	// collision point is meant, that travelling from _inputStart to _inputEnd,
	// outputEnd is the first point where the traveller would collide into solid
	// walls.
	void Trace(float * _inputStart, float * _inputEnd);
	
	// Traverses BSP down according to the traceline until leaves are found.
	// Checks leaves for collision with the traceline
	void CheckNode(int _nodeIndex, 
	  float _startFraction, float _endFraction, 
	  float * _start, float * _end);
	
	//void CheckBrush(TBrush _brush, float * _inputStart, float * _inputEnd);
	void CheckBrush(TBrush _brush);
	
	/**
	 * Draws the convex volume of a brush that is specified by a brushes 
	 * brushsides. Every brushside stores an index into the planes lump. These
	 * planes indexed by the brushsides, are used to clip together the convex
	 * volume
	 */
	//void DrawBrush(int _brushIndex);

  /**
   * Returns good flag, which is set to false if bsp file could not
   * be found on harddrive
   */
  bool Good() {
    return good;
  }
	
private:
  void ParseVector(const char * _vector, float * _dest);

  /**
   * opens the file, loads content into buffer, keeps the buffer
   * allocated, someone else (CleanUp()) has to delete it.
   *
   * Returns 0 in case of success, -1 otherwise
   */
	int PerformFileHandling(const std::string fileName);

	// Parses the buffer, extracts the Q3BSP content
	void Parse();

  void TesselatePatches(int _tesselationEdges);
	
	float DotProduct(float *v1, float *v2);
	
	void DumpLeavesContainingFace(int _faceIndex);
	
	void ChangeGamma(unsigned char * _pImage, int _size, float _factor);
	
	void CreateLightmapTexture(unsigned int & _textureObjectID, unsigned char * _pImageBits, 
    int _width, int _height);
	
public:
  float info_player_start[3];
  int currentFace;
  int currentCluster;
  
  // These are the output variables of the collision detection algorithm
  // another output variabel, outputEnd, is a member of the class
  bool outputStartsOut;
  bool outputAllSolid;
  float outputFraction;
  float outputEnd[3];
  
  float * inputStart;
  float * inputEnd;
	
private:
  THeader * header;
	TFace * faces;
	TVertex * vertices;
	TEntity * entities;
	TTexture * textures;
	TPlane * planes;
	TNode * nodes;
	TLeaf * leaves;
	TLeafFace * leaffaces;
	TLeafBrush * leafbrushes;
	TModel * models;
	TBrush * brushes;
	TBrushSide * brushsides;
	TMeshVert * meshverts;
	TEffect * effects;
	TLightMap * lightmaps;
	TLightVol * lightvolumes;
	TVisData visdata;	
	int faceCount;
	int vertexCount;
	int leafCount;
	int bytesInAlreadyVisibleList;
	int clusterCount;
	unsigned char *alreadyVisible;
	// used to store the complete file content
	char * buffer;
  std::map<int, std::vector<Bezier *> *> bezierMap;
  std::vector<int> texIDtexObjMap;
  bool noVisData;
  bool nolightmaps;
  float gamma;
  unsigned int * lightmapArray;
  bool good;  
};

#endif

