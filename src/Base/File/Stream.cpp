#include "Base/File.hpp"
#include "logging.h"

const char File::fsres_prefix[] = "__res/";
const char File::res_prefix[] = "res://";

File::~File()
{
    if (!this->isembedded && this->stream)
        fclose(this->stream);
}

File::File(const char* path, const char* mode): stream(NULL), currmode(mode), currpath(path)
{    
    if (path_has_resprefix(path))
    {
        if (!mode)
            this->currmode = "rb";
        
        if (_game_res)
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
    if (!_game_res) // In case I call this with _game_res = NULL...
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

    const unsigned char* res_ptr = _game_res;
    while (res_ptr[0] != '\n')
    {
        int cmp = strcmp(reinterpret_cast<const char*>(res_ptr), find_path);
        if (!cmp)
        {
            res_ptr += pathlen;
            int32_t datapos;
            for (uint32_t i = 0; i < sizeof int32_t; i++)
                reinterpret_cast<unsigned char*>(&datapos)[i] = res_ptr[sizeof int32_t - 1 - i];            
            
            this->res_begin = _game_res + datapos + sizeof uint32_t;
            this->res_pos = 0;


            uint32_t size;
            for (uint32_t i = 0; i < sizeof int32_t; i++)
                reinterpret_cast<unsigned char*>(&size)[i] = (this->res_begin - 1 - i)[0];
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
        const char* old = this->currpath;
        #ifndef _MSC_VER
        char buff[strlen(this->currpath)];
        #else
        char* buff = static_cast<char*>(alloca(strlen(this->currpath)+1));
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

bool File::has_write(const char* mode)
{
    return strchr(mode, '+') || strchr(mode, 'a') || strchr(mode, 'w');
}

static int sdlrw_close(SDL_RWops* rw)
{
    if (rw)
    {
        operator delete(rw->hidden.mem.base);
        SDL_FreeRW(rw);
    }

    return 0;
}

SDL_RWops* File::toRWops() const
{
    SDL_RWops* rw = SDL_RWFromFP(stream, SDL_FALSE);

    if (!rw)
    {
        Chunk chunk = const_cast<File*>(this)->allocate();
        rw = SDL_RWFromConstMem(chunk.data, static_cast<int>(chunk.size));
        rw->close = sdlrw_close;

        const_cast<const char*>(chunk.data) = NULL;
    }

    if (!rw)
    {
        ERROR("Failed to create RWops structure from File: %s", SDL_GetError());
        return NULL;
    }

    return rw;
}