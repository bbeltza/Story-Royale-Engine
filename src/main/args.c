#include <stdio.h>
#include <stdlib.h>
#include "args.h"

#include <Base/Log.h>

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

const arg_t SRENGINE_ARGS[] = {
    {"help", "h", "\t\t\t\tPrint this message!", handle_help},
    {"alloc-console", "ac", "\t\t\tAllocate a new console (for NO_CONSOLE programs in Windows)", handle_ac},
    {NULL}
};


#ifndef _WIN32
static void handle_ac(const char* arg, char* argv[])
{
    (void)arg;
    (void)argv;
    sre_log(SRE_LOG_WARN "Option \"alloc-console\" is only supported on Windows");
    return;
}
#else
#include <Windows.h>

static void handle_ac(const char* arg, char* argv[])
{
    if (!winmain_enter)
    {
        sre_log(SRE_LOG_WARN "Specified \"alloc-console\" but application already has a console");
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