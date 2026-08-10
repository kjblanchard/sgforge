#include <sgforge/directory.h>
#include <sgforge/header.h>
#include <sgforge/unpack.h>
#include <sgtools/log.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <arpa/inet.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

#define WAD_FILENAME "/tmp/test.sg"
static char* sOutputName = WAD_FILENAME;
static bool sExtractMode = false;
static char* sInputFile = NULL;

static bool isValidLumpFile(const char* path) {
	FILE* f = fopen(path, "rb");
	if (!f) return false;
	fclose(f);
	return true;
}

static long fileSize(FILE* f) {
	int prev = ftell(f);
	fseek(f, 0L, SEEK_END);
	long sz = ftell(f);
	fseek(f, prev, SEEK_SET);
	return sz;
}

/* static int countFiles(int totalNumFiles, char* files[]) { */
/* 	int n = 0; */
/* 	for (int i = 1; i < totalNumFiles; ++i) { */
/* 		if (isValidLumpFile(files[i])) ++n; */
/* 	} */
/* 	return n; */
/* } */

static int countFiles(int totalNumFiles, char* files[]) {
	int n = 0;
	for (int i = 1; i < totalNumFiles; ++i) {
		char* a = files[i];
		if (strcmp(a, sOutputName) == 0 || a[0] == '-') {
			continue;
		}
		if (isValidLumpFile(a)) ++n;
	}
	return n;
}

static bool handleArgs(int argc, char* argv[]) {
	if (argc < 2) {
		sgLogWarn("No params passed in, exiting.");
		return false;
	}
	for (int i = 1; i < argc; ++i) {
		char* a = argv[i];
		if (strcmp(a, "-o") == 0) {
			int n = i + 1;
			if (n < argc) {
				sOutputName = argv[n];
			}
		} else if (strcmp(a, "-x") == 0) {
			sExtractMode = true;
			int n = i + 1;
			if (n < argc) {
				sInputFile = argv[n];
			}
		}
	}
	return true;
}

static int extractArchive(void) {
	if (!sInputFile) {
		sgLogWarn("Extract mode requires an input file: sgforge -x <file.sg> -o <output_dir>");
		return 1;
	}
	MKDIR(sOutputName);
	Directory* directory = LoadDirectoryFromFile(sInputFile);
	if (!directory) {
		sgLogWarn("Could not open archive %s", sInputFile);
		return 1;
	}
	for (int i = 0; i < directory->Header.NumLumps; ++i) {
		Entry* entry = &directory->Entries[i];
		char* dataBuffer;
		size_t dataSize;
		if (!GetDataFromDirectory(entry->Name, &dataBuffer, &dataSize, directory)) {
			sgLogWarn("Could not read entry %s", entry->Name);
			continue;
		}
		char outPath[512];
		snprintf(outPath, sizeof(outPath), "%s/%s", sOutputName, entry->Name);
		FILE* fptr = fopen(outPath, "wb");
		if (!fptr) {
			sgLogWarn("Could not write %s", outPath);
			continue;
		}
		fwrite(dataBuffer, 1, dataSize, fptr);
		fclose(fptr);
	}
	sgFreeDirectory(directory);
	return 0;
}

int main(int argc, char* argv[]) {
	if (!handleArgs(argc, argv)) return 1;
	sgSetLogLevel(sgLogLevelWarn);
	if (sExtractMode) return extractArchive();
	sgHeader header;
	strcpy(header.Magic, "sgsav");
	header.Flags = 0;
	header.NumLumps = countFiles(argc, argv);
	Entry* entries = malloc(header.NumLumps * sizeof(*entries));
	/* use entries[] normally */

	FILE* wadFptr = fopen(sOutputName, "wb");
	// Move forward bast the header to write the file contents
	fseek(wadFptr, HEADER_BINARY_SIZE, SEEK_SET);
	size_t currentOffset = 0;
	int entryIndex = 0;
	// 0 is the exe name, start at 1
	for (int i = 1; i < argc; ++i) {
		char* a = argv[i];
		// If the arg is the output name or if it starts with - then we should skip
		if (strcmp(a, sOutputName) == 0 || a[0] == '-') {
			sgLogDebug("current arg %s is the output or an argument, skipping", a);
			continue;
		}
		if (!isValidLumpFile(a)) {
			sgLogWarn("Skipping %s as it isn't a valid file", a);
			continue;
		}
		sgLogDebug("Arg num is %d and it is %s\n", i, a);
		Entry* entry = &entries[entryIndex++];
		FILE* fptr = fopen(a, "rb");
		if (!fptr) {
			sgLogError("Could not open file %s for reading!\n", a);
			continue;
		}
		// Trim off the path to get the basename if it exists
		char* pLastSlash = strrchr(a, '/');
		char* pszBaseName = pLastSlash ? pLastSlash + 1 : a;
		strncpy(entry->Name, pszBaseName, MAX_ENTRY_NAME);
		entry->Offset = currentOffset;
		entry->Size = fileSize(fptr);
		int c;
		// Write the entire file writing to the wad file.
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
	sgSerializeHeader(&header, wadFptr);
	fclose(wadFptr);
	free(entries);
	return 0;
}
