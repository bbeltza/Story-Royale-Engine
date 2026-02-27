#include <Base/RWops.h>
#include <Base/File.h>
#include <Base/Chunk.h>
#include <Base/Log.h>

static int sdlrw_close(SDL_RWops *rw)
{
    if (rw)
    {
        sre_chunkfree((const sre_Chunk*)((const size_t*)rw->hidden.mem.base - 1));
        SDL_FreeRW(rw);
    }

    return 0;
}

SDL_RWops* sre_filetorwops(const sre_File* file)
{
    if (!file) return NULL;
    if (!file->impl) return NULL;

    SDL_RWops *rw;
    size_t size = sre_filesize(file);
    const sre_byte* begin = sre_filebegin(file);

    if (begin)
        rw = SDL_RWFromConstMem(begin, (int)size);
    else
    {
        // I think it's a better idea to not support stdio for reading only
        // since it's slower and heavier than to just allocate a simple chunk
        const sre_Chunk* chunk = sre_fileallocate(file, 0);
        rw = SDL_RWFromConstMem(chunk->data, (int)chunk->size);
        rw->close = sdlrw_close;
    }

    if (!rw)
    {
        sre_log(SRE_LOGCATEGORY_ERROR, "Failed to create RWops structure from File: %s", SDL_GetError());
        return NULL;
    }

    return rw;
}