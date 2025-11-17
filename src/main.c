#define SDL_MAIN_HANDLED
#include "internal.h"
#include "args.h"

#include "utils/logging.h"

struct _engine_data __engine_data;

#include <string.h>
#include <signal.h>
#include <stdio.h>

static void sig_handler(int signal) {}

static const char alloc_console[] = "--alloc-console";
static const char ac_shortcut[] = "-ac";

const arg_t SRENGINE_ARGS[] = {
    {"help", "h", "Print this message"},
    {alloc_console+2, ac_shortcut+1, "Allocate a new console (for NO_CONSOLE programs in Windows)"},
    {NULL}
};

#define cmpstr(x, y) (strcmp(x, y) == 0)
#define cmparg(x) cmpstr(arg, x)

const char* option_prefix = "---";

static void handle_arg(const char* arg, char* argv[])
{
    if (strncmp(arg, option_prefix, 1)) return;
    if (strncmp(arg, option_prefix, 3) == 0) return;

    if (strncmp(arg, option_prefix, 2) == 0)
    {
        for (int i = 0; SRENGINE_ARGS[i].arg; i++)
        {
            if (!strcmp(arg+2, SRENGINE_ARGS[i].arg))
            {
                SRENGINE_ARGS[i].handler(arg, argv);
                printf("%s\n", SRENGINE_ARGS[i].arg);
                break;
            }
        }
    }
    else
    {
        for (int i = 0; SRENGINE_ARGS[i].arg; i++)
        {
            if (!strcmp(arg+1, SRENGINE_ARGS[i].shortcut))
            {
                SRENGINE_ARGS[i].handler(arg, argv);
                printf("%s\n", SRENGINE_ARGS[i].arg);
                break;
            }
        }
    }

    /*
    
    if cmparg("--help")
    {
        printf("Usage: %s [ OPTIONS... ]\n\n", argv[0]);
        printf("Options:\n");
        printf("\t-h, --help\t\t\tPrint this message!\n");
        printf("\t-ac, --alloc-console\t\tAllocate a new console while being on a windows subsystem");
        exit(0);
    }
        */
}

int main(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
        handle_arg(argv[i], argv);
	
	//signal(SIGSEGV, sig_handler);

	__initialize_engine();
	__run_engine();
}

#ifdef _WIN32
#include <Windows.h>

static void add_console()
{
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    int argc = __argc;
    char** argv = __argv;

    for (int i=0; i<argc; i++)
    {
        if (!strcmp(argv[i], SRENGINE_ARGS[1].arg-2) || !strcmp(argv[i], SRENGINE_ARGS[1].shortcut-1))
        {
            AllocConsole();
            add_console();
            break;
        }
    }

	return main(argc, argv);
}

#else
static void add_console()
{
    printf("--alloc-console is only supported on Windows\n");
}
#endif