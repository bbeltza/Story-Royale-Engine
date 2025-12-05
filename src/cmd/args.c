#include <stdio.h>
#include <stdlib.h>
#include "args.h"
#include "utils/logging.h"

static void handle_ac(const char* arg, char* argv[]);

static void handle_help(const char* arg, char* argv[])
{
    fprintf(stderr, "Usage: %s [ OPTIONS... ]\n\n", argv[0]);
    fprintf(stderr, "Options:\n");
    for (size_t i = 0; SRENGINE_ARGS[i].arg; i++)
    {
        fprintf(stderr, "\t--%s, -%s\t\t\t%s\n", SRENGINE_ARGS[i].arg, SRENGINE_ARGS[i].shortcut, SRENGINE_ARGS[i].desc);
    }
    exit(0);
}


const arg_t SRENGINE_ARGS[] = {
    {"help", "h", "Print this message!", handle_help},
    {"alloc-console", "ac", "Allocate a new console (for NO_CONSOLE programs in Windows)", handle_ac},
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