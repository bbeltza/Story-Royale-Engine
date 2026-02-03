const sre_byte* sre_getresource(const char* path, size_t* size_output)
{
    assert(_game_res[0] != '\0');
    assert(size_output != NULL); // Why would you not take the size of a resource? Evil unsafer...

    const sre_byte* res_begin;

    const size_t pathlen = strlen(path) + 1;
    const sre_byte* res_ptr = _game_res;
    while (res_ptr[0] != '\n')
    {
        int cmp = strcmp((const char*)res_ptr, path);
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
        res_ptr += strlen((const char*)(res_ptr)) + 1 + sizeof(size_t);
    }

    errno = ENOENT;
    return NULL;
}

#if _WIN32
#include <Resapi.h> // Only include Resapi.h, we don't need the whole Windows header...

void sre_win_resourceinit(void)
{
    assert(_game_res == NULL);

    HRSRC res = FindResourceA(NULL, "GAME_RES", MAKEINTRESOURCE(RT_RCDATA));
    if (!res)
    {
        static const UCHAR nullbyte = '\0';
        _game_res = &nullbyte;
        return;
    }
    HGLOBAL hglobal = LoadResource(NULL, res);
    _game_res = LockResource(hglobal);
}
#endif