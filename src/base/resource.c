#include <Base/File.h>

#include <utils/logging.h>

const sre_byte* sre_getresource(const char* path, size_t* size_output)
{
    if (!_game_res[0])
    {
        ERROR("Misscall to load_resource() when embedded resources are disabled...");
        return NULL;
    }

    const sre_byte* res_begin;

    const size_t pathlen = strlen(path) + 1;
    const sre_byte *res_ptr = _game_res;
    while (res_ptr[0] != '\n')
    {
        int cmp = strcmp((const char *)res_ptr, path);
        if (!cmp)
        {
            res_ptr += pathlen;
            size_t datapos = ((const size_t*)res_ptr)[0];

            res_begin = _game_res + datapos + sizeof(size_t);

            size_t size = ((const size_t*)res_begin)[-1];
            if (size_output)
                *size_output = size;

            return res_begin;
        }
        res_ptr += strlen((const char *)(res_ptr)) + 1 + sizeof(size_t);
    }

    if (sre_filehasprefix(path, SRE_RES_PREFIX))
    {
        WARN("sre_getresource(): Path '%s' starts with 'res://', the function expects filepaths without it", path);
        return sre_getresource(path + SRE_FILEPREFIX_LENGTH, size_output);
    }

    errno = ENOENT;
    return NULL;
}