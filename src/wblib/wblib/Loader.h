#ifndef _LOADER
#define _LOADER

#include "WBResource.h"

namespace wbLib {
  class Loader : WBResource {
  protected:
    char * buffer;

  public:
    Loader();

  protected:
    virtual void load(const char * _fileName) = 0;
    virtual void getBuffer() = 0;
  };
}

#endif