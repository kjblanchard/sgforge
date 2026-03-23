#include <arpa/inet.h>
#include <sgforge/directory.h>
#include <sgforge/header.h>
#include <sgtools/log.h>
#include <stdbool.h>
#include <stdio.h>
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
	return true;
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
		sgLogDebug("Arg num is %d and it is %s\n", i, argv[i]);
		if (!isValidLumpFile(argv[i])) {
			sgLogWarn("Not good file, skipping");
			continue;
		}
		Entry* entry = &entries[entryIndex++];
		FILE* fptr = fopen(argv[i], "rb");
		if (!fptr) {
			sgLogError( "Could not open file!\n");
		}
		strncpy(entry->Name, argv[i], MAX_ENTRY_NAME);
		entry->Offset = currentOffset;
		entry->Size = fileSize(fptr);
		int c;
		// Write the entire file to the wad.
		while ((c = fgetc(fptr)) != EOF) {
			fputc(c, wadFptr);
		}
		fclose(fptr);
		currentOffset += entry->Size;
	}
	header.DirectoryOffset = currentOffset + HEADER_BINARY_SIZE;
	SerializeDirectoryToFileEntries(entries, header.NumLumps, wadFptr);
	rewind(wadFptr);
	SerializeHeaderF(&header, wadFptr);
	fclose(wadFptr);
	return true;
}
