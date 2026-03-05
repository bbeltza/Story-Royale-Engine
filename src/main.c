#define SDL_MAIN_HANDLED
#include "internal.h"
#include "cmd/args.h"

#include "Base/Log.h"

struct _engine_data __engine_data;

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
        const char* curr_arg = ((const char**)&SRENGINE_ARGS[i])[offset];
        int eq = 1;
        for (size_t i = 0; 1; i++)
        {
            if (!curr_arg[i] && arg[i] == '=') break;
            if (!arg[i] && !curr_arg[i]) break;
            if (arg[i] == curr_arg[i]) continue;

            eq = 0;
            break;
        }

        if (!eq) continue;

        SRENGINE_ARGS[i].handler(arg, argv);
        return;
    }

    sre_log(SRE_LOGCATEGORY_WARN, "Unrecognized option: %s", arg-j);
}

int main(int argc, char* argv[])
{
    sre_log(SRE_LOGCATEGORY_DEBUG, "%d", (size_t)(&__engine_data) % 64);
	for (int i = 1; i < argc; i++)
        handle_arg(argv[i], argv);

	__initialize_engine();
	__run_engine();
}

#if _WIN32
#include <Windows.h>
unsigned int winmain_enter = 0;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    winmain_enter = 1;

    int argc = __argc;
    char** argv = __argv;
	return main(argc, argv);
}

#endif