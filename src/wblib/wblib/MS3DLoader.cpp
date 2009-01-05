#include "MS3DLoader.h"

wbLib::ms3d::MS3DLoader::MS3DLoader() {
  buffer = NULL;
  meshes = NULL;
  cleanUp();
}

/**
 * Loads a md2 and its texture
 */
wbLib::ms3d::MS3DLoader::MS3DLoader(const std::string _fileName) {
  buffer = NULL;
  meshes = NULL;
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
  wbLib::ms3d::MS3DHeader * header = NULL;

  if (NULL == buffer) {
    std::ios_base::failure f("MS3DLoader::parse() - buffer not filled");
    throw f;
  }

  // set pointer on buffer so that original buffer pointer 
  // stays untouched for memory deallocation
  ptr = buffer;

  // cast to MS3DHeader
	header = (wbLib::ms3d::MS3DHeader *)ptr;
  
  // advance pointer
	ptr += sizeof(wbLib::ms3d::MS3DHeader);

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
  vertices = (MS3DVertex *)ptr;
  // advance pointer
  ptr += (sizeof(MS3DVertex) * numVertices);

  // read number of triangles 2 byte
  numTriangles = *(unsigned short *)ptr;
  // advance pointer
  ptr += sizeof(unsigned short);
  std::cout << "Number of Triangles: " << numTriangles << std::endl;

  // set pointer onto triangles
  triangles = (MS3DTriangle *)ptr;
  // advance pointer
  ptr += (sizeof(MS3DTriangle) * numTriangles);

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
  meshes = new MS3DGroup[numMeshes];
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

  // set pointer onto triangles
  materials = (MS3DMaterial *)ptr;
  // advance pointer
  ptr += (sizeof(MS3DMaterial) * numMaterials);
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
      const MS3DTriangle * tri = &triangles[triangleIndex];

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
}