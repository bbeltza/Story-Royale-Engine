#ifndef SRE_CHUNK_H
#define SRE_CHUNK_H
#include <C_API.h>
#include <ints.h>

SRE_CAPI_BEGIN

typedef struct sre_Chunk
{
    size_t size;
    sre_byte data[sizeof(size_t)];
} sre_Chunk;

struct sre_File;

const sre_Chunk *sre_fileallocate(const sre_File* file, size_t max_size);
void sre_chunkfree(const sre_Chunk* chunk);

SRE_CAPI_END

#endif