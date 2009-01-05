// Using this class:
// You can use the static method LoadPCXTexture() to retrieve an OpenGL 
// Object ID to the texture object. Use that id to bind the texture
// using glBindTexture(GL_TEXTURE_2D, 0);
//
// If you are using several texture units, you have to make sure that the
// texture is bound onto the correct texture unit:
// glActiveTextureARB(GL_TEXTURE0_ARB + 0); 
// glBindTexture(GL_TEXTURE_2D, textureObjectID);
// glEnable(GL_TEXTURE_2D);

#ifndef _PCXLOADER
#define _PCXLOADER

#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>

// Pragmas are needed to enforce the correct size of pcx header struct
#pragma pack(1)
struct PCXHeader {
  char	Identifier;        /* PCX Id Number (Always 0x0A) */
  char	Version;           /* Version Number */
  char	Encoding;          /* Encoding Format */
  char	BitsPerPixel;      /* Bits per Pixel */
  short	XStart;            /* Left of image */
  short	YStart;            /* Top of Image */
  short	XEnd;              /* Right of Image */
  short	YEnd;              /* Bottom of image */
  short	HorzRes;           /* Horizontal Resolution */
  short	VertRes;           /* Vertical Resolution */
  char	Palette[48];       /* 16-Color EGA Palette */
  char	Reserved1;         /* Reserved (Always 0) */
  char	NumBitPlanes;      /* Number of Bit Planes */
  short	BytesPerLine;      /* Bytes per Scan-line */
  short	PaletteType;       /* Palette Type */
  short	HorzScreenSize;    /* Horizontal Screen Size */
  short	VertScreenSize;    /* Vertical Screen Size */
  char	Reserved2[54];     /* Reserved (Always 0) */
};
#pragma pack()

class PCXLoader {
public:
  static int LoadPCXTexture(const char * _fileName);
};

#endif

