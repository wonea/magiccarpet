#include "HeightMapLoader.h"

/**
 * This method assumes that the heightmap is stored as a
 * square grayscale, 1 byte per pixel raw image of
 * dimension 1024 x 1024 byte. It loads the data into a buffer.
 */
void wbLib::HeightMapLoader::load(const char * _fileName) {

  // check if file is present, if not throw exception

  // use 1024 x 1024 grayscale raw image format as standard
  //std::cout << __FILE__ << " " << __LINE__ << " Could not find " << _fileName << std::endl;
}

/**
 * This method assumes that the heightmap is stored as a
 * square grayscale, 1 byte per pixel raw image of
 * dimension _width x _height byte. It loads the data into a buffer.
 */
void wbLib::HeightMapLoader::load(const char * _fileName, int _width, int _height) {
}

void wbLib::HeightMapLoader::getBuffer() {
}

void wbLib::HeightMapLoader::cleanUp() {
  std::cout << __FILE__ << " " << __LINE__ << " cleanUp()" << std::endl;
}