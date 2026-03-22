#include <arpa/inet.h>
#include <assert.h>
#include <sgforge/header.h>
#include <string.h>
void SerializeHeader(sgHeader *header, char *buff) {
 /* assert(1, "Not implemented"); */
}

void SerializeHeaderF(sgHeader *header, FILE *fptr) {
  strcpy(header->Magic, MAGIC);
  printf("header data is serialize %d %d %s\n", header->DirectoryOffset, header->NumLumps, header->Magic);
  uint16_t flags = htons(header->Flags);
  uint16_t nLumps = htons(header->NumLumps);
  uint32_t dOffset = htonl(header->DirectoryOffset);
  fwrite(header->Magic, sizeof(header->Magic), 1, fptr);
  fwrite(&flags, sizeof(flags), 1, fptr);
  fwrite(&nLumps, sizeof(nLumps), 1, fptr);
  fwrite(&dOffset, sizeof(dOffset), 1, fptr);
}

void DeserializeHeader(char *src, sgHeader *header) {
  assert(src && header && "Bad params passed in");
  uint32_t offset = 0;
  size_t size = 0;
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
  printf("header data is %d %d %s\n", header->DirectoryOffset, header->NumLumps, header->Magic);
}
