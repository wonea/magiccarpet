#include "MS3DLoader.h"

wbLib::ms3d::MS3DLoader::MS3DLoader() {
  buffer = NULL;
  meshes = NULL;
  joints = NULL;
  cleanUp();
}

/**
 * Loads a md2 and its texture
 */
wbLib::ms3d::MS3DLoader::MS3DLoader(const std::string _fileName) {
  buffer = NULL;
  meshes = NULL;
  joints = NULL;
	cleanUp();

  performFileHandling(_fileName);
	parse();
}

/**
 * Loads a md2 and its texture
 */
void wbLib::ms3d::MS3DLoader::load(const std::string _fileName) {
  buffer = NULL;
  meshes = NULL;
  joints = NULL;
	cleanUp();

  performFileHandling(_fileName);
	parse();
}

/**
 * Opens the file, loads content into buffer
 *
 * returns -1 in case of error, returns 0 in case of success
 */
void wbLib::ms3d::MS3DLoader::performFileHandling(const std::string _fileName) {
	buffer = NULL;

	// std::ios::ate - set the initial position to the end of the file
	std::ifstream file;
	file.open(_fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
    std::ios_base::failure f("MS3DLoader::performFileHandling() - Could not find model file");
    throw f;
	}
	int size = file.tellg();
	file.seekg (0, std::ios::beg);

	buffer = new char[size];
	file.read(buffer, size);

	if (!file.good()) {
		delete[] buffer;
		buffer = NULL;
    file.close();
    
    std::ios_base::failure f("MS3DLoader::performFileHandling() - Could not read from model file");
    throw f;
	}

	file.close();
}

void wbLib::ms3d::MS3DLoader::parse() throw (std::ios_base::failure) {
  const char * ptr = NULL;
  wbLib::ms3d::MS3D_Header * header = NULL;

  if (NULL == buffer) {
    std::ios_base::failure f("MS3DLoader::parse() - buffer not filled");
    throw f;
  }

  // set pointer on buffer so that original buffer pointer 
  // stays untouched for memory deallocation
  ptr = buffer;

  // cast to MS3D_Header
	header = (wbLib::ms3d::MS3D_Header *)ptr;
  
  // advance pointer
	ptr += sizeof(wbLib::ms3d::MS3D_Header);

  if (strncmp(header->id, "MS3D000000", 10) != 0) {
    cleanUp();
    std::cout << "MS3DLoader::parse() - Not A Valid Milkshape3D Model File." << std::endl;
    std::ios_base::failure f("MS3DLoader::parse() - Not A Valid Milkshape3D Model File.");
    throw f;
  } else {
    std::cout << "id: " << header->id << std::endl;
  }

  if (header->version < 3 || header->version > 4) {
    cleanUp();
    std::cout << "MS3DLoader::parse() - Unhandled File Version. Only Milkshape3D Version 1.3 And 1.4 Is Supported." << std::endl;
    std::ios_base::failure f("MS3DLoader::parse() - Unhandled File Version. Only Milkshape3D Version 1.3 And 1.4 Is Supported.");
    throw f;
  } else {
    std::cout << "version: " << header->version << std::endl;
  }

  // read number of vertices 2 byte
  numVertices = *(unsigned short *)ptr;
  // advance pointer
  ptr += sizeof(unsigned short);
  std::cout << "Number of Vertices: " << numVertices << std::endl;

  // set pointer onto vertices
  vertices = (MS3D_Vertex *)ptr;
  // advance pointer
  ptr += (sizeof(MS3D_Vertex) * numVertices);

  // read number of triangles 2 byte
  numTriangles = *(unsigned short *)ptr;
  // advance pointer
  ptr += sizeof(unsigned short);
  std::cout << "Number of Triangles: " << numTriangles << std::endl;

  // set pointer onto triangles
  triangles = (MS3D_Triangle *)ptr;
  // advance pointer
  ptr += (sizeof(MS3D_Triangle) * numTriangles);

  // swizzle t texture coordinate from ms3d spec to OpenGL convention
  for (int i = 0; i < numTriangles; i++) {
    triangles[i].t[0] = 1.0f - triangles[i].t[0];
    triangles[i].t[1] = 1.0f - triangles[i].t[1];
    triangles[i].t[2] = 1.0f - triangles[i].t[2];
  }

  // read number of groups (aka meshes) (2 byte)
  numMeshes = *(unsigned short *)ptr;
  // advance pointer
  ptr += sizeof(unsigned short);
  std::cout << "Number of Meshes: " << numMeshes << std::endl;

  // read groups/meshes
  meshes = new MS3D_Group[numMeshes];
  for (int i = 0; i < numMeshes; i++) {
    meshes[i].flags = *(unsigned char *)ptr;
    ptr += sizeof(unsigned char);

    meshes[i].name = (char *)ptr;
    ptr += (sizeof(unsigned char) * 32);
    //ptr += 32;
    //std::cout << "group name: " << meshes[i].name << std::endl;

    meshes[i].numTriangles = *(unsigned short *)ptr;
    ptr += sizeof(unsigned short);
    std::cout << "num triangles: " << meshes[i].numTriangles << std::endl;

    meshes[i].triangleIndices = (unsigned short *)ptr;
    ptr += (sizeof(unsigned short) * meshes[i].numTriangles);

    meshes[i].materialIndex = *(char *)ptr;
    ptr += sizeof(char);
  }

  // read number of materials (2 byte)
  numMaterials = *(unsigned short *)ptr;
  // advance pointer
  ptr += sizeof(unsigned short);
  std::cout << "Number of Materials: " << numMaterials << std::endl;

  // set pointer onto materials
  materials = (MS3D_Material *)ptr;
  // advance pointer
  ptr += (sizeof(MS3D_Material) * numMaterials);

  // read keyframer data
  animationFPS = *(float *)ptr;
  ptr += sizeof(float);
  std::cout << "animationFPS: " << animationFPS << std::endl;
  currentTime = *(float *)ptr; 
  ptr += sizeof(float);
  std::cout << "currentTime: " << currentTime << std::endl;
  totalFrames = *(int *)ptr;
  ptr += sizeof(int);
  std::cout << "totalFrames: " << totalFrames << std::endl;

  // read number of joints
  numJoints = *(unsigned short *)ptr;
  ptr += sizeof(unsigned short);
  std::cout << "numJoints: " << numJoints << std::endl;

  // Then come nNumJoints joints
  // there is a static part in every joint which consists of 
  //
  //byte            flags;                              // SELECTED | DIRTY
  //char            name[32];                           //
  //char            parentName[32];                     //
  //float           rotation[3];                        // local reference matrix
  //float           position[3];
  //
  //word            numKeyFramesRot;                    //
  //word            numKeyFramesTrans;                  //
  //
  // After that, there are numKeyFramesRot
  //
  //typedef struct // 16 bytes
  //{
  //    float           time;                               // time in seconds
  //    float           rotation[3];                        // x, y, z angles
  //} ms3d_keyframe_rot_t;
  //
  // After that, there are numKeyFramesTrans
  //
  //typedef struct // 16 bytes
  //{
  //    float           time;                               // time in seconds
  //    float           position[3];                        // local position
  //} ms3d_keyframe_pos_t;

  MS3D_Joint * joint = NULL;
  joints = new WB_MS3D_Joint[numJoints];
  for (int i = 0; i < numJoints; i++) {
    joint = (MS3D_Joint *)ptr;
    ptr += sizeof(unsigned char);
    ptr += (sizeof(char) * 2 * 32);
    ptr += (sizeof(float) * 2 * 3);
    ptr += (sizeof(unsigned short) * 2);

    std::cout << "Joint " << i << " flags: " << joint->flags << std::endl;
    std::cout << "Joint " << i << " name: " << joint->name << std::endl;
    std::cout << "Joint " << i << " parentName: " << joint->parentName << std::endl;
    std::cout << "Joint " << i << " rotation: (" << joint->rotation[0] << "|" << joint->rotation[1] << "|" << joint->rotation[2] << ")" << std::endl;
    std::cout << "Joint " << i << " position: (" << joint->position[0] << "|" << joint->position[1] << "|" << joint->position[2] << ")" << std::endl;
    std::cout << "Joint " << i << " numKeyFramesRot: " << joint->numKeyFramesRot << std::endl;
    std::cout << "Joint " << i << " numKeyFramesTrans: " << joint->numKeyFramesTrans << std::endl;

    joints[i].flags = joint->flags;
    memcpy(joints[i].name, joint->name, 32);
    memcpy(joints[i].parentName, joint->parentName, 32);
    joints[i].rotation[0] = joint->rotation[0];
    joints[i].rotation[1] = joint->rotation[1];
    joints[i].rotation[2] = joint->rotation[2];
    joints[i].position[0] = joint->position[0];
    joints[i].position[1] = joint->position[1];
    joints[i].position[2] = joint->position[2];
    joints[i].numKeyFramesRot = joint->numKeyFramesRot;
    joints[i].numKeyFramesTrans = joint->numKeyFramesTrans;

    // copy frame rotations
    if (0 < joint->numKeyFramesRot) {
      joints[i].keyFramesRot = new MS3D_Keyframe_Rot[joint->numKeyFramesRot];
      MS3D_Keyframe_Rot * kfRotPtr = (MS3D_Keyframe_Rot *)ptr;
      for (int j = 0; j < joint->numKeyFramesRot; j++) {
        joints[i].keyFramesRot[j].time = kfRotPtr->time;
        joints[i].keyFramesRot[j].rotation[0] = kfRotPtr->rotation[0];
        joints[i].keyFramesRot[j].rotation[1] = kfRotPtr->rotation[1];
        joints[i].keyFramesRot[j].rotation[2] = kfRotPtr->rotation[2];

        kfRotPtr++;
      }
    } else {
      joints[i].keyFramesRot = NULL;
    }

    // advance pointer
    ptr += (sizeof(MS3D_Keyframe_Rot) * joint->numKeyFramesRot);

    // TODO: copy frame translations
    if (0 < joint->numKeyFramesTrans) {
      joints[i].keyFramesTrans = new MS3D_Keyframe_Pos[joint->numKeyFramesTrans];
      MS3D_Keyframe_Pos * kfTransPtr = (MS3D_Keyframe_Pos *)ptr;
      for (int j = 0; j < joint->numKeyFramesTrans; j++) {
        joints[i].keyFramesTrans[j].time = kfTransPtr->time;
        joints[i].keyFramesTrans[j].position[0] = kfTransPtr->position[0];
        joints[i].keyFramesTrans[j].position[1] = kfTransPtr->position[1];
        joints[i].keyFramesTrans[j].position[2] = kfTransPtr->position[2];

        kfTransPtr++;
      }
    } else {
      joints[i].keyFramesTrans = NULL;
    }

    // advance pointer
    ptr += (sizeof(MS3D_Keyframe_Pos) * joint->numKeyFramesTrans);
  }
  joint = NULL;

  findParentID();
}

/**
 * The field parentJoint from the WB_MS3D_Joint struct cannot be loaded
 * from the model file, as the hierarchie is not stored by indices into
 * the joint array, but by names of joints. This methode deduces the 
 * parentIDs from the names. It therefore scans the joint array repeatedly
 * for each joint. parentIDs are just indices into the joint array.
 */
void wbLib::ms3d::MS3DLoader::findParentID() {
  if (NULL == joints) {
    return;
  }

  WB_MS3D_Joint * ptr = joints;
  for (int i = 0; i < numJoints; i++) {

    //std::cout << "Joint: " << i << " Parentjointname: " << ptr->parentName << std::endl;

    // empty parent name means that this joint has no parent
    // store a -1 to indicate that fact
    if (0 == strcmp(ptr->parentName, "")) {
      //std::cout << "Joint: " << i << " has no parent" << std::endl;
      ptr->parentJoint = -1;
    } else {

      // search name and store index
      WB_MS3D_Joint * searchPtr = joints;
      for (int j = 0; j < numJoints; j++) {
        if (0 == strcmp(searchPtr->name, ptr->parentName)) {
          ptr->parentJoint = j;
          break;
        }
        // advance pointer
        searchPtr++;
      }
    }

    /*if (-1 == ptr->parentJoint) {
      std::cout << "Joint " << i << " called " << ptr->name << " has no parent" << std::endl;
    } else {
      std::cout << "Joint " << i << " called " << ptr->name << " has " 
        << joints[ptr->parentJoint].name << " as parent" << std::endl;
    }*/

    // advance the pointer
    ptr++;
  }
}

void wbLib::ms3d::MS3DLoader::drawVertices() {
  glPushMatrix();
  glBegin(GL_POINTS);
  for (int i = 0; i < numVertices; i++) {
    glVertex3f(vertices[i].vertex[0], vertices[i].vertex[1], vertices[i].vertex[2]);
  }
  glEnd();
  glPopMatrix();
}

void wbLib::ms3d::MS3DLoader::drawTriangles() {
  glPushMatrix();
  glBegin(GL_TRIANGLES);
  for (int i = 0; i < numTriangles; i++) {
    glVertex3f(vertices[triangles[i].vertexIndices[0]].vertex[0],
      vertices[triangles[i].vertexIndices[0]].vertex[1],
      vertices[triangles[i].vertexIndices[0]].vertex[2]);

    glVertex3f(vertices[triangles[i].vertexIndices[1]].vertex[0],
      vertices[triangles[i].vertexIndices[1]].vertex[1],
      vertices[triangles[i].vertexIndices[1]].vertex[2]);

    glVertex3f(vertices[triangles[i].vertexIndices[2]].vertex[0],
      vertices[triangles[i].vertexIndices[2]].vertex[1],
      vertices[triangles[i].vertexIndices[2]].vertex[2]);
  }
  glEnd();
  glPopMatrix();
}

void wbLib::ms3d::MS3DLoader::drawGroups() {
  glPushMatrix();

  for (int i = 0; i < numMeshes; i++ ) {

    glBegin(GL_TRIANGLES);
    for (int j = 0; j < meshes[i].numTriangles; j++) {

      int triangleIndex = meshes[i].triangleIndices[j];
      const MS3D_Triangle * tri = &triangles[triangleIndex];

      for (int k = 0; k < 3; k++) {
				int index = tri->vertexIndices[k];

				glNormal3fv(tri->vertexNormals[k]);
				glTexCoord2f(tri->s[k], tri->t[k]);
        glVertex3fv(vertices[index].vertex);
			}

      /*glVertex3f(vertices[triangles[i].vertexIndices[0]].vertex[0],
        vertices[triangles[i].vertexIndices[0]].vertex[1],
        vertices[triangles[i].vertexIndices[0]].vertex[2]);

      glVertex3f(vertices[triangles[i].vertexIndices[1]].vertex[0],
        vertices[triangles[i].vertexIndices[1]].vertex[1],
        vertices[triangles[i].vertexIndices[1]].vertex[2]);

      glVertex3f(vertices[triangles[i].vertexIndices[2]].vertex[0],
        vertices[triangles[i].vertexIndices[2]].vertex[1],
        vertices[triangles[i].vertexIndices[2]].vertex[2]);*/
    }
    glEnd();
  }
  glPopMatrix();
}

void wbLib::ms3d::MS3DLoader::cleanUp() {
	if (NULL != buffer) {
		delete [] buffer;
		buffer = NULL;
	}
  vertices = NULL;
  triangles = NULL;
  if (NULL != meshes) {
    delete [] meshes;
    meshes = NULL;
  }
  materials = NULL;
  

  numVertices = 0;
  numTriangles = 0;
  numMeshes = 0;
  numMaterials = 0;

  animationFPS = 0.0f;
	currentTime = 0.0f;
	totalFrames = 0;  

  if (joints != NULL) {
    for (int i = 0; i < numJoints; i++) {
      // cleanUp frame rotations
      if (NULL != joints[i].keyFramesRot) {
        delete [] joints[i].keyFramesRot;
        joints[i].keyFramesRot = NULL;
      }
      // cleanUp frame translations
      if (NULL != joints[i].keyFramesTrans) {
        delete [] joints[i].keyFramesTrans;
        joints[i].keyFramesTrans = NULL;
      }
    }
    delete [] joints;
    joints = NULL;
  }

  numJoints = 0;
}