#include <stdio.h>

static bool stdio_open(sre_FileImpl* impl, const char* path, int flags)
{
    FILE* file;
    if (!flags)
    {
        file = fopen(path, "rb");
        if (file) goto FINISH;
    }

    char mode[8] = "\0\0\0\0\0\0\0";
    {
        bool has_read = flags & SRE_FILE_READ;
        bool has_write = flags & SRE_FILE_WRITE;
        bool has_text = flags & SRE_FILE_TEXT;

        if (has_read)
        {
            mode[0] = 'r';
            if (has_write)
            {
                mode[1] = '+';
                mode[2] = has_text ? '\0' : 'b';
            }
            else
                mode[1] = has_text ? '\0' : 'b';
        }
        else if (has_write)
        {
            mode[0] = 'w';
            mode[1] = has_text ? '\0' : 'b';
        }
    }
    

    file = fopen(path, mode);
    if (!file) return false;

    FINISH:
    *impl = file;
    return true;
}

static void stdio_close(sre_FileImpl impl)
{
    fclose(impl);
}

static sre_usize stdio_read(sre_FileImpl impl, void* data, sre_usize size)
{
    return fread(data, 1, size, impl);
}

static sre_usize stdio_write(sre_FileImpl impl, const void* data, sre_usize size)
{
    return fwrite(data, 1, size, impl);
}

static bool stdio_seek(sre_FileImpl impl, long offset, sre_seek _origin)
{
    int origin = -1;
    switch (_origin)
    {
        case SRE_SEEK_SET: origin = SEEK_SET; break;
        case SRE_SEEK_CUR: origin = SEEK_CUR; break;
        case SRE_SEEK_END: origin = SEEK_END; break;
    }

    assert( origin != -1 );

    return fseek(impl, offset, origin) == 0;
}

static long stdio_tell(sre_FileImpl impl)
{
    return ftell(impl);
}

static sre_usize stdio_size(sre_FileImpl impl)
{
    long offs = ftell(impl);
    if (offs < 0)
        return 0;
    if (fseek(impl, 0, SEEK_END))
        return 0;

    long size = ftell(impl);
    assert(size >= 0);

    fseek(impl, offs, SEEK_SET);

    return (sre_usize)size;
}

static const sre_byte* stdio_begin(sre_FileImpl impl)
{
    (void)impl;
    return NULL;
}

const sre_FVFT SRE_STDIO_VFT = {
    stdio_open,
    stdio_close,

    stdio_read,
    stdio_write,
    
    stdio_seek,
    stdio_tell,

    stdio_size,
    stdio_begin
};