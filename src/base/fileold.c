#include <GameSettings.h>
#include <Base/File.h>
#include <string.h>

#include <errno.h>

#include <utils/logging.h>
#include <utils/lockfile.h>
#include <utils/mem.h>

#include <SDL_filesystem.h>

#if defined(_WIN32)
    #include <io.h>
#elif __unix__
    #include <unistd.h>
    #define _access access
#endif

extern const char __game_pwdres[];

const char SRE_FSRES_PREFIX[] = "__res/";
const char SRE_RES_PREFIX[] = "res://";
const char SRE_USR_PREFIX[] = "usr://";

void sre_fileclose(sre_File* file)
{
    if (!file) return;
    if (file->embedded) return;
    if (file->fp.fp)
        fclose(file->fp.fp);
    if (file->fp.path)
        free(file->fp.path);
}

static int sre_fileopenfs(sre_File* file, const char* path);
extern void sre_win_resourceinit(void);

int sre_fileopen(sre_File* file, const char* path, const char* mode)
{
    memset(file, 0, sizeof(*file));

    if (mode)
    {
        if (strnlen(mode, sizeof(file->fp.mode)) >= 8)
        {
            ERROR("sre_File: Invalid and/or unknown mode for '%s': %s", path, mode);
            return -1;
        }
        
        strncpy(file->fp.mode, mode, sizeof(file->fp.mode));
    }
    else
        file->fp.mode[0] = '\0';
    
    #ifdef _WIN32
        if (!_game_res)
            sre_win_resourceinit();
    #endif

    if (sre_filehasprefix(path, SRE_RES_PREFIX))
    {
        if (!file->fp.mode[0])
            strcpy(file->fp.mode, "rb");
        
        if (_game_res[0])
        {
            if (sre_modehaswrite(file->fp.mode))
            {
                ERROR("sre_File: Failed opening '%s', file has write access, remove write access to proceed...", path);
                return -1;
            }

            file->res.begin = sre_getresource(path + SRE_FILEPREFIX_LENGTH, &file->res.size);
            if (!file->res.begin)
            {
                ERROR("sre_File: Could not open file '%s': %s", path, strerror(errno));
                return -1;
            }

            file->embedded = 1;
            file->res.pos = 0;
            return 0;
        }

        if (sre_modehaswrite(file->fp.mode))
             WARN("File stream with path '%s' is pointing to a resource with writing rights. Make sure to disable writing rights when opening resources", path);
    }

    return sre_fileopenfs(file, path);
}

static int sre_fileopenfs(sre_File* file, const char* path)
{
    const char* bound_prefix = NULL;
    void (*pref_deallocator)(void*) = NULL;
    if (sre_filehasprefix(path, SRE_RES_PREFIX))
    {
        if (__game_pwdres[0])
            bound_prefix = __game_pwdres;
        else
            bound_prefix = SRE_FSRES_PREFIX;

    }
    else if (sre_filehasprefix(path, SRE_USR_PREFIX))
    {
        bound_prefix = SDL_GetPrefPath(NULL, game_settings.Title);
        pref_deallocator = SDL_free;
    }

    if (bound_prefix)
    {
        ut_dynsalloc(char, buff, strlen(path) + (strlen(bound_prefix) - SRE_FILEPREFIX_LENGTH) + 1);

        strcpy(buff, bound_prefix);
        strcat(buff, path + SRE_FILEPREFIX_LENGTH);

        if (pref_deallocator)
            pref_deallocator((void*)bound_prefix);

        
        return sre_fileopenfs(file, buff);
    }

    {
        size_t pathlen = strlen(path) + 1;
        if (pathlen > 261)
        {
            WARN("Length of the path is bigger than 260, Windows doesn't allow paths up to 260 characters");
            pathlen = 261;
        }



        file->fp.path = malloc(pathlen);
        strncpy(file->fp.path, path, pathlen);
    }

    if (!file->fp.mode[0])
    {
        if (_access(path, 00) == 0)
        {
            strcpy(file->fp.mode, "r");
        }
        else if (errno == ENOENT)
        {
            strcpy(file->fp.mode, "w+");
        }
        else
        {
            ERROR("sre_File: Error opening file '%s': %s", path, strerror(errno));
            return -1;
        }
    }

    if (file->fp.fp)
        file->fp.fp = freopen(path, file->fp.mode, file->fp.fp);
    else
        file->fp.fp = fopen(path, file->fp.mode);

    if (!file->fp.fp)
    {
        ERROR("sre_FILE: Failed opening file '%s': %s", path, strerror(errno));
        return -1;
    }

    return 0;
}

static size_t sre_filesizefs(const sre_File* file)
{
    assert(!file->embedded);

    long oldpos;
    size_t endpos;

    flockfile(file->fp.fp);
        oldpos = ftell_unlocked(file->fp.fp);
        fseek_unlocked(file->fp.fp, 0, SEEK_END);

        endpos = (size_t)ftell_unlocked(file->fp.fp);
        fseek_unlocked(file->fp.fp, oldpos, SEEK_SET);
    funlockfile(file->fp.fp);

    return endpos;
}

size_t sre_filesize(const sre_File* file)
{
    if (!file->fp.fp)
    {
		ERROR("sre_filesize(): could not tell the size of the File, file is not valid.");
        return 0;
    }

    if (file->embedded)
        return file->res.size;
    else
        return sre_filesizefs(file);
}

//

long sre_fileseek(const sre_File* file, long offset, int origin)
{
    if (!file) goto ERROR;
    if (!file->fp.fp) goto ERROR;

    if (file->embedded)
    {
        switch (origin)
        {
            case SEEK_CUR:
            *(size_t*)file->res.pos += offset;
            break;
            case SEEK_END:
            *(size_t*)file->res.pos = file->res.size - offset;
            break;
            case SEEK_SET:
            *(size_t*)file->res.pos = offset;
            break;
        }

        if (file->res.pos < 0) *(size_t*)file->res.pos = 0;
        else if (file->res.pos > file->res.size) *(size_t*)file->res.pos = file->res.size;

        return (long)file->res.pos;
    }
    else
    {
        if (fseek(file->fp.fp, offset, origin) < 0) goto ERROR;
        return ftell(file->fp.fp);
    }

    ERROR:
    return -1;
}

bool sre_filewrite(const sre_File* file, const void* rawdata, size_t size)
{
    if (file->embedded || !file->fp.fp)
        return false;
    if (size == 0)
        return true;

    return fwrite(rawdata, size, 1, file->fp.fp) == 1;
}