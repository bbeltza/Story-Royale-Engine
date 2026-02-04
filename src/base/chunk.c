#include <Base/File.h>
#include <Base/Chunk.h>

#include <utils/logging.h>
#include <utils/lockfile.h>
#include <utils/mem.h>

const sre_Chunk *sre_fileallocate(const sre_File* file, size_t max_size)
{
    if (!file->impl)
    {
        ERROR("Failed to allocate file data. File is not valid");
        return NULL;
    }

    sre_usize filesize = sre_filesize(file);
    if (!filesize)
    {
        WARN("File size will be equal to 0 when allocating chunk");
        return NULL;
    }

    if (max_size > filesize)
        WARN("'max_size' with a value of %zd is larger than the %zd file size", max_size, filesize);
    else
        filesize = max_size ? max_size : filesize;

    sre_Chunk* data = sre_new(sizeof(sre_Chunk) + filesize);
    data->size = filesize;

    if (!sre_fileread(file, data->data, filesize))
        goto CLEANUP_FAIL;

    memset(data->data + filesize, 0, sizeof(data->data));

    return data;

CLEANUP_FAIL:
    sre_delete(data);
    return NULL;
}

void sre_chunkfree(const sre_Chunk* chunk)
{
    sre_delete((void*)chunk);
}