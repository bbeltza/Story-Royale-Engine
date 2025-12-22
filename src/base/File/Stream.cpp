#include "Base/File.hpp"
#include "utils/logging.h"
#include "utils/mem.h"
#include "utils/lockfile.h"

#include "GameSettings.h"

const char File::fsres_prefix[] =
#ifdef ANDROID
        "assets/"
#else
        "__res/"
#endif
        ;
const char File::res_prefix[] = "res://";
const char File::usr_prefix[] = "usr://";

File::~File()
{
    if (!this->isembedded && isValid())
        fclose(this->stream.ptr);
}

File::File(const File& other): currpath(other.currpath), isembedded(other.isembedded)
{
    this->stream.currmode = other.stream.currmode;

    if (this->isembedded)
    {
        this->res = other.res;
        return;
    }

    load_stream();
}

#ifdef WIN32
    extern "C" void SRE_init_resources();
#endif

File::File(const char *path, const char *mode) : currpath(path)
{
    this->stream.currmode = mode;

    #ifdef WIN32
        if (!_game_res)
            SRE_init_resources();
    #endif
    if (path_hasprefix(path, res_prefix))
    {
        if (!mode)
            this->stream.currmode = "rb";

        if (_game_res[0])
        {
            this->load_resource();
            return;
        }

        if (has_write(this->stream.currmode)) // Warn the user
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
    if (has_write(this->stream.currmode)) // You can't modify embedded resources
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
            size_t datapos = reinterpret_cast<const size_t*>(res_ptr)[0];

            this->res.begin = _game_res + datapos + sizeof(size_t);
            this->res.pos = 0;

            size_t size = reinterpret_cast<const size_t*>(this->res.begin)[-1];;
            this->res.size = size;
            this->isembedded = true;

            return;
        }
        res_ptr += strlen(reinterpret_cast<const char *>(res_ptr)) + 1 + sizeof(size_t);
    }

    this->error_notfound();
}

void File::load_stream()
{
    const char* bind_prefix = NULL;
    if (path_hasprefix(this->currpath, res_prefix))
        bind_prefix = fsres_prefix;
    else if (path_hasprefix(this->currpath, usr_prefix))
        bind_prefix = SDL_GetPrefPath(NULL, sre::game_settings.Title);

    if (bind_prefix)
    {
        const char *old = this->currpath;
        ut_dynsalloc(char, buff, strlen(this->currpath) + (strlen(bind_prefix) - PREFIX_LENGTH) + 1);

        strcpy(buff, bind_prefix);
        strcat(buff, this->currpath + sizeof res_prefix - 1);
        this->currpath = buff;

        load_stream();
        this->currpath = old;

        return;
    }


    if (!this->stream.currmode)
    {
        this->stream.ptr = fopen(this->currpath, "r");
        if (!this->stream.ptr)
            this->stream.currmode = "w+";
        else
            this->stream.currmode = "r";
    }

    if (this->stream.ptr)
        this->stream.ptr = freopen(this->currpath, this->stream.currmode, this->stream.ptr);
    else
        this->stream.ptr = fopen(this->currpath, this->stream.currmode);

    if (!this->stream.ptr)
    {
        if (has_write(this->stream.currmode))
        {
            ERROR("Error opening file %s for writing: %s", this->currpath, strerror(errno));
        }
        else
            this->error_notfound();
    }
}

void File::error_notfound()
{
    ERROR("Error opening file %s for reading: %s", this->currpath, strerror(errno));
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
        rw = SDL_RWFromConstMem(res.begin, static_cast<int>(res.size));
    else
    {
#ifdef SDL_HAVE_STDIO_H
        File copy = *this;
        rw = SDL_RWFromFP(copy.stream.ptr, SDL_TRUE);
        if (rw)
            copy.stream.ptr = NULL;
#else
        Chunk chunk = this->allocate();
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

size_t File::stream_size() const
{
    assert(!isembedded);

    long oldpos;
    size_t endpos;

    flockfile(stream.ptr);
        oldpos = ftell_unlocked(stream.ptr);
        fseek_unlocked(stream.ptr, 0, SEEK_END);

        endpos = static_cast<size_t>(ftell_unlocked(stream.ptr));
        fseek_unlocked(stream.ptr, oldpos, SEEK_SET);
    funlockfile(stream.ptr);

    return endpos;
}

size_t File::readBytes(void* buff, size_t count)
{
    if (isembedded)
    {
        const size_t max_count = res.size - res.pos;
        if (count > max_count)
            count = max_count;
        memcpy(buff, res.begin + res.pos, max_count);
        res.pos += max_count;
        return max_count;
    }
    else if (stream.ptr)
        return fread(buff, count, 1, stream.ptr);
    
    ERROR("File is not valid");
    return 0;
}

std::vector<uint8_t> File::readBytes(size_t max_size)
{
    const long curr = ftell(stream.ptr);
    const size_t remaining = getSize() - curr;
    if (!max_size || max_size > remaining)
        max_size = remaining;

    std::vector<uint8_t> vec;
    vec.resize(max_size);
    readBytes(vec.data(), max_size);

    return vec;
}

int File::readF(const char* fmt, ...)
{
    int res;
    va_list va;
    va_start(va, fmt);
    if (isembedded)
        res = vsscanf((const char*)(this->res.begin + this->res.pos), fmt, va);
    else
    {
        flockfile(stream.ptr);

        long oldpos = ftell_unlocked(stream.ptr);
        res = vfscanf(stream.ptr, fmt, va);
        fseek_unlocked(stream.ptr, oldpos, SEEK_SET);

        funlockfile(stream.ptr);
    }

    va_end(va);
    return res;
}

//

size_t File::writeBytes(const void* data, size_t size)
{
    if (isembedded)
    {
        ERROR("Cannot write to read-only embedded resources");
        return 0;
    }

    if (!has_write(stream.currmode))
    {
        ERROR("Cannot write to read-only mode file");
        return 0;
    }

    if (stream.ptr)
    {
        return fwrite(data, size, 1, stream.ptr);
    }

    ERROR("File is not valid for writing");
    return 0;
}