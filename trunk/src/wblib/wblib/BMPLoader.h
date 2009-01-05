#ifndef _BMPFILE
#define _BMPFILE

#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "WBResource.h"

namespace wbLib {

  //typedef unsigned short WORD;
  //typedef unsigned int DWORD;
  //typedef long LONG;

  //#pragma pack(1)
#pragma pack(push, 1)
  struct BitmapFileHeader { // 14 Byte
    unsigned short bfType; // 2 Byte
    unsigned int bfSize; // 4 Byte
    unsigned short bfReserved1; // 2 Byte
    unsigned short bfReserved2; // 2 Byte
    //unsigned int bfOffBits:1;
    unsigned int bfOffBits; // 4 Byte
  };
  //}__attribute((packed));
  //#pragma pack()
#pragma pack(pop)

  // In 64 bit linux, long is 8 bytes! The bmp info header has to be 40 bytes!
  // Do not use long but use int instead
  //#pragma pack(1)
#pragma pack(push, 1)
  struct BitmapInfoHeader { // 40 Byte
    unsigned int biSize; // 4 Byte
    //long biWidth; // 8 Byte in 64 bit Linux
    int biWidth; // 4 Byte
    //long biHeight; // 8 Byte in 64 bit Linux
    int biHeight; // 4 Byte
    unsigned short biPlanes; // 2 Byte
    unsigned short biBitCount; // 2 Byte
    unsigned int biCompression; // 4 Byte
    unsigned int biSizeImage; // 4 Byte
    //long biXPelsPerMeter; // 8 Byte in 64 bit Linux
    int biXPelsPerMeter; // 4 Byte
    //long biYPelsPerMeter; // 8 Byte in 64 bit Linux
    int biYPelsPerMeter; // 4 Byte
    unsigned int biClrUsed; // 4 Byte
    //unsigned int biClrImportant:1;
    unsigned int biClrImportant; // 4 Byte
  };
  //}__attribute((packed));
  //#pragma pack()
#pragma pack(pop)

  class BMPLoader : WBResource {
  public:
    static void screenShot(int _x, int _y, int _width, int _height, const char * _filename);
    static int screenshot(int _x, int _y, int _width, int _height, const char * _filename);
    static int loadBMPTexture(const char * _filename);
  };

}

#endif
