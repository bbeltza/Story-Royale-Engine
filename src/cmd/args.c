#include <stdlib.h>
#include <string.h>
#include "args.h"
#include "utils/logging.h"

static inline void display_option(const arg_t* argument)
{
    fprintf(stderr, "\t--%s, -%s%s\n", argument->arg, argument->shortcut, argument->desc);
}

static void handle_ac(const char* arg, char* argv[]);

static void handle_help(const char* arg, char* argv[])
{
    fprintf(stderr, "Usage: %s [ OPTIONS... ]\n\n", argv[0]);
    fprintf(stderr, "Options:\n");
    for (size_t i = 0; SRENGINE_ARGS[i].arg; i++)
    {
        display_option(SRENGINE_ARGS + i);
    }
    exit(0);
}

static FILE* logfile;
FILE* const* const SRE_LOGFILE = &logfile;
#define LOGFILE_ERROR(...) ERROR("--logfile: " __VA_ARGS__)

static void handle_logfile(const char* arg, char* argv[])
{
    if (logfile)
    {
        LOGFILE_ERROR("Attempt to open log file twice! Maybe you typed --logfile twice?");
        return;
    }

    const char* file = strchr(arg, '=');
    if (!file)
    {
        display_option(SRENGINE_ARGS + 1);
        exit(1);
    }
    file++;

    if (!(*file))
    {
        LOGFILE_ERROR("Could not open log file. Path is empty");
        return;
    }

    logfile = fopen(file, "w");
    if (!logfile)
        LOGFILE_ERROR("Could not open log file %s, maybe access is denied?", file);
}

const arg_t SRENGINE_ARGS[] = {
    {"help", "h", "\t\t\t\tPrint this message!", handle_help},
    {"logfile", "lf", "=[ file name ]\t\tSave logs on a file", handle_logfile},
    {"alloc-console", "ac", "\t\t\tAllocate a new console (for NO_CONSOLE programs in Windows)", handle_ac},
    {NULL}
};


#ifndef _WIN32
static void handle_ac(const char*, char*[])
{
    WARN("Option \"alloc-console\" is only supported on Windows\n");
    return;
}
#else
#include <Windows.h>

static void handle_ac(const char* arg, char* argv[])
{
    if (!winmain_enter)
    {
        WARN("Specified \"alloc-console\" but application already has a console");
        return;
    }

    AllocConsole();
    //SOURCE: https://gist.github.com/myd7349/c9ef3ff31de458b418bbe1bd6410e0e8
    
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stderr);
    freopen("CONOUT$", "w", stdout);
    
    HANDLE hStdout = CreateFile("CONOUT$",  GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hStdin = CreateFile("CONIN$",  GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    SetStdHandle(STD_OUTPUT_HANDLE,hStdout);
    SetStdHandle(STD_ERROR_HANDLE,hStdout);
    SetStdHandle(STD_INPUT_HANDLE,hStdin);
}
#endif