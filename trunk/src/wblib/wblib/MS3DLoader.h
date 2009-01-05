#ifndef _MS3DLoader
#define _MS3DLoader

#include <GL/glew.h>
#include <GL/glut.h>
#include <string>
#include <iostream>
#include <fstream>

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
        unsigned char flags;                              // SELECTED | HIDDEN
        char * name;                           //
        unsigned short numTriangles;                       //
        unsigned short * triangleIndices;      // the groups group the triangles
        char materialIndex;                      // -1 = no material
    } PACK_STRUCT;

    /*struct Material {
	    float ambient[4], diffuse[4], specular[4], emissive[4];
	    float shininess;
	    int texture;
	    char * textureFilename;
    };*/

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

    struct MS3D_Keyframe_Rot { // 16 bytes
        float           time;                               // time in seconds
        float           rotation[3];                        // x, y, z angles
    } PACK_STRUCT;

    struct MS3D_Keyframe_Pos { // 16 bytes
        float           time;                               // time in seconds
        float           position[3];                        // local position
    } PACK_STRUCT;

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
        unsigned char   flags;                              // SELECTED | DIRTY

        char            name[32];                           //
        char            parentName[32];                     //

        float           rotation[3];                        // local reference matrix
        float           position[3];

        unsigned short  numKeyFramesRot;                    //
        unsigned short  numKeyFramesTrans;                  //

        int parentJoint; // index into joint array, -1 if no parent joint exists

        MS3D_Keyframe_Rot * keyFramesRot;      // local animation matrices
        MS3D_Keyframe_Pos * keyFramesTrans;  // local animation matrices
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
     * 2. use model
     *
     * 3. clean up
     * ms3dLoader.cleanUp();
     *
     * File Format Specification:
     * 14 Byte MS3DHeader
     * 2 Byte number of vertices
     * 
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

    private:
      void performFileHandling(const std::string _md2FileName) throw (std::ios_base::failure);
      void parse() throw (std::ios_base::failure);
      void findParentID();

    private:
      char * buffer;
      unsigned short numVertices;
      MS3D_Vertex * vertices;
      int numTriangles;
      MS3D_Triangle * triangles;
      int numMeshes;
      MS3D_Group * meshes;
      int numMaterials;
      MS3D_Material * materials;
      unsigned short numJoints;
      WB_MS3D_Joint * joints;

      float animationFPS;
	    float currentTime;
	    int totalFrames;
      
    };
  }
}

#endif