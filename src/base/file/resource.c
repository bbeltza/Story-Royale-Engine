#include <Base/File.h>

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
#include <Windows.h>

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

struct impl_Res
{
    const sre_byte* begin;
    size_t size;
    size_t pos;
};

//

static bool resource_open(sre_FileImpl* impl, const char* path, int mode)
{
    mode = mode != SRE_FILE_DEFAULT ? mode : SRE_FILE_READ;
    if (mode != SRE_FILE_READ) // Make sure you only state the READ mode to load resources...
    {
        errno = EACCES;
        return false;
    }

    const sre_byte* begin;
    size_t size;

    begin = sre_getresource(path, &size);
    if (!begin)
        return false;

    struct impl_Res* res = sre_new(sizeof(struct impl_Res));
    res->pos = 0;
    res->size = size;
    res->begin = begin;

    *impl = res;
    return true;
}

static void resource_close(sre_FileImpl impl)
{
    sre_delete(impl);
}

//

static size_t resource_read(sre_FileImpl _impl, void* data, size_t size)
{
    struct impl_Res* impl = _impl;
    
    {
        size_t remaining_size = impl->size - impl->pos;
        size = remaining_size < size ? remaining_size : size;
    }

    if (!memcpy(data, impl->begin + impl->pos, size))
        return 0;
    impl->pos += size;
    return size;
}

static size_t resource_write(sre_FileImpl _impl, const void* data, size_t size)
{
    (void)_impl;
    (void)data;
    (void)size;

    return 0;
}

static bool resource_seek(sre_FileImpl _impl, long offset, sre_seek origin)
{
    struct impl_Res* impl = _impl;
    switch (origin)
    {
        case SRE_SEEK_SET:
            impl->pos = offset;
            break;
        case SRE_SEEK_CUR:
            impl->pos += offset;
            break;
        case SRE_SEEK_END:
            impl->pos = impl->size - offset;
            break;
    }

    impl->pos = impl->pos > impl->size ? impl->size : (impl->pos < 0 ? 0 : impl->pos);
    return true;
}

static long resource_tell(sre_FileImpl _impl)
{
    struct impl_Res* impl = _impl;
    return (long)impl->pos;
}

static size_t resource_size(sre_FileImpl _impl)
{
    struct impl_Res* impl = _impl;
    return impl->size;
}

static const sre_byte* resource_begin(sre_FileImpl _impl)
{
    struct impl_Res* impl = _impl;
    return impl->begin;
}

const sre_FVFT SRE_RESOURCE_VFT = {
    resource_open,
    resource_close,

    resource_read,
    resource_write,
    
    resource_seek,
    resource_tell,

    resource_size,
    resource_begin
};