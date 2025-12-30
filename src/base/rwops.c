#include <Base/RWops.h>
#include <Base/File.h>
#include <Base/Chunk.h>

#include <utils/logging.h>

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
    if (!file->fp.fp)
        return NULL;

    SDL_RWops *rw;
    if (file->embedded)
        rw = SDL_RWFromConstMem(file->res.begin, (int)file->res.size);
    else
    {
#ifdef SDL_HAVE_STDIO_H
        File copy = *this;
        rw = SDL_RWFromFP(copy.stream.ptr, SDL_TRUE);
        if (rw)
            copy.stream.ptr = NULL;
#else
        const sre_Chunk* chunk = sre_fileallocate(file, 0);
        rw = SDL_RWFromConstMem(chunk->data, (int)chunk->size);
        rw->close = sdlrw_close;
#endif
    }
    if (!rw)
    {
        ERROR("Failed to create RWops structure from File: %s", SDL_GetError());
        return NULL;
    }

    return rw;
}