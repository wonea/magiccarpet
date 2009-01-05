#ifndef _RAWLOADER
#define _RAWLOADER

#include <string>
#include <iostream>
#include <fstream>

/**
 * Loads a .raw file into a buffer. The size of the raw file is 
 * used as amount of bytes to allocate, so that no dimensions
 * have to be specified
 * 
 * REMARKS: DOES NOT DELETE THE ALLOCATED BUFFER! DEALLOCATE BUFFER SPACE YOURSELF!
 */
class RAWLoader {
private:
	char * buffer;
public:
	RAWLoader(const std::string fileName);

	char* getData();
};

#endif

