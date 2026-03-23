#include <sgforge/directory.h>
#include <sgforge/unpack.h>
#include <sgtools/log.h>
#include <stdbool.h>
#include <string.h>

Directory* LoadDirectoryFromFile(const char* name) {
	return DeserializeDirectoryFromFile(name);
}

int GetDataFromDirectory(const char* entryName, char** dataBuffer, size_t* dataSize, Directory* directory) {
	int nLumps = directory->Header.NumLumps;
	Entry* foundEntry = NULL;
	for (int i = 0; i < nLumps; ++i) {
		Entry* entry = &directory->Entries[i];
		if (strcmp(entryName, entry->Name) == 0) {
			foundEntry = entry;
			break;
		}
	}

	if (!foundEntry) {
		sgLogError("Could not find entry in wad for %s in wad %s", entryName, directory->FileName);
		return false;
	}
	*dataBuffer = directory->Data + foundEntry->Offset + HEADER_BINARY_SIZE;
	*dataSize = foundEntry->Size;
	return true;
}
