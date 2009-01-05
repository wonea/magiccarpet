#ifndef _MD2LOADER
#define _MD2LOADER

#include <GL/glew.h>
#include <GL/glut.h>
#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <vector>
#include <math.h>

#include "PCXLoader.h"

#ifdef _WIN32
#pragma(1)
#endif
// md2 header
struct MD2Header
{
    int     ident;              // magic number. must be equal to "IDP2"
    int     version;            // md2 version. must be equal to 8

    int     skinwidth;          // width of the texture
    int     skinheight;         // height of the texture
    int     framesize;          // size of one frame in bytes

    int     num_skins;          // number of textures
    int     num_xyz;            // number of vertices
    int     num_st;             // number of texture coordinates
    int     num_tris;           // number of triangles
    int     num_glcmds;         // number of opengl commands
    int     num_frames;         // total number of frames

    int     ofs_skins;          // offset to skin names (64 bytes each)
    int     ofs_st;             // offset to s-t texture coordinates
    int     ofs_tris;           // offset to triangles
    int     ofs_frames;         // offset to frame data
    int     ofs_glcmds;         // offset to opengl commands
    int     ofs_end;            // offset to end of file

};

typedef float vec3[3];

// texture coordinates
struct texCoord
{
    short    s;
    short    t;
};

// vertex
struct vertex
{
    unsigned char   v[3];                // compressed vertex (x, y, z) coordinates
    unsigned char   lightnormalindex;    // index to a normal vector for the lighting
};

// frame
struct frame
{
    float       scale[3];       // scale values
    float       translate[3];   // translation vector
    char        name[16];       // frame name
    vertex    verts[1];       // first vertex of this frame
};

// triangle
struct triangle
{
    short   index_xyz[3];    // indexes to triangle's vertices
    short   index_st[3];     // indexes to vertices' texture coorinates
};

struct anim_t
{
    int     first_frame;            // first frame of the animation
    int     last_frame;             // number of frames
    int     fps;                    // number of frames per second
};

#ifdef _WIN32
#pragma()
#endif

// animation list
enum animType_t {
    STAND,
    RUN,
    ATTACK,
    PAIN_A,
    PAIN_B,
    PAIN_C,
    JUMP,
    FLIP,
    SALUTE,
    FALLBACK,
    WAVE,
    POINT,
    CROUCH_STAND,
    CROUCH_WALK,
    CROUCH_ATTACK,
    CROUCH_PAIN,
    CROUCH_DEATH, 
    DEATH_FALLBACK,
    DEATH_FALLFORWARD,
    DEATH_FALLBACKSLOW,
    BOOM,

    MAX_ANIMATIONS
};

class MD2Loader {
public:
	MD2Loader(const std::string _md2FileName,
      const std::string _textureFileName);
	void CleanUp();
  void DrawVertices();
  void DrawMesh();
  void DrawFrame();
  void DrawCurrentAnimation();
  void SetCurrentAnimType(animType_t _currentAnimType) {
    currentAnimType = _currentAnimType;
  }
  bool Good() {
    return good;
  }

private:
  // opens the file, loads content into buffer
	int PerformFileHandling(const std::string _md2FileName,
                           const std::string _textureFileName);

	// Parses the buffer, extracts the Q3BSP content
	void Parse();

  void CalculateNormal(float * p1, float * p2, float * p);

public:
  static anim_t animlist[21]; // animation list

private:
  char * buffer;
  MD2Header * header;  
  vec3 * vertices;
  triangle * mesh;
  int * glcmds;
  texCoord * texCoords;
  int textureObjectID;
  animType_t currentAnimType;
  int currentFrame;
  int nextFrame;
  float interpol;
  bool good;
};

#endif
