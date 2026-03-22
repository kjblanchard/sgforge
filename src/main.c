#include <arpa/inet.h>
#include <sgforge/directory.h>
#include <sgforge/header.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define WAD_FILENAME "/tmp/test.sg"

int isValidLumpFile(const char* path) {
	struct stat s;
	// Does it exist?
	if (stat(path, &s) != 0)
		return 0;
	// Is it a directory?
	if (S_ISDIR(s.st_mode))
		return 0;
	// Can we open it?
	FILE* f = fopen(path, "rb");
	if (!f)
		return 0;
	fclose(f);
	return 1;
}

long fileSize(FILE* fp) {
	int prev = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, prev, SEEK_SET);	// go back to where we were
	return sz;
}

int countFiles(int numFiles, char* files[]) {
	int nFiles = 0;
	for (int i = 1; i < numFiles; ++i) {
		if (isValidLumpFile(files[i]))
			++nFiles;
	}
	return nFiles;
}

int main(int argc, char* argv[]) {
	// Handle args
	if (argc < 2) {
		puts("No params passed in, exiting.");
		return 0;
	}

	sgHeader header;
	strcpy(header.Magic, "sgsav");
	header.NumLumps = countFiles(argc, argv);
	Entry entries[header.NumLumps];
	FILE* wadFptr = fopen(WAD_FILENAME, "wb");
	// Move forward bast the header to write the file contents
	fseek(wadFptr, HEADER_BINARY_SIZE, SEEK_SET);

	size_t currentOffset = 0;
	// Check if they are all files, and if so update the directory.
	int entryIndex = 0;
	for (int i = 1; i < argc; ++i) {
		printf("Arg num is %d and it is %s\n", i, argv[i]);
		if (!isValidLumpFile(argv[i])) {
			puts("Not good file, skipping");
			continue;
		}
		Entry* entry = &entries[entryIndex++];
		FILE* fptr = fopen(argv[i], "rb");
		if (!fptr) {
			fprintf(stderr, "COuld not open file!\n");
			;
		}
		strncpy(entry->Name, argv[i], MAX_ENTRY_NAME);
		entry->Offset = currentOffset;
		entry->Size = fileSize(fptr);
		int c;
		// Write the file to the wad.
		while ((c = fgetc(fptr)) != EOF) {
			/* printf("Writing character %c\n", c); */
			fputc(c, wadFptr);
		}
		fclose(fptr);
		currentOffset += entry->Size;
	}
	/* header.DirectoryOffset = currentOffset + sizeof(header); */
	header.DirectoryOffset = currentOffset + HEADER_BINARY_SIZE;
	header.Flags = 0;
	SerializeDirectoryToFileEntries(entries, header.NumLumps, wadFptr);
	rewind(wadFptr);
	SerializeHeaderF(&header, wadFptr);
	fclose(wadFptr);
	return 0;
}
