#pragma once
#include <sgforge/header.h>
#include <stdint.h>
#include <stdio.h>
#define MAX_ENTRY_NAME 64

#ifdef __cplusplus
extern "C" {
#endif

// Only can handle up to about 4gb size of a directory, if we ever get bigger we got big probs
typedef struct Entry {
	char Name[MAX_ENTRY_NAME];
	uint32_t Size;
	uint32_t Offset;
} Entry;

typedef struct Directory {
	sgHeader Header;
	char* FileName;
	Entry* Entries;
	char* Data;
} Directory;

// Serialize a directory to file with entries only.
void sgSerializeDirectoryToFileEntries(Entry entries[], int nEntries, FILE* file);
// cleans up resources
void sgFreeDirectory(Directory* directory);
Directory* sgDeserializeDirectoryFromBuffer(char* buf);
Directory* sgDeserializeDirectoryFromBufferWithHeader(sgHeader* header, char* buf);
Directory* sgDeserializeDirectoryFromFile(const char* filename);
#ifdef __cplusplus
}
#endif
