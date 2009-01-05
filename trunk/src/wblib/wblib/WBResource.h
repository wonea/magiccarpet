#ifndef _WBRESOURCE
#define _WBRESOURCE

/**
 * A resource might allocate memory dynamically and
 * therefore has a cleanUp() method which frees
 * the memory to prevent memory leaks
 */
class WBResource {
  virtual void cleanUp() = 0;
};

#endif