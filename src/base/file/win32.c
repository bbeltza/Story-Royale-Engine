#include <stdio.h>
#include <stdbool.h>

#include <Windows.h>
#include <Base/File.h>

static DWORD mapmodetodesired(int mode)
{
    switch (mode)
    {
        case SRE_FILE_READ: return GENERIC_READ;
        case SRE_FILE_WRITE: return GENERIC_WRITE;
        case SRE_FILE_READWRITE: return GENERIC_READ | GENERIC_WRITE;
        case SRE_FILE_READWRITE_CREATE: return GENERIC_READ | GENERIC_WRITE;
        case SRE_FILE_OVERRIDE: return GENERIC_WRITE;
    }
}

static DWORD mapmodetocreate(int mode)
{
    switch (mode)
    {
        case SRE_FILE_READ:
        case SRE_FILE_READWRITE: return OPEN_EXISTING;
        case SRE_FILE_WRITE:
        case SRE_FILE_OVERRIDE: return GENERIC_WRITE;
        case SRE_FILE_READWRITE_CREATE: return OPEN_ALWAYS;
    }

    return 0;
}


static bool stdio_open(sre_FileImpl* impl, const char* path, int mode)
{
    mode = mode != SRE_FILE_DEFAULT ? mode : SRE_FILE_READ; // Just set the mode to read by default

    HANDLE hfile = CreateFile(path, mapmodetodesired(mode), 0, NULL, mapmodetocreate(mode), 0, NULL);
    if (!hfile)
        return false;

    return true;
}