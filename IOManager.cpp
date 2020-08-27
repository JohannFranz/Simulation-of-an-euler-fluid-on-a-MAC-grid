#include "IOManager.h"
#include <string>
#include <iostream>

//method developed by Jeff Chastine
char* IOManager::readFile(const char* filename) {
	FILE *filepoint;
	errno_t err;

	if ((err = fopen_s(&filepoint, filename, "r")) != 0) {
		std::cout << "File could not be opened." << std::endl;
		return nullptr;
	} 

	fseek(filepoint, 0, SEEK_END);
	long file_length = ftell(filepoint);
	fseek(filepoint,0,SEEK_SET);
	char* contents = new char[file_length+1];

	for(int i =0; i < file_length + 1; i++){
		contents[i] = 0;
	}

	fread(contents, 1, file_length, filepoint);
	contents[file_length+1] = '\0';
	fclose(filepoint);
	return contents;
}

IOManager::IOManager(void)
{
}


IOManager::~IOManager(void)
{
}
