#include "Base/File.hpp"
#include "utils/logging.h"
#include "utils/mem.h"
#include "utils/lockfile.h"

#include "GameSettings.hpp"

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
    if (path_hasprefix(path, res_prefix))
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
            size_t datapos = reinterpret_cast<const size_t*>(res_ptr)[0];

            this->res_begin = _game_res + datapos + sizeof(size_t);
            this->res_pos = 0;

            size_t size = reinterpret_cast<const size_t*>(this->res_begin)[-1];;
            this->res_size = size;
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
        bind_prefix = SDL_GetPrefPath(NULL, GameSettings::Title);

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

    flockfile(stream);
        oldpos = ftell_unlocked(stream);
        fseek_unlocked(stream, 0, SEEK_END);

        endpos = static_cast<size_t>(ftell_unlocked(stream));
        fseek_unlocked(stream, oldpos, SEEK_SET);
    funlockfile(stream);

    return endpos;
}

size_t File::readBytes(void* buff, size_t count)
{
    if (isembedded)
    {
        const size_t max_count = res_size - res_pos;
        if (count > max_count)
            count = max_count;
        memcpy(buff, res_begin + res_pos, max_count);
        res_pos += max_count;
        return max_count;
    }
    else
        return fread(buff, count, 1, stream);
}

std::vector<uint8_t> File::readBytes(size_t max_size)
{
    const long curr = ftell(stream);
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
        res = vsscanf((const char*)(res_begin + res_pos), fmt, va);
    else
    {
        flockfile(stream);

        long oldpos = ftell_unlocked(stream);
        res = vfscanf(stream, fmt, va);
        fseek_unlocked(stream, oldpos, SEEK_SET);

        funlockfile(stream);
    }

    va_end(va);
    return res;
}