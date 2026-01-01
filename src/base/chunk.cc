#include <Base/File.h>
#include <Base/Chunk.h>

#include <utils/logging.h>
#include <utils/lockfile.h>

const sre_Chunk *sre_fileallocate(const sre_File* file, size_t max_size)
{
    if (!file->fp.fp)
    {
        ERROR("Failed to allocate file data. File is not valid");
        return NULL;
    }

    if (file->embedded)
    {
        sre_Chunk* data = static_cast<sre_Chunk*>(operator new(sizeof(sre_Chunk) + file->res.size));
        data->size = file->res.size;
        memcpy(data->data, file->res.begin, file->res.size);

        WARN("File::allocate(): Allocating embedded resource. It is better to use the raw data instead");
        return data;
    }
    else
    {
        flockfile(file->fp.fp);
        long old_pos = ftell_unlocked(file->fp.fp);

        fseek_unlocked(file->fp.fp, 0L, SEEK_END);
        size_t file_size = ftell_unlocked(file->fp.fp);
        if (!max_size)
            max_size = file_size;
        else if (file_size < max_size)
        {
            WARN("Speficied a size higher than the file size (size: %zd & file size: %d). Using file size", max_size, file_size);
            max_size = file_size;
        }

        rewind(file->fp.fp);

        sre_Chunk *data = static_cast<sre_Chunk *>(operator new(sizeof(size_t) + max_size));
        data->size = max_size;
        fread_unlocked(data->data, max_size, 1, file->fp.fp);

        fseek_unlocked(file->fp.fp , old_pos, SEEK_SET);

        funlockfile(file->fp.fp);

        return data;
    }
}

void sre_chunkfree(const sre_Chunk* chunk)
{
    operator delete(const_cast<sre_Chunk*>(chunk));
}