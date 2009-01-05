#include "PCXLoader.h"

int PCXLoader::LoadPCXTexture(const char * _fileName) {
  char * buffer = NULL;
  unsigned char * unscaledImageData = NULL;
  unsigned char * scaledImageData = NULL;
  unsigned char * data = NULL;
  unsigned char * palette = NULL;
  int size = 0;
  PCXHeader * header = NULL;
  int counter = 0;
  unsigned short width = 0;
  unsigned short height = 0;  
  long double scaledWidth = 0;
  long double scaledHeight = 0;  
  unsigned char c = '\0';
  unsigned int numRepeat = 0;
  unsigned int textureObject = 0;

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
	
	header = (PCXHeader *)buffer;
	
	std::cout << "Identifier: " << (unsigned int)header->Identifier << std::endl;
	std::cout << "Version: " << (unsigned int)header->Version << std::endl;
	std::cout << "Encoding: " << (unsigned int)header->Encoding << std::endl;
	std::cout << "BitsPerPixel: " << (unsigned int)header->BitsPerPixel << std::endl;
	std::cout << "XStart: " << (unsigned int)header->XStart << std::endl;
	std::cout << "YStart: " << (unsigned int)header->YStart << std::endl;
	std::cout << "XEnd: " << (unsigned int)header->XEnd << std::endl;
	std::cout << "YEnd: " << (unsigned int)header->YEnd << std::endl;
	std::cout << "HorzRes: " << (unsigned int)header->HorzRes << std::endl;
	std::cout << "VertRes: " << (unsigned int)header->VertRes << std::endl;
	
	if (0x0A != (char)header->Identifier) {
	  if (NULL != buffer) {
	    delete[] buffer;
	    buffer = NULL;
	  }
    std::cout << "PCX Identifier 0x0A not found! Terminating ..." << std::endl;
	  return -1;
  } else {
    std::cout << "PCX Identifier 0x0A found" << std::endl;
  }

  if (0x05 != (char)header->Version) {
	  if (NULL != buffer) {
	    delete[] buffer;
	    buffer = NULL;
	  }
    std::cout << "PCX Version 5 not found! Terminating ..." << std::endl;
	  return -1;
	} else {
    std::cout << "PCX Version 5 found" << std::endl;
  }

  // put pointer on palette part
  palette = (unsigned char *)&buffer[size-1];
  palette -= 768; // palette contains 256 colors, each 3 Byte RGB = 768 Bytes

  // the first byte of the palette data has to be 0x0C
  if (0x0C != (char)*palette) {
    if (NULL != buffer) {
	    delete[] buffer;
	    buffer = NULL;
	  }
    std::cout << "First byte of palette is not 0x0C! Terminating ..." << std::endl;
	  return -1;
  } else {
    std::cout << "First byte of palette is 0x0C" << std::endl;
  }
  // move pointer from control byte 0x0C to first byte of palette
  palette++;

  width = header->XEnd - header->XStart + 1;
  height = header->YEnd - header->YStart + 1;
  std::cout << "Extracted width: " << width << std::endl;
  std::cout << "Extracted height: " << height << std::endl;

  counter = 0;

  // unscaled image data contains rgba data resolved from the
  // palette of the pcx, but is not necessarily power of 2.
  // It has to be scaled to power of two for hardware which does
  // not support the non power of two extension. Hence the name
  // unscaled. 4 stands for RGBA data which is four bytes per pixel.
  unscaledImageData = new unsigned char[width*height*4];
  
  // put pointer into data
  data = (unsigned char *)buffer + sizeof(PCXHeader);

  // Resolve indexes from image data, into unscaledImageData using
  // the colors in the vga palette which is stored inside the 
  // pcx file.
  std::cout << "start decoding index data and resolving via vga color table ..." << std::endl;
  while (counter < (width * height)) {
    c = *data;
    data++;

    if (c > 0xbf) {
      numRepeat = 0x3f & c;
      c = *data;
      data++;

      for (unsigned int i = 0; i < numRepeat; i++) {
        unscaledImageData[counter * 4 + 0] = palette[c * 3 + 0]; // R
        unscaledImageData[counter * 4 + 1] = palette[c * 3 + 1]; // G
        unscaledImageData[counter * 4 + 2] = palette[c * 3 + 2]; // B
        unscaledImageData[counter * 4 + 3] = 255; // Alpha
        counter++;
      }
    } else {
        unscaledImageData[counter * 4 + 0] = palette[c * 3 + 0]; // R
        unscaledImageData[counter * 4 + 1] = palette[c * 3 + 1]; // G
        unscaledImageData[counter * 4 + 2] = palette[c * 3 + 2]; // B
        unscaledImageData[counter * 4 + 3] = 255;
        counter++;
    }
  }
  std::cout << "decoding index data and resolving via vga color table done" << std::endl;

  // find nearest power of two
  int widthPow = 0;
  int tempWidth = width;
  while (tempWidth) {
    tempWidth /= 2;
    widthPow++;
  }
  scaledWidth = (long double)::pow((long double)2.0L, (long double)(widthPow-1));
  std::cout << "Computed scaled width: " << scaledWidth << std::endl;
  int heightPow = 0;
  int tempHeight = height;
  while (tempHeight) {
    tempHeight /= 2;
    heightPow++;
  }
  scaledHeight = (long double)::pow((long double)2.0L, (long double)(heightPow-1));
  std::cout << "Computed scaled height: " << scaledHeight << std::endl;

  // allocate memory for scaled data
  scaledImageData = new unsigned char[(long int)scaledWidth*(long int)scaledHeight*4];

  // gluScaleImage
  gluScaleImage(GL_RGBA, // type of input and output data

    // input dimensions, data type and pointer to input data
    width, height,
    GL_UNSIGNED_BYTE,
    unscaledImageData,

    // output dimensions, data type and pointer to output data
    scaledWidth, scaledHeight,
    GL_UNSIGNED_BYTE,
    scaledImageData);

  // throw away memory for unscaled data
  if (NULL != unscaledImageData) {
	  delete[] unscaledImageData;
	  unscaledImageData = NULL;
	}

	// create texture
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
    GL_RGBA,
    scaledWidth, 
    scaledHeight, 
    0, // border
    //GL_LUMINANCE_ALPHA,
    GL_RGBA,
    //GL_GREEN|GL_RED,

    //GL_SHORT,
    //GL_UNSIGNED_INT,
    GL_UNSIGNED_BYTE,
    //&buffer[0][0][0]);
    scaledImageData);

  //::CheckErrors("");

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
  // throw away memory for scaled image
  if (NULL != scaledImageData) {
	    delete [] scaledImageData;
	    scaledImageData = NULL;
	  }

  // throw away buffer
	if (NULL != buffer) {
	  delete[] buffer;
	  buffer = NULL;
	}
	
	glDisable(GL_TEXTURE_2D);

  if ((NULL != buffer) || (NULL != unscaledImageData) || (NULL != scaledImageData)) {
    std::cout << "PCX Loader MEMORY LEAK DETECTED!!!!" << std::endl;

    // throw away buffer
    if (NULL != buffer) {
      delete [] buffer;
      buffer = NULL;
      std::cout << "buffer cleared" << std::endl;
    }

    // throw away memory for unscaled data
    if (NULL != unscaledImageData) {
	    delete [] unscaledImageData;
	    unscaledImageData = NULL;
      std::cout << "unscaled image data cleared" << std::endl;
	  }

    // throw away memory for scaled data
    if (NULL != scaledImageData) {
	    delete [] scaledImageData;
	    scaledImageData = NULL;
      std::cout << "scaled image data cleared" << std::endl;
	  }
  }
	
	return textureObject;
}

