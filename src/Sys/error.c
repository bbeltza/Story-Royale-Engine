#include <stdio.h>
#include <stdlib.h>
#include "Sys.h"

static char err_buffer[500];

static int exitcode = 0;
static uint8_t exiting = 0;

#define printmsg(stream) fprintf(stream, "[Error]: %s | Code: %d\n", err_buffer, code)

void syserror(syserr code, ...)
{
    va_list va;
    va_start(va, code);

    const char* fmtmsg;
    switch (code)
    {
    case FILE_NOT_FOUND:
        fmtmsg = "Couldn't find %s, no such file or directory.";
        break;
    case SDL_ERROR:
        fmtmsg = "SDL Error: %s '%s'";
        break;
    default:
        fmtmsg = "An error has occurred! %s.";
        break;
    }

    //int len = 
    vsnprintf(err_buffer, 500, fmtmsg, va);

    va_end(va);

    exitcode = code;
    
    printmsg(stderr);
    abort();
}


void sysexit(int code)
{
    exitcode = code;
    exiting = 1;
}

int sysexiting()
{
    return exiting;
}

int syscode()
{
    return exitcode;
}