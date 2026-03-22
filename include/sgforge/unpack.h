#pragma once

#include <stddef.h>
struct Directory;

struct Directory *LoadDirectoryFromFile(const char *name);
int GetDataFromDirectory(const char *entryName, char **dataBuffer,
                         size_t *dataSize, struct Directory *directory);
// int GetDataFromDirectory(char *buffer, const char *name,
//                          struct Directory *directory);
