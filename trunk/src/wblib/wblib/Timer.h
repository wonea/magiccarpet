#ifndef _TIMER
#define _TIMER

#include <windows.h>
#include <stdio.h>

namespace wbLib {
  class Timer {
  public:
    Timer();
    void reset();
    double getTime();
  private:
    LARGE_INTEGER ticksPerSecond;
    LARGE_INTEGER last;
  };
}
#endif