#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
struct Directory;
// Loads a directory that can be used to get buffers from
struct Directory* LoadDirectoryFromFile(const char* name);
// Fills dataBuffer and dataSize with info about the data of entryName in Directory directory
int GetDataFromDirectory(const char* entryName, char** dataBuffer, size_t* dataSize, struct Directory* directory);
#ifdef __cplusplus
}
#endif
