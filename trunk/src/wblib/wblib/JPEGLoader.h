// Compiling libjpeg on windows xp:
//
// download jpegsrc.v6b.tar.gz from http://www.ijg.org/files/
// extract it to say c:\jpegsrc.v6b
// c:\jpegsrc.v6b should contain a folder \jpeg-6b
// Copy "c:\jpegsrc.v6b\jpeg-6b\makefile.vc" to the file "c:\jpegsrc.v6b\jpeg-6b\Makefile"
// Copy "c:\jpegsrc.v6b\jpeg-6b\jconfig.vc" to "c:\jpegsrc.v6b\jpeg-6b\jconfig.h"
// At the command prompt:
//
//    * "C:\Program Files\Microsoft Visual Studio .net 2003\Common7\Tools\vsvars32.bat"
//    * cd c:\jpegsrc.v6b\jpeg-6b
//    * nmake -f Makefile 
//    
// edit jmorecfg.h, make it look like this:
//
// INT32 must hold at least signed 32-bit values. 
// #ifndef XMD_H			// X11/xmd.h correctly defines INT32 
// #if !defined( XMD_H ) && !defined( WIN32 )
// typedef long INT32;
// #endif
//
// #ifdef NEED_FAR_POINTERS
// #define FAR  far
// #else
// #undef FAR  // Added by Linden Lab to remove warnings
// #define FAR
// #endif
//    
// Installing: 
// copy "c:\jpegsrc.v6b\jpeg-6b\libjpeg.lib" to C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Lib
// copy "jconfig.h", "jerror.h", "jmorecfg.h", "jpeglib.h", and "jinclude.h" from "c:\jpegsrc.v6b\jpeg-6b\libjpeg.lib" to 
// C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Include
// Select the project > open context menu > properties > configuration properties > linker > input > additional dependencies > add libjpeg.lib
//
// Using:
// DO NOT JUST #include "jpeglib.h" but write the following:
// extern "C" {
// #include "jpeglib.h"
// }
//
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


#ifndef _JPEGFILE
#define _JPEGFILE

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "jconfig.h"
#include "jerror.h"
#include "jmorecfg.h"
extern "C" {
#include "jpeglib.h"
}
#include "jinclude.h"

namespace wbLib {

  /**
   * OpenGL texture info 
   */
  struct gl_texture_t
  {
    GLsizei width;
    GLsizei height;

    GLenum format;
    GLint internalFormat;
    GLuint id;

    GLubyte *texels;
  };

  class JPEGLoader {
  public:
    static struct gl_texture_t * ReadJPEGFromFile(const char *filename);
    static GLuint LoadJPEGTexture(const char *filename);
  };
}

#endif

