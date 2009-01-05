#include "RAWFile.h"

RAWFile::RAWFile(const std::string fileName) {
	buffer = NULL;

	// std::ios::ate - set the initial position to the end of the file
	std::ifstream file;
	file.open(fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		return;
	}
	int size = file.tellg();
	file.seekg (0, std::ios::beg);

	buffer = new char[size];
	file.read(buffer, size);

	if (!file.good()) {
		delete[] buffer;
		buffer = NULL;
	}

	file.close();
}

/*RAWFile::~RAWFile() {
	if (NULL != buffer) {
		delete[] buffer;
		buffer = NULL;
	}
}*/

char * RAWFile::getData() {
	return buffer;
}

