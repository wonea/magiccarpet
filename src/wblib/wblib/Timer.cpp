#include "Timer.h"

wbLib::Timer::Timer() {
  // get the high resolution counter's accuracy
  ::QueryPerformanceFrequency(&ticksPerSecond);
}

/**
 * Starts the timer. The return value of getTime() 
 * gives the time in milliseconds since reset() was
 * called last.
 */
void wbLib::Timer::reset() {
  ::QueryPerformanceCounter(&last);
}

/**
 * The return value of getTime() 
 * gives the time in milliseconds since reset() was
 * called last.
 */
double wbLib::Timer::getTime() {
  LARGE_INTEGER li;
  ::QueryPerformanceCounter(&li);

  double deltaTicks = li.QuadPart - last.QuadPart;
  return (deltaTicks / ((double)ticksPerSecond.QuadPart / 1000.0));
}