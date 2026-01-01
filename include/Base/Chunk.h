#ifndef SRE_CHUNK_H
#define SRE_CHUNK_H
#include <C/API.h>

#include <ints.h>

_CAPI_BEGIN

typedef struct sre_Chunk
{
    size_t size;
    sre_byte data[sizeof(size_t)];
} sre_Chunk;

typedef struct sre_File sre_File;

const sre_Chunk *sre_fileallocate(const sre_File* file, size_t max_size);
void sre_chunkfree(const sre_Chunk* chunk);

_CAPI_END

#endif