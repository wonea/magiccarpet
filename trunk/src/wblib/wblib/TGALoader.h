// Using this class:
// You can use the static method LoadJPEGTexture() to retrieve an OpenGL 
// Object ID to the texture object. Use that id to bind the texture
// using glBindTexture(GL_TEXTURE_2D, 0);
//
// If you are using several texture units, you have to make sure that the
// texture is bound onto the correct texture unit:
// glActiveTextureARB(GL_TEXTURE0_ARB + 0); 
// glBindTexture(GL_TEXTURE_2D, textureObjectID);
// glEnable(GL_TEXTURE_2D);

#ifndef _TGALOADER
#define _TGALOADER

#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GL/glut.h>

// Pragmas are needed to force 18 byte size of tga header struct
#pragma pack(1)
struct TGAHeader {
  unsigned char imageIDlength; // number of characters in the ID field (1 Byte), no ID Field
                               // if a zero is stored in this element
  unsigned char colorMapType;  // Type of colormap, always 0 (1 Byte)
  unsigned char imageTypeCode; // 8 Bit paletized = 1
				                       // uncompressed RGB oder RGBA = 2
                               // uncompressed grayscale = 3
                               // Run-Length Encoded RGB oder RGBA = 10
                               // compressed grayscale = 11 (1 Byte)
  short int colorMapOrigin;    // always 0 (2 Byte)
  short int colorMapLength;    // always 0 (2 Byte)
  unsigned char entrySize;     // size of one entry in the palette (1 Byte)
  short int imageXOrigin;      // always 0 (2 Byte)
  short int imageYOrigin;      // always 0 (2 Byte)
  short int imageWidth;        // Width in Pixels (2 Byte)
  short int imageHeight;       // Height in Pixels (2 Byte)
  unsigned char bitCount;      // Bits per Pixel (1 Byte)
  unsigned char imageDescriptor; // 24 bit = 0x00; 32 bit = 0 0x08 (1 Byte)
};
#pragma pack()

class TGALoader {
public:
  static int LoadTGATexture(const char * _fileName);
};

#endif

