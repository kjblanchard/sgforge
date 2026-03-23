#include <sgforge/directory.h>
#include <sgforge/unpack.h>
#include <sgtools/log.h>
#include <stdbool.h>
#include <string.h>

Directory* LoadDirectoryFromFile(const char* name) {
	return sgDeserializeDirectoryFromFile(name);
}

int GetDataFromDirectory(const char* entryName, char** dataBuffer, size_t* dataSize, Directory* directory) {
	int nLumps = directory->Header.NumLumps;
	Entry* entry = NULL;
	for (int i = 0; i < nLumps; ++i) {
		Entry* cEntry = &directory->Entries[i];
		if (strcmp(entryName, cEntry->Name) == 0) {
			entry = cEntry;
			break;
		}
	}
	if (!entry) {
		sgLogError("Could not find entry in wad for %s in wad %s", entryName, directory->FileName);
		return false;
	}
	*dataBuffer = directory->Data + entry->Offset + HEADER_BINARY_SIZE;
	*dataSize = entry->Size;
	return true;
}
