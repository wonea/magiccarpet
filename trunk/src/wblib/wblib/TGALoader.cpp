#include "TGAFile.h"

int TGAFile::LoadTGATexture(const char * _fileName) {
  char * buffer = NULL;
  int size = 0;
  TGAHeader * header = NULL;
  char * pointer = NULL;
  unsigned int imageDataSize = 0;
  unsigned int textureObject = 0;
  GLenum texFormat = GL_RGB;

	// std::ios::ate - set the initial position to the end of the file
	std::ifstream file;
	file.open(_fileName, std::ios::in | std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		return -1;
	}
	size = file.tellg();
	file.seekg (0, std::ios::beg);

	buffer = new char[size];
	file.read(buffer, size);

	if (!file.good()) {
		delete[] buffer;
		buffer = NULL;
	}

	file.close();
	
	header = (TGAHeader *)buffer;
	
	//std::cout << "imageIDlength: " << (unsigned int)header->imageIDlength << std::endl;
	//std::cout << "colorMapType: " << (unsigned int)header->colorMapType << std::endl;
	//std::cout << "imageTypeCode: " << (unsigned int)header->imageTypeCode << std::endl;
	//std::cout << "colorMapOrigin: " << header->colorMapOrigin << std::endl;
	//std::cout << "colorMapLength: " << header->colorMapLength << std::endl;
	//std::cout << "entrySize: " << (unsigned int)header->entrySize << std::endl;
	//std::cout << "imageXOrigin: " << header->imageXOrigin << std::endl;
	//std::cout << "imageYOrigin: " << header->imageYOrigin << std::endl;
	//std::cout << "imageWidth: " << header->imageWidth << std::endl;
	//std::cout << "imageHeight: " << header->imageHeight << std::endl;
	//std::cout << "bitCount: " << (unsigned int)header->bitCount << std::endl;
	//std::cout << "imageDescriptor: " << (unsigned int)header->imageDescriptor << std::endl;
	
	imageDataSize = header->imageWidth * header->imageHeight * ((unsigned int)header->bitCount / 8);
	
	//std::cout << "**** fileSize: " << size << std::endl;
	//std::cout << "TGAHeader size: " << sizeof(TGAHeader) << std::endl;
	//std::cout << "ImageDataSize: " << imageDataSize << std::endl;
	
	if (2 != (unsigned int)header->imageTypeCode) {
	  if (NULL != buffer) {
	    delete[] buffer;
	    buffer = NULL;
	  }
	  return -1;
	}
	
	// swap BGR to RGB
	pointer = buffer;
	pointer += sizeof(TGAHeader);
	
	for (int i = 0; i < (header->imageWidth * header->imageHeight); i++) {
	  unsigned char temp = *pointer;
	  *pointer = *(pointer+2);
	  *(pointer+2) = temp;
	  pointer += ((unsigned int)header->bitCount / 8);
	}
	
	// Create OpenGL texture object
	pointer = buffer;
	pointer += sizeof(TGAHeader);
	
	switch ((unsigned int)header->bitCount / 8) {
	  case 4:
	    texFormat = GL_RGBA;
	    break;
	    
	  default:
	    texFormat = GL_RGB;
	    break;
	}
	
	glEnable(GL_TEXTURE_2D);    
  glGenTextures(1, &textureObject);
  glBindTexture(GL_TEXTURE_2D, textureObject);
  //glPixelStorei(GL_UNPACK_ALIGNMENT, 3);
  //::CheckErrors("");

  // Generate The Texture
  //  void glTexImage2D( GLenum target,
  //  GLint level,
  //  GLint internalformat,
  //  GLsizei width,
  //  GLsizei height,
  //  GLint border,
  //  GLenum format,
  //  GLenum type,
  //  const GLvoid *pixels )
  //
  // The last three values supply the data from which the texture is 
  // created plus the information how this data is arranged and how it
  // should be interpreted by OpenGL.
  // format - how many parts make up one texel (e.g. GL_RGB, one
  // texel is specified by a R, G and B information)
  // type - which datatype is used to specify one information (e.g. when
  // GL_RGB is used for format, which datatype is used to store the R, G and
  // B components)
  // buffer - the memory location that holds the data. The data is arranged
  // as a successive stream of [format] information which is stored using
  // [type] datatype
  glTexImage2D(GL_TEXTURE_2D, 
    0,
    //png_colorCount, 
    GL_RGB,
    header->imageWidth, 
    header->imageHeight, 
    0, // border
    //GL_LUMINANCE_ALPHA,
    texFormat,
    //GL_GREEN|GL_RED,

    //GL_SHORT,
    //GL_UNSIGNED_INT,
    GL_UNSIGNED_BYTE,
    //&buffer[0][0][0]);
    pointer);

  //::CheckErrors("");

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	if (NULL != buffer) {
	  delete[] buffer;
	  buffer = NULL;
	}
	
	glDisable(GL_TEXTURE_2D);
	
	return textureObject;
}

