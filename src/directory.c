#include <arpa/inet.h>
#include <assert.h>
#include <sgforge/directory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void SerializeDirectoryToFileEntries(Entry entries[], int nEntries,
									 FILE* file) {
	for (int i = 0; i < nEntries; ++i) {
		Entry* entry = &entries[i];
		uint32_t size = htonl(entry->Size);
		uint32_t offset = htonl(entry->Offset);
		fwrite(entry->Name, 64, 1, file);
		fwrite(&size, sizeof(size), 1, file);
		fwrite(&offset, sizeof(offset), 1, file);
	}
}
Directory* DeserializeDirectoryFromBufferWithHeader(sgHeader* header,
													char* buf) {
	Directory* directory = malloc(sizeof(*directory));
	directory->Entries = calloc(header->NumLumps, sizeof(Entry));
	size_t entrySize = 64 + (sizeof(uint32_t) * 2);
	for (int i = 0; i < header->NumLumps; ++i) {
		Entry* entry = &directory->Entries[i];
		size_t entryOffset = entrySize * i + header->DirectoryOffset;
		memcpy(entry->Name, buf + entryOffset, 64);
		uint32_t size, offset;
		memcpy(&size, buf + 64 + entryOffset, sizeof(uint32_t));
		memcpy(&offset, buf + 64 + sizeof(uint32_t) + entryOffset, sizeof(uint32_t));
		entry->Offset = ntohl(offset);
		entry->Size = ntohl(size);
	}
	memcpy(&directory->Header, header, sizeof(*header));
	return directory;
}

Directory* DeserializeDirectoryFromFile(const char* filename) {
	sgHeader header;
	char* data;
	// Get the filesize
	FILE* fptr;
	fptr = fopen(filename, "rb");
	if (!fptr) {
		fprintf(stderr, "could not open file\n");
	}
	fseek(fptr, 0, SEEK_END);
	long fsize = ftell(fptr);
	data = malloc(fsize);
	fseek(fptr, 0, SEEK_SET);
	size_t read = fread(data, 1, fsize, fptr);
	if (read != fsize) {
		fprintf(stderr, "fread failed\n");
		free(data);
		fclose(fptr);
		return NULL;
	}
	fclose(fptr);
	DeserializeHeader(data, &header);
	Directory* directory = DeserializeDirectoryFromBufferWithHeader(&header, data);
	/* directory->FileName = strcpy(directory->FileName, filename); */
	directory->FileName = malloc(strlen(filename) + 1);
	directory->Data = data;
	strcpy(directory->FileName, filename);

	return directory;
}

void FreeDirectory(Directory* directory) {
	free(directory->FileName);
	free(directory->Entries);
	free(directory->Data);
	free(directory);
}
