#include "Base/File.hpp"
#include "utils/logging.h"

const char File::fsres_prefix[] =
#ifdef ANDROID
        "assets/"
#else
        "__res/"
#endif
        ;
const char File::res_prefix[] = "res://";

File::~File()
{
    if (!this->isembedded && this->stream)
        fclose(this->stream);
}

File::File(const File& other): currpath(other.currpath), currmode(other.currmode), isembedded(other.isembedded), stream(NULL)
{
    if (this->isembedded)
    {
        this->res_begin = other.res_begin;
        this->res_size = other.res_size;
        this->res_pos = other.res_pos;
        return;
    }

    load_stream();
}

#ifdef WIN32
    extern "C" void SRE_init_resources();
#endif

File::File(const char *path, const char *mode) : stream(NULL), currmode(mode), currpath(path)
{
    #ifdef WIN32
        if (!_game_res)
            SRE_init_resources();
    #endif
    if (path_has_resprefix(path))
    {
        if (!mode)
            this->currmode = "rb";

        if (_game_res[0])
        {
            this->load_resource();
            return;
        }

        if (has_write(currmode)) // Warn the user
            WARN("File stream with path %s is pointing to a resource with writing rights. Make sure to disable writing rights when opening resources", path);
    }

    this->load_stream();
}

void File::load_resource()
{
    if (!_game_res[0]) // In case I call this with _game_res[0] = NULL...
    {
        ERROR("Misscall to load_resource() when embedded resources are disabled...");
        return;
    }
    if (has_write(this->currmode)) // You can't modify embedded resources
    {
        ERROR("Failed opening resource '%s', file has write access, remove write access to proceed...", this->currpath);
        return;
    }

    const char *find_path = this->currpath + sizeof(res_prefix) - 1;
    const size_t pathlen = strlen(find_path) + 1;

    const unsigned char *res_ptr = _game_res;
    while (res_ptr[0] != '\n')
    {
        int cmp = strcmp(reinterpret_cast<const char *>(res_ptr), find_path);
        if (!cmp)
        {
            res_ptr += pathlen;
            int32_t datapos;
            for (uint32_t i = 0; i < sizeof(int32_t); i++)
                reinterpret_cast<unsigned char *>(&datapos)[i] = res_ptr[sizeof(int32_t) - 1 - i];

            this->res_begin = _game_res + datapos + sizeof(uint32_t);
            this->res_pos = 0;

            uint32_t size;
            for (uint32_t i = 0; i < sizeof(int32_t); i++)
                reinterpret_cast<unsigned char *>(&size)[i] = (this->res_begin - 1 - i)[0];
            this->res_size = size;
            this->isembedded = true;

            return;
        }
        res_ptr += strlen((const char *)res_ptr) + 5;
    }

    this->error_notfound();
}

void File::load_stream()
{
    if (path_has_resprefix(this->currpath))
    {
        const char *old = this->currpath;
#ifndef _MSC_VER
        char buff[strlen(this->currpath) + (sizeof(fsres_prefix) - sizeof(res_prefix))];
#else
        char *buff = static_cast<char *>(alloca(strlen(this->currpath) + (1 + sizeof(fsres_prefix) - sizeof(res_prefix))));
#endif

        strcpy(buff, fsres_prefix);
        strcat(buff, this->currpath + sizeof res_prefix - 1);
        this->currpath = buff;


        load_stream();
        this->currpath = old;

        return;
    }

    if (!this->currmode)
    {
        this->stream = fopen(this->currpath, "r");
        if (!this->stream)
            this->currmode = "w+";
        else
            this->currmode = "r";
    }

    if (this->stream)
        this->stream = freopen(this->currpath, this->currmode, this->stream);
    else
        this->stream = fopen(this->currpath, this->currmode);

    if (!this->stream)
    {
        if (has_write(currmode))
            ERROR("Error opening file %s for writing. File is probably already opened", this->currpath);
        else
            this->error_notfound();
    }
}

void File::error_notfound()
{
    ERROR("Error opening file %s for reading, no such file or directory", this->currpath);
}

bool File::has_write(const char *mode)
{
    return strchr(mode, '+') || strchr(mode, 'a') || strchr(mode, 'w');
}

static int sdlrw_close(SDL_RWops *rw)
{
    if (rw)
    {
        operator delete(rw->hidden.mem.base);
        SDL_FreeRW(rw);
    }

    return 0;
}

SDL_RWops *File::toRWops() const
{
    if (!isValid())
        return NULL;

    SDL_RWops *rw;
    if (isembedded)
        rw = SDL_RWFromConstMem(res_begin, static_cast<int>(res_size));
    else
    {
#ifdef SDL_HAVE_STDIO_H
        File copy = *this;
        rw = SDL_RWFromFP(copy.stream, SDL_TRUE);
        if (rw)
            copy.stream = NULL;
#else
        Chunk chunk = const_cast<File *>(this)->allocate();
        rw = SDL_RWFromConstMem(chunk.data, static_cast<int>(chunk.size));
        rw->close = sdlrw_close;

        if (rw)
            *const_cast<const char **>(&chunk.data) = NULL;
#endif
    }
    if (!rw)
    {
        ERROR("Failed to create RWops structure from File: %s", SDL_GetError());
        return NULL;
    }

    return rw;
}