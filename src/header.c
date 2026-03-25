#include <arpa/inet.h>
#include <assert.h>
#include <sgforge/header.h>
#include <sgtools/log.h>
#include <string.h>

void sgSerializeHeader(sgHeader* header, FILE* fptr) {
	assert(header && fptr && "Bad header or file ptr!");
	strcpy(header->Magic, MAGIC);
	sgLogDebug("header data is serialize %d %d %s\n", header->DirectoryOffset, header->NumLumps, header->Magic);
	uint16_t flags = htons(header->Flags);
	uint16_t nLumps = htons(header->NumLumps);
	uint32_t dOffset = htonl(header->DirectoryOffset);
	fwrite(header->Magic, sizeof(header->Magic), 1, fptr);
	fwrite(&flags, sizeof(flags), 1, fptr);
	fwrite(&nLumps, sizeof(nLumps), 1, fptr);
	fwrite(&dOffset, sizeof(dOffset), 1, fptr);
}

void sgDeserializeHeader(char* src, sgHeader* header) {
	assert(src && header && "Bad buffer or header passed to deserialize header");
	uint32_t offset = 0;
	uint16_t flags, nLumps;
	uint32_t dOffset;
	memcpy(header->Magic, src + offset, sizeof(header->Magic));
	offset += sizeof(header->Magic);
	memcpy(&flags, src + offset, sizeof(flags));
	offset += sizeof(flags);
	memcpy(&nLumps, src + offset, sizeof(nLumps));
	offset += sizeof(nLumps);
	memcpy(&dOffset, src + offset, sizeof(dOffset));
	header->Flags = ntohs(flags);
	header->NumLumps = ntohs(nLumps);
	header->DirectoryOffset = ntohl(dOffset);
	sgLogDebug("header data is %d %d %s\n", header->DirectoryOffset, header->NumLumps, header->Magic);
}
