#ifndef _HEIGHTMAPLOADER
#define _HEIGHTMAPLOADER

#include <iostream>
#include "Loader.h"

namespace wbLib {
  class HeightMapLoader : wbLib::Loader {
  public:
    void load(const char * _fileName);
    void load(const char * _fileName, int _width, int _height);
    void getBuffer();
    void cleanUp();
  };
}

#endif