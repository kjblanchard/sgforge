#include <arpa/inet.h>
#include <sgforge/directory.h>
#include <sgforge/header.h>
#include <sgtools/log.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WAD_FILENAME "/tmp/test.sg"
static char* sOutputName = WAD_FILENAME;

static bool isValidLumpFile(const char* path) {
	FILE* f = fopen(path, "rb");
	if (!f) return false;
	fclose(f);
	return true;
}

static long fileSize(FILE* fp) {
	int prev = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, prev, SEEK_SET);
	return sz;
}

static int countFiles(int numFiles, char* files[]) {
	int nFiles = 0;
	for (int i = 1; i < numFiles; ++i) {
		if (isValidLumpFile(files[i])) ++nFiles;
	}
	return nFiles;
}

static bool handleArgs(int argc, char* argv[]) {
	if (argc < 2) {
		sgLogWarn("No params passed in, exiting.");
		return false;
	}
	// Check to see if we should update the output name when -o is passed
	for (int i = 1; i < argc; ++i) {
		char* a = argv[i];
		if (strcmp(a, "-o") == 0) {
			// Next one is the output filename if it exists
			int n = i + 1;
			if (n < argc) {
				sOutputName = argv[n];
			}
		}
	}

	return true;
}

static char* getFilenameNoPath(const char* f) {
	return NULL;
}

int main(int argc, char* argv[]) {
	if (!handleArgs(argc, argv)) return false;
	sgHeader header;
	strcpy(header.Magic, "sgsav");
	header.Flags = 0;
	header.NumLumps = countFiles(argc, argv);
	Entry entries[header.NumLumps];
	FILE* wadFptr = fopen(sOutputName, "wb");
	// Move forward bast the header to write the file contents
	fseek(wadFptr, HEADER_BINARY_SIZE, SEEK_SET);
	size_t currentOffset = 0;
	// Check if they are all files, and if so update the directory.
	int entryIndex = 0;
	for (int i = 1; i < argc; ++i) {
		char* a = argv[i];
		// If the arg is the output name or if it starts with - then we should skip
		if (strcmp(a, sOutputName) == 0 || a[0] == '-') {
			sgLogDebug("current arg %s is the output or an argument, skipping", a);
			continue;
		}
		sgLogDebug("Arg num is %d and it is %s\n", i, a);
		if (!isValidLumpFile(a)) {
			sgLogWarn("Not good file, skipping");
			continue;
		}
		Entry* entry = &entries[entryIndex++];
		FILE* fptr = fopen(a, "rb");
		if (!fptr) {
			sgLogError("Could not open file!\n");
		}
		char* pLastSlash = strrchr(a, '/');
		char* pszBaseName = pLastSlash ? pLastSlash + 1 : a;
		strncpy(entry->Name, pszBaseName, MAX_ENTRY_NAME);
		entry->Offset = currentOffset;
		entry->Size = fileSize(fptr);
		int c;
		// Write the entire file to the wad.
		while ((c = fgetc(fptr)) != EOF) {
			fputc(c, wadFptr);
		}
		fclose(fptr);
		currentOffset += entry->Size;
		sgLogDebug("Wrote entry %s with offset %d and size %d", entry->Name, entry->Offset, entry->Size);
	}
	header.DirectoryOffset = currentOffset + HEADER_BINARY_SIZE;
	sgSerializeDirectoryToFileEntries(entries, header.NumLumps, wadFptr);
	rewind(wadFptr);
	SerializeHeaderF(&header, wadFptr);
	fclose(wadFptr);
	return true;
}
