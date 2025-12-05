#define SDL_MAIN_HANDLED
#include "internal.h"
#include "cmd/args.h"

#include "utils/logging.h"

struct _engine_data __engine_data;

#include <string.h>
#include <signal.h>
#include <stdio.h>

#ifndef __clang__ 
static void sig_handler(int signal) {}
#endif

static void handle_arg(const char* arg, char* argv[])
{
    size_t j = 0;
    while (1) {
        if (arg[j] != '-') break;
        j++;
    }

    if (j > 0 && j < 3)
        arg += j;
    else
        return;
    
    size_t offset = 2 - j;
    for (size_t i = 0; SRENGINE_ARGS[i].arg; i++)
    {
        if (strcmp(arg, ((const char**)&SRENGINE_ARGS[i])[offset] )) continue;

        SRENGINE_ARGS[i].handler(arg, argv);
        break;
    }
}

int main(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
        handle_arg(argv[i], argv);

	__initialize_engine();
	__run_engine();
}

#ifdef _WIN32
#include <Windows.h>
unsigned int winmain_enter = 0;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    winmain_enter = 1;

    int argc = __argc;
    char** argv = __argv;
    SRENGINE_ARGS[1].handler(NULL, argv);
	return main(argc, argv);
}

#endif