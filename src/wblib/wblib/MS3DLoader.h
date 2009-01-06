#ifndef _MS3DLoader
#define _MS3DLoader

#include <GL/glew.h>
#include <GL/glut.h>
#include <string>
#include <iostream>
#include <fstream>
#include "Maths.h"
#include "Timer.h"
#include "JPEGLoader.h"

namespace wbLib {
  namespace ms3d {

// byte-align structures
#ifdef _MSC_VER
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error you must byte-align these structures with the appropriate compiler directives
#endif

    struct MS3D_Header {
      char id[10];
      int version;
    } PACK_STRUCT;

    /*struct Vertex {
	    char boneID;
	    float location[3];
    };*/

    struct MS3D_Vertex {
	    unsigned char flags;
	    float vertex[3];
	    char boneID; // -1 = no bone
	    unsigned char refCount;
    } PACK_STRUCT;

    /*struct Triangle {
	    float vertexNormals[3][3];
	    float s[3], t[3];
	    int vertexIndices[3];
    };*/

    // Triangle information
    struct MS3D_Triangle {
	    unsigned short flags;
	    unsigned short vertexIndices[3];
	    float vertexNormals[3][3];
	    float s[3];
      float t[3];
	    unsigned char smoothingGroup;
	    unsigned char groupIndex;
    } PACK_STRUCT;

    /*struct Mesh {
	    int materialIndex;
	    int numTriangles;
	    int * triangleIndices;
    };*/

    struct MS3D_Group {
        unsigned char flags; // SELECTED | HIDDEN
        char * name; // exactly 32 byte
        unsigned short numTriangles;
        unsigned short * triangleIndices; // the groups triangles
        char materialIndex; // -1 = no material, otherwise index into the material array
    } PACK_STRUCT;

    /*struct Material {
	    float ambient[4], diffuse[4], specular[4], emissive[4];
	    float shininess;
	    int texture;
	    char * textureFilename;
    };*/

    // A lot of the shebang in this structure does not interest us.
    // We also need a new field which stores the OpenGL Texture Object ID
    // if the material has a texture. So we use this structure only
    // for parsing purposes as its size is important to advance the 
    // buffer pointer correctly
    struct MS3D_Material {
        char            name[32];                           //
        float           ambient[4];                         //
        float           diffuse[4];                         //
        float           specular[4];                        //
        float           emissive[4];                        //
        float           shininess;                          // 0.0f - 128.0f
        float           transparency;                       // 0.0f - 1.0f
        char            mode;                               // 0, 1, 2 is unused now
        char            texture[128];                        // texture.bmp
        char            alphamap[128];                       // alpha.bmp
    } PACK_STRUCT;

    struct WB_MS3D_Material {
        //char            name[32];                           //
        float           ambient[4];                         //
        float           diffuse[4];                         //
        float           specular[4];                        //
        float           emissive[4];                        //
        float           shininess;                          // 0.0f - 128.0f
        float           transparency;                       // 0.0f - 1.0f
        //char            mode;                               // 0, 1, 2 is unused now
        //char            texture[128];                        // texture.bmp
        //char            alphamap[128];                       // alpha.bmp

        // Is used to store the identifier that allows to
        // bind the texture for OpenGL
        int textureObjectID;
    } PACK_STRUCT;

    struct MS3D_Keyframe_Rot { // 16 bytes
        float           time;                               // time in seconds
        float           rotation[3];                        // x, y, z angles
    } PACK_STRUCT;

    struct MS3D_Keyframe_Pos { // 16 bytes
        float           time;                               // time in seconds
        float           position[3];                        // local position
    } PACK_STRUCT;

    // A lot of the shebang in this structure does not interest us.
    // So we use this structure only
    // for parsing purposes as its size is important to advance the 
    // buffer pointer correctly 
    struct MS3D_Joint {
        unsigned char   flags;                              // SELECTED | DIRTY

        char            name[32];                           //
        char            parentName[32];                     //

        float           rotation[3];                        // local reference matrix
        float           position[3];

        unsigned short  numKeyFramesRot;                    //
        unsigned short  numKeyFramesTrans;                  //

        MS3D_Keyframe_Rot * keyFramesRot;      // local animation matrices
        MS3D_Keyframe_Pos * keyFramesTrans;  // local animation matrices
    } PACK_STRUCT;

    // This structure adds a index into the joint array onto the joint
    // which is the parent joint of this joint
    struct WB_MS3D_Joint {
        unsigned char   flags; // SELECTED | DIRTY

        char            name[32];
        char            parentName[32];

        // local reference matrix, reformulated as matrix and
        // concatenated to the parent joints
        // absolute matrix gives the joints absolute position
        // and rotation (Every keyframe uses these as base reference.
        // Is this true !?!)
        float           rotation[3]; // stores the local rotation
        float           position[3]; // stores the local translation

        unsigned short  numKeyFramesRot;
        unsigned short  numKeyFramesTrans;

        int currentTranslationKeyframe;
        int currentRotationKeyframe;
			  Matrix4X4f final;

        int parentJoint; // index into joint array, -1 if no parent joint exists
        Matrix4X4f relative; // gives relative rotation and position. Used to get from parent joint to this joint
        Matrix4X4f absolute; // matrix which translates (0|0|0) to this joints position and rotation

        MS3D_Keyframe_Rot * keyFramesRot; // local animation matrices
        MS3D_Keyframe_Pos * keyFramesTrans; // local animation matrices
    } PACK_STRUCT;


// Default alignment
#ifdef _MSC_VER
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT

    /**
     * USAGE:
     * 1. Load model
     * try {
     *   wbLib::MS3DLoader ms3dLoader("model.ms3d", "tex.bmp");
     * } catch (std::ios_base::failure f) {
     *   std::cout << f.what() << std::endl;
     * }
     *
     * 2. use model:
     * call ms3dLoader.advanceAnimation() in your message loop.
     * After advanceAnimation() has been called, call drawGroupsUsingJoints()
     * to draw the animated model
     *
     * 3. clean up
     * ms3dLoader.cleanUp();
     */
    class MS3DLoader {
    public:
      MS3DLoader();
	    MS3DLoader(const std::string _fileName) throw (std::ios_base::failure);
      void load(const std::string _fileName) throw (std::ios_base::failure);
	    void cleanUp();

      void drawVertices();
      void drawTriangles();
      void drawGroups();
      void drawJoints();
      void drawJointsAnimated();
      void drawGroupsUsingJoints();
      void advanceAnimation();

    private:
      void performFileHandling(const std::string _md2FileName) throw (std::ios_base::failure);
      void parse() throw (std::ios_base::failure);
      void findParentID();
      void setupJoints();
      void restart();

    private:
      char * buffer;
      unsigned short numVertices;
      MS3D_Vertex * vertices;
      int numTriangles;
      MS3D_Triangle * triangles;
      int numMeshes;
      MS3D_Group * meshes;
      int numMaterials;
      WB_MS3D_Material * materials;
      unsigned short numJoints;
      WB_MS3D_Joint * joints;

      float animationFPS;
	    float currentTime;
	    int totalFrames;

      wbLib::Timer timer;      
    };
  }
}

#endif