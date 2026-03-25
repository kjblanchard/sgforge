#include <arpa/inet.h>
#include <assert.h>
#include <sgforge/directory.h>
#include <sgtools/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sgSerializeDirectoryToFileEntries(Entry entries[], int nEntries, FILE* file) {
	for (int i = 0; i < nEntries; ++i) {
		Entry* e = &entries[i];
		uint32_t sz = htonl(e->Size);
		uint32_t offset = htonl(e->Offset);
		fwrite(e->Name, 64, 1, file);
		fwrite(&sz, sizeof(sz), 1, file);
		fwrite(&offset, sizeof(offset), 1, file);
	}
}
Directory* sgDeserializeDirectoryFromBufferWithHeader(sgHeader* header, char* buf) {
	Directory* d = malloc(sizeof(*d));
	d->Entries = calloc(header->NumLumps, sizeof(Entry));
	static const size_t sz = 64 + (sizeof(uint32_t) * 2);
	for (int i = 0; i < header->NumLumps; ++i) {
		Entry* e = &d->Entries[i];
		size_t entryOffset = sz * i + header->DirectoryOffset;
		memcpy(e->Name, buf + entryOffset, 64);
		uint32_t size, offset;
		memcpy(&size, buf + 64 + entryOffset, sizeof(uint32_t));
		memcpy(&offset, buf + 64 + sizeof(uint32_t) + entryOffset, sizeof(uint32_t));
		e->Offset = ntohl(offset);
		e->Size = ntohl(size);
	}
	memcpy(&d->Header, header, sizeof(*header));
	return d;
}

Directory* sgDeserializeDirectoryFromFile(const char* filename) {
	sgHeader header;
	char* data;
	// Get the filesize
	FILE* fptr;
	fptr = fopen(filename, "rb");
	if (!fptr) {
		sgLogError("could not open file\n");
	}
	fseek(fptr, 0, SEEK_END);
	long fsize = ftell(fptr);
	data = malloc(fsize);
	fseek(fptr, 0, SEEK_SET);
	size_t read = fread(data, 1, fsize, fptr);
	if (read != fsize) {
		sgLogError("fread failed\n");
		free(data);
		fclose(fptr);
		return NULL;
	}
	fclose(fptr);
	sgDeserializeHeader(data, &header);
	Directory* directory = sgDeserializeDirectoryFromBufferWithHeader(&header, data);
	directory->FileName = malloc(strlen(filename) + 1);
	directory->Data = data;
	strcpy(directory->FileName, filename);
	return directory;
}

void sgFreeDirectory(Directory* directory) {
	free(directory->FileName);
	free(directory->Entries);
	free(directory->Data);
	free(directory);
}
