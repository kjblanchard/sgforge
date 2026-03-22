#pragma once

#include <stdint.h>
#include <stdio.h>
// What we write to the magic
#define MAGIC "sgWad"
// Sizeof the full sg header when it is binary
// #define HEADER_BINARY_SIZE (sizeof(uint16_t) * 4) + sizeof(MAGIC)
#define HEADER_BINARY_SIZE (sizeof(uint16_t) * 4) + sizeof(MAGIC)

typedef enum sgHeaderFlags {
	SG_HEADER_NO_FLAGS = 0,
} sgHeaderFlags;

typedef struct sgHeader {
	char Magic[sizeof(MAGIC)];
	uint16_t Flags;
	uint16_t NumLumps;
	uint32_t DirectoryOffset;
} sgHeader;

// serialize a header into a buffer stream buff
void SerializeHeader(sgHeader* header, char* buff);
// serialize a header into a file stream
void SerializeHeaderF(sgHeader* header, FILE* fptr);
// Deserialize a header into dst from the beginning of a buffer stream src;
void DeserializeHeader(char* src, sgHeader* header);
