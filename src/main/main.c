#define SDL_MAIN_HANDLED
#include "../internal.h"
#include "args.h"

#include <Base/Log.h>
#include <Base/Error.h>
#include <Hints.h>

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

    sre_log(SRE_LOG_WARN "Unrecognized option: %s", arg-j);
}


int main(int argc, char* argv[])
{
    (void)handle_arg; // Note: handle_arg is no longer used, at least for now. Do we need integrated engine argument options?
    const sreArgumentHandler* arg_handler = sre_gethint("ARG_HANDLER");
    if (arg_handler)
    {
        int handlercount;
        {
            const int* argshint = sre_gethint("ARG_HANDLERS");
            if (argshint)
            {
                handlercount = *argshint;
                if (handlercount < 1)
                {
                    sre_log(SRE_LOG_INFO "SRE_HINT_ARG_HANDLERS is lower or equal to zero. SRE_HINT_ARG_HANDLERS has to be a positive non-null signed integer of type `int`. The current value for the hint is '%d'", handlercount);
                    sre_CRITICAL(SRE_ERR_INVALID_HINT, "ARG_HANDLERS", handlercount);

                    return -1;
                }
            }
            else
                handlercount = 1;
            
            for (int i = 0; i < handlercount; i++)
            {
                sreArgumentHandler func = arg_handler[i];
                if (!func)
                {
                    sre_log(SRE_LOG_INFO "SRE_HINT_ARG_HANDLER has a `NULL` function pointer at index '%d'", i);
                    sre_CRITICAL(SRE_ERR_INVALID_HINT, "ARG_HANDLER", func);
                }

                // Run argument handler for every argument, this doesn't include argv[0] since it's the executable name. If you want to use argv[0] it's in the function callback (there's the entire set of `argv`)
                for (int i = 1; i < argc; i++)
                {
                    int status = func(argv[i], argc, argv);
                    if (status != 0) // `func` has to return 0 if it wants to continue running the application
                        return status;
                }
            }
        }
    }
    /*
	for (int i = 1; i < argc; i++)
        handle_arg(argv[i], argv);
    */

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